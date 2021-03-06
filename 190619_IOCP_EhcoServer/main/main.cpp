
// GetQueuedCompletionPort 세번째 인자 NULL 이면, 나옴

#include "pch.h"
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <process.h>
#include <list>
#include "CRingBuffer.h"

#pragma comment(lib,"ws2_32")
#pragma comment(lib,"winmm")

#define dfSERVER_PORT	9000
#define dfMAX_BUFFER	10000
#define dfMAX_WORKER	10
#define dfMAX_THREAD	(dfMAX_WORKER + 1)
#define dfMAX_TIMEOUT	300000
#define dfMAX_PACKET	512

using namespace std;

typedef struct st_SESSION {
	
	SOCKET		socket;
	SOCKADDR_IN sockAddr;
	OVERLAPPED	sendOverlap;
	OVERLAPPED	recvOverlap;
	CRingBuffer SendQ;
	CRingBuffer RecvQ;
	WSABUF		Sendbuf;
	WSABUF		Recvbuf;
	bool		bSendFlag = false;
	int			iIOCount = 0;

} SESSION;

SOCKET g_listenSock;
HANDLE g_hCP;
HANDLE g_hUpdateExit;
list<SESSION *> g_SessionList;

CRITICAL_SECTION g_csSession;

unsigned int __stdcall AcceptThread(void *args);
unsigned int __stdcall IOThread(void *args);
bool DeleteSession(SESSION *pSession);

int main()
{
	// beginPeriod
	timeBeginPeriod(1);

	// setlocale
	setlocale(LC_ALL, " ");

	// initial Critical
	InitializeCriticalSection(&g_csSession);
		
	// win start
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// IOCP Create
	g_hCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (g_hCP == NULL)
		return 1;

	// socket
	g_listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_listenSock == INVALID_SOCKET)
		return 1;

	// bind
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(dfSERVER_PORT);
	//InetPton(AF_INET, L"0.0.0.0", &serverAddr.sin_addr);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	int retval = bind(g_listenSock, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR) {
		printf("bind()\n");
		return 1;
	}

	// listen
	retval = listen(g_listenSock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		return 1;

	// Create Thread
	HANDLE hThread[dfMAX_THREAD];
	DWORD dwThreadID[dfMAX_THREAD];
	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, AcceptThread, NULL, 0, (unsigned int *)&dwThreadID[0]);
	for(int i=1; i<dfMAX_THREAD; i++)
		hThread[i] = (HANDLE)_beginthreadex(NULL, 0, IOThread, NULL, 0, (unsigned int *)&dwThreadID[i]);
	for (int i = 0; i < dfMAX_THREAD; i++) {
		if (hThread[i] == NULL)
			return 1;

		// closehandle 해줘야 함?
	}

	//printf("ID : %d, %d\n", dwThreadID[0], dwThreadID[1]);

	// CreateEvent
	g_hUpdateExit = CreateEvent(NULL, FALSE, FALSE, NULL);

	for (;;) {

		// SPACE 키가 눌렸다면 종료
		if (GetAsyncKeyState(VK_SPACE) & 0x8001) {

			// Update 종료
			SetEvent(g_hUpdateExit);

			// Accept 종료
			closesocket(g_listenSock);

			// Post쳐줘야 함
			PostQueuedCompletionStatus(g_hCP, 0, NULL, NULL);
		}

		DWORD dwRet = WaitForMultipleObjects(dfMAX_THREAD, hThread, true, 500);
		if (dwRet != WAIT_TIMEOUT) {
			break;
		}
	}

	// closehandle
	for (int i = 0; i < dfMAX_THREAD; i++)
		CloseHandle(hThread[i]);

	// WSACleanup
	WSACleanup();

	// delete Critical
	DeleteCriticalSection(&g_csSession);

	// EndPeriod
	timeEndPeriod(1);

	return 0;
}

unsigned int __stdcall AcceptThread(void *args) {

	SOCKET		clientSock;
	SOCKADDR_IN clientAddr;
	int			clientLen;
	DWORD		dwRecvByte;
	DWORD		dwFlag;
	
	printf("Enter Accept Thread\n");

	for (;;) {

		clientLen = sizeof(clientAddr);
		clientSock = accept(g_listenSock, (SOCKADDR *)&clientAddr, &clientLen);
		if (clientSock == INVALID_SOCKET) {
			break;
		}

		//=============================================
		// 정상 로직
		//=============================================

		// 세션 추가
		SESSION *pSession = new SESSION;
		pSession->socket = clientSock;
		pSession->sockAddr = clientAddr;
		pSession->bSendFlag = false;
		pSession->Recvbuf.buf = pSession->RecvQ.GetRearBufferPtr();
		pSession->Recvbuf.len = pSession->RecvQ.GetEnqSize();
		pSession->Sendbuf.buf = pSession->SendQ.GetFrontBufferPtr();
		pSession->Sendbuf.len = pSession->SendQ.GetDeqSize();
		memset(&pSession->recvOverlap, 0, sizeof(OVERLAPPED));
		memset(&pSession->sendOverlap, 0, sizeof(OVERLAPPED));

		// Create IOCP
		CreateIoCompletionPort((HANDLE)clientSock, g_hCP, (ULONG_PTR)pSession, 0);

		// WSARecv
		dwRecvByte = 0;
		dwFlag = 0;
		memset(&pSession->recvOverlap, 0, sizeof(OVERLAPPED));
		int iRet = WSARecv(clientSock, &pSession->Recvbuf, 1, &dwRecvByte,
			&dwFlag, &pSession->recvOverlap, NULL);
		
		if (iRet == SOCKET_ERROR ) {
		
			int iError = WSAGetLastError();
			if (iError != WSA_IO_PENDING) {

				// 에러가 언제 뜨지?
				// PENDING -> 지금 당장 들어온거 없음, WOULDBLOCK 대신
				// 소켓 해제, Session 삭제
				closesocket(clientSock);
				delete pSession;
			}
			continue;
		}

		// 정상적인 클라 세션
		EnterCriticalSection(&g_csSession);
		g_SessionList.push_back(pSession);
		LeaveCriticalSection(&g_csSession);

		//printf("[Accept Thread] Client IP : %d, Client Port : %d\n", 
			//inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
	}

	printf("Accept Thread Exit\n");

	return 0;
}

unsigned int __stdcall IOThread(void *args) {

	DWORD dwTransfer;
	SESSION *pSession;
	OVERLAPPED *pOverlap;
	DWORD dwSendByte;
	DWORD dwRecvByte;
	DWORD dwFlag;

	printf("Enter IO Thread\n");

	for (;;) {

		dwTransfer = 0;
		pSession = NULL;
		pOverlap = NULL;
		GetQueuedCompletionStatus(g_hCP, &dwTransfer, (PULONG_PTR)&pSession, &pOverlap, INFINITE);
		
		// GQCS 가 false 나는 경우
		// 1) IO 작업 실패시
		// 2) IOCP가 

		// PostQueuedCompletionStatus에 의한 스레드 종료
		if (dwTransfer == 0 && pSession == NULL && pOverlap == NULL)
			break;

		if (pOverlap == NULL)
			continue;

		if (pSession == NULL)
			continue;

		// dwTransfer == 0 && pOverlap != NULL
		if (dwTransfer == 0) {

			// 세션종료처리
			closesocket(pSession->socket);
			bool bRet = DeleteSession(pSession);
			if (bRet)
				delete pSession;
			else
				printf("Erase False\n");

			continue;
		}

		// 정상 로직
		if (pOverlap == &pSession->sendOverlap && pSession->bSendFlag == false) {

			dwSendByte = 0;

			pSession->Sendbuf.buf = pSession->SendQ.GetFrontBufferPtr();
			pSession->Sendbuf.len = pSession->SendQ.GetDeqSize();
			
			pSession->bSendFlag = true;

			int iRet = WSASend(pSession->socket, &pSession->Sendbuf, 1, &dwSendByte, 0, &pSession->sendOverlap, NULL);
			if (iRet == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING) {

				shutdown(pSession->socket, SD_BOTH);
				continue;
			}

			pSession->bSendFlag = false;

			// timeout
			pSession->dwTimeout = timeGetTime();

			// MoveFront
			pSession->RecvQ.MoveFront(dwSendByte);

		}

		else if (pOverlap == &pSession->recvOverlap) {

			dwRecvByte = 0;
			dwFlag = 0;
			
			pSession->Recvbuf.buf = pSession->RecvQ.GetRearBufferPtr();
			pSession->Recvbuf.len = pSession->RecvQ.GetEnqSize();
			int iRet = WSARecv(pSession->socket, &pSession->Recvbuf, 1, &dwRecvByte, &dwFlag, &pSession->recvOverlap, NULL);
			if (iRet == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING) {

				shutdown(pSession->socket, SD_BOTH);
				continue;
			}

			// timeout
			pSession->dwTimeout = timeGetTime();
			
			// MoveRear
			pSession->RecvQ.MoveRear(dwRecvByte);
		}
	}

	printf("Exit IO Thread\n");

	// 스레드 연계 종료
	PostQueuedCompletionStatus(g_hCP, 0, NULL, NULL);

	return 0;
}

bool DeleteSession(SESSION *pSession) {

	bool bRet = false;

	EnterCriticalSection(&g_csSession);

	list<SESSION *>::iterator SessionIter = g_SessionList.begin();
	list<SESSION *>::iterator SessionIterEnd = g_SessionList.end();

	for (; SessionIter != SessionIterEnd; ++SessionIter) {

		if (pSession == (*SessionIter)) {

			g_SessionList.erase(SessionIter);

			bRet = true;
			break;
		}
	}

	LeaveCriticalSection(&g_csSession);

	return bRet;
}
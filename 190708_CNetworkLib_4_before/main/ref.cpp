//
//// GetQueuedCompletionPort 세번째 인자 NULL 이면, 나옴
//
//#include <iostream>
//#include <WinSock2.h>
//#include <WS2tcpip.h>
//#include <process.h>
//#include <list>
//#include "CRingBuffer.h"
//#include "CPacket.h"
//
//#pragma comment(lib,"ws2_32")
//#pragma comment(lib,"winmm")
//
//#define dfSERVER_PORT	6000
//#define dfMAX_BUFFER	1000
//#define dfMAX_TIMEOUT	300000
//#define dfMAX_PACKET	512
//#define dfMAX_IO_COUNT	10
//#define dfMAX_THREAD	(dfMAX_IO_COUNT + 1)
//
//using namespace std;
//
//typedef struct HEADER{
//
//	short shLen;
//};
//
//typedef struct st_SESSION {
//	
//	SOCKET		socket;
//	SOCKADDR_IN sockAddr;
//	OVERLAPPED	sendOverlap;
//	OVERLAPPED	recvOverlap;
//	CRingBuffer SendQ;
//	CRingBuffer RecvQ;
//	int			IOCount = 0;
//	long		lSendFlag = false;
//	DWORD		dwSendByte = 0;
//
//} SESSION;
//
//SOCKET g_listenSock;
//HANDLE g_hCP;
//HANDLE g_hUpdateExit;
//list<SESSION *> g_SessionList;
//CRITICAL_SECTION g_csSession;
//
//unsigned int __stdcall AcceptThread(void *args);
//unsigned int __stdcall IOThread(void *args);
//int	RecvPost(SESSION *pSession);
//int	SendPost(SESSION *pSession);
//int SendPacket(SESSION *pSession, CPacket *pPack);
//bool ReleaseSession(SESSION *pSession);
//
//int main()
//{
//	// beginPeriod
//	timeBeginPeriod(1);
//
//	// setlocale
//	setlocale(LC_ALL, " ");
//
//	// initial Critical
//	InitializeCriticalSection(&g_csSession);
//		
//	// win start
//	WSADATA wsa;
//	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
//		return 1;
//
//	// IOCP Create
//	g_hCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
//	if (g_hCP == NULL)
//		return 1;
//
//	// socket
//	g_listenSock = socket(AF_INET, SOCK_STREAM, 0);
//	if (g_listenSock == INVALID_SOCKET)
//		return 1;
//
//	// bind
//	SOCKADDR_IN serverAddr;
//	ZeroMemory(&serverAddr, sizeof(serverAddr));
//	serverAddr.sin_family = AF_INET;
//	serverAddr.sin_port = htons(dfSERVER_PORT);
//	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
//	int retval = bind(g_listenSock, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
//	if (retval == SOCKET_ERROR) {
//		printf("bind()\n");
//		return 1;
//	}
//
//	// listen
//	retval = listen(g_listenSock, SOMAXCONN);
//	if (retval == SOCKET_ERROR)
//		return 1;
//
//	// Create Thread
//	HANDLE hThread[dfMAX_THREAD];
//	DWORD dwThreadID[dfMAX_THREAD];
//	
//	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, AcceptThread, NULL, 0, (unsigned int *)&dwThreadID[0]);
//	for(int i=1; i<dfMAX_THREAD; i++)
//		hThread[i] = (HANDLE)_beginthreadex(NULL, 0, IOThread, NULL, 0, (unsigned int *)&dwThreadID[i]);
//	
//	for (int i = 0; i < dfMAX_THREAD; i++) {
//		if (hThread[i] == NULL)
//			return 1;
//	}
//	
//	DWORD dwRet = 0;
//	for (;;) {
//
//		// SPACE 키가 눌렸다면 종료
//		if (GetAsyncKeyState(VK_SPACE) & 0x8001) {
//
//			// Accept 종료
//			closesocket(g_listenSock);
//
//			// Post쳐줘야 함
//			PostQueuedCompletionStatus(g_hCP, 0, NULL, NULL);
//		}
//
//		dwRet = WaitForMultipleObjects(dfMAX_THREAD, hThread, true, 200);
//		if (dwRet != WAIT_TIMEOUT) {
//			printf("End of All Thread\n");
//			break;
//		}
//	}
//
//	// closehandle
//	for (int i = 0; i < dfMAX_THREAD; i++)
//		CloseHandle(hThread[i]);
//
//	// closesocket
//	closesocket(g_listenSock);
//
//	// WSACleanup
//	WSACleanup();
//
//	// delete Critical
//	DeleteCriticalSection(&g_csSession);
//
//	// EndPeriod
//	timeEndPeriod(1);
//
//	return 0;
//}
//
//unsigned int __stdcall AcceptThread(void *args) {
//
//	SOCKET		clientSock = NULL;
//	SOCKADDR_IN clientAddr;
//	int			clientLen = 0;
//	DWORD		dwRecvByte = 0;
//	DWORD		dwFlag = 0;
//	
//	printf("Enter Accept Thread\n");
//
//	for (;;) {
//
//		clientLen = sizeof(clientAddr);
//		clientSock = accept(g_listenSock, (SOCKADDR *)&clientAddr, &clientLen);
//		if (clientSock == INVALID_SOCKET) {
//			printf("accept fail\n");
//			break;
//		}
//
//		//=============================================
//		// 정상 로직
//		//=============================================
//
//		// 세션 추가
//		SESSION *pSession = new SESSION;
//		pSession->socket = clientSock;
//		pSession->sockAddr = clientAddr;
//		pSession->lSendFlag = false;
//
//		EnterCriticalSection(&g_csSession);
//		g_SessionList.push_back(pSession);
//		LeaveCriticalSection(&g_csSession);
//
//		// Create IOCP
//		CreateIoCompletionPort((HANDLE)clientSock, g_hCP, (ULONG_PTR)pSession, 0);
//
//		// WSARecvPost
//		RecvPost(pSession);
//		
//		// 정상적인 클라 세션
//		/*EnterCriticalSection(&g_csSession);
//		g_SessionList.push_back(pSession);
//		LeaveCriticalSection(&g_csSession);
//*/
//		WCHAR wchBuf[22] = { 0, };
//		wprintf(L"[Accept Thread] Client IP : %s, Client Port : %d\n",
//			InetNtop(AF_INET,&clientAddr.sin_addr, wchBuf, 22), ntohs(clientAddr.sin_port));
//	}
//
//	printf("Accept Thread Exit\n");
//
//	return 0;
//}
//
//unsigned int __stdcall IOThread(void *args) {
//
//	DWORD dwTransfer;
//	SESSION *pSession;
//	OVERLAPPED *pOverlap;
//	DWORD dwSendByte;
//	DWORD dwRecvByte;
//	DWORD dwFlag;
//	DWORD dwId = GetThreadId(GetCurrentThread());
//	
//	printf("Enter IO Thread\n");
//
//	for (;;) {
//
//		dwTransfer = 0;
//		pSession = NULL;
//		pOverlap = NULL;
//		GetQueuedCompletionStatus(g_hCP, &dwTransfer, (PULONG_PTR)&pSession, &pOverlap, INFINITE);
//		
//		//printf("Pass GQCS\n");
//		// GQCS 가 false 나는 경우
//		// 1) IO 작업 실패시
//		// 2) IOCP가 멈춘경우
//				
//		// PostQueuedCompletionStatus에 의한 스레드 종료
//		if (dwTransfer == 0 && pSession == NULL && pOverlap == NULL)
//			break;
//
//		if (pOverlap == NULL)
//			continue;
//
//		if (pSession == NULL)
//			continue;
//
//		// dwTransfer == 0 && pOverlap != NULL
//		if (dwTransfer == 0) {
//
//			// 세션종료처리
//			printf("[%d] Transfer 0 \n", GetThreadId(GetCurrentThread()));
//			shutdown(pSession->socket, SD_BOTH);
//		}
//
//		// 정상 로직
//		if (pOverlap == &pSession->sendOverlap) {
//			
//			pSession->SendQ.MoveFront(pSession->dwSendByte);
//			InterlockedExchange((long *)&pSession->lSendFlag, false);
//			
//			SendPost(pSession);
//		}
//
//		else if (pOverlap == &pSession->recvOverlap) {
//			
//			/*CPacket Pack(2000);
//			CPacket SendPack(2000);
//			Pack.Clear();
//			SendPack.Clear();
//
//			pSession->RecvQ.MoveRear(dwTransfer);
//			int iSize = pSession->RecvQ.Dequeue(Pack.GetWritePtr(), pSession->RecvQ.GetUseSize());
//			
//			Pack.MoveWritePos(iSize);
//
//			SendPacket(pSession, &Pack);
//			RecvPost(pSession);*/
//			
//			// Values
//			int iRet = 0;
//			int iRecvUseSize = 0;
//			int iRetDeqSize = 0;
//			HEADER Header = { 0, };
//			CPacket RecvPack(20);	// 2000 아니어도 될 여지 있음
//
//			// 받은 데이터 Byte만큼 RecvQ 업데이트
//			pSession->RecvQ.MoveRear(dwTransfer);
//
//			//pSession->RecvQ.PrintBuffer();
//			//printf("Recv \n");
//
//			// Peek
//			while (1) {
//
//				// init
//				iRecvUseSize = pSession->RecvQ.GetUseSize();
//				iRetDeqSize = 0;
//				Header = { 0, };
//				RecvPack.Clear();
//
//				// RecvQ 안에 헤더 사이즈 만큼의 데이터도 없으면, break;
//				if (iRecvUseSize < sizeof(HEADER))
//					break;
//
//				// 헤더 Peek
//				//printf("sizeof Head : %d\n", sizeof(HEADER));
//				pSession->RecvQ.Peek((char*)&Header, sizeof(HEADER));
//
//				// Header에 있는 Len만큼 페이로드가 RecvQ에 있는지, 없으면 break;
//				if (iRecvUseSize < sizeof(HEADER) + Header.shLen)
//					break;
//
//				// 완전한 패킷이 RecvQ에 있으므로 먼저 헤더 RecvQ에서 빼기
//				pSession->RecvQ.MoveFront(sizeof(HEADER));
//
//				// 페이로드 Dequeue -> Pack
//
//				iRetDeqSize = pSession->RecvQ.Dequeue(RecvPack.GetWritePtr(), Header.shLen);
//
//				// Pack Size 업데이트
//				RecvPack.MoveWritePos(iRetDeqSize);
//
//				// OnRecv
//				CPacket SendPack;
//				SendPack.Clear();
//
//				// Payload
//				__int64 i64Data = 0;
//				RecvPack >> i64Data;
//
//				SendPack << i64Data;
//
//				HEADER Header = { 0, };
//				Header.shLen = SendPack.GetPacketSize();
//
//				// 헤더 + Packet -> SendQ에 Enq
//				pSession->SendQ.Enqueue((char *)&Header, sizeof(HEADER));
//				pSession->SendQ.Enqueue(SendPack.GetReadPtr(), SendPack.GetPacketSize());
//
//				//printf("SendPacket Try\n");
//				iRet = SendPost(pSession);
//			}
//
//			RecvPost(pSession);
//		}
//
//		if (!InterlockedDecrement((long *)&pSession->IOCount)) {
//
//			printf("[%d] IOCount 0 \n", GetThreadId(GetCurrentThread()));
//			closesocket(pSession->socket);
//			ReleaseSession(pSession);
//
//			continue;
//		}
//	}
//
//	printf("Exit IO Thread\n");
//
//	// 스레드 연계 종료
//	PostQueuedCompletionStatus(g_hCP, 0, NULL, NULL);
//
//	return 0;
//}
//
//int RecvPost(SESSION * pSession)
//{
//	DWORD dwFlag = 0;
//	DWORD dwRecvByte = 0;
//	SESSION *pLocalSession = pSession;
//	WSABUF	WSARecvBuf[2];
//	int		NumOfBuf = 1;
//	int		iError = 0;
//	int		iEnqSize = 0;
//	int		iFreeSize = 0;
//	int		iRemainSize = 0;
//
//	// 정상로직
//	
//	iEnqSize = pLocalSession->RecvQ.GetEnqSize();
//	iFreeSize = pLocalSession->RecvQ.GetFreeSize();
//	iRemainSize = iFreeSize - iEnqSize;
//	WSARecvBuf[0].buf = pLocalSession->RecvQ.GetRearBufferPtr();
//	WSARecvBuf[0].len = iEnqSize;
//	
//	if (iRemainSize >= 0) {
//
//		WSARecvBuf[1].buf = pLocalSession->RecvQ.GetBufferPtr();
//		WSARecvBuf[1].len = iRemainSize;
//		NumOfBuf = 2;
//	}
//
//	memset(&pLocalSession->recvOverlap, 0, sizeof(OVERLAPPED));
//
//	InterlockedIncrement((long *)&pLocalSession->IOCount);
//	//printf("[%d] Recv ReqSize : %d\n", pSession, iFreeSize);
//	int iRet = WSARecv(pLocalSession->socket, WSARecvBuf, NumOfBuf, &dwRecvByte, &dwFlag, &pLocalSession->recvOverlap, NULL);
//	iError = WSAGetLastError();
//	if (iRet == SOCKET_ERROR && iError != ERROR_IO_PENDING) {
//
//		if (!InterlockedDecrement((long *)&pLocalSession->IOCount)) {
//
//			printf("[%d] Recv IOCount 0 \n", GetThreadId(GetCurrentThread()));
//			closesocket(pSession->socket);
//			ReleaseSession(pSession);
//		}
//		else {
//			printf("[%d] Recv SockError \n", GetThreadId(GetCurrentThread()));
//			shutdown(pSession->socket, SD_BOTH);
//		}
//	}
//
//	return iRet;
//}
//
//int SendPost(SESSION * pSession)
//{	
//	DWORD dwSendByte = 0;
//	SESSION *pLocalSession = pSession;
//	WSABUF	WSASendBuf[2];
//	int		NumOfBuf = 1;
//	int		iError = 0;
//	int		iDeqSize = 0;
//	int		iUseSize = 0;
//	int		iRemainSize = 0;
//
//	// F->T
//	if ((bool)InterlockedExchange((long *)&pSession->lSendFlag, true) == true) {
//
//		return 1;
//	}
//
//	iUseSize = pSession->SendQ.GetUseSize();
//	if (iUseSize <= 0) {
//
//		InterlockedExchange((long *)&pSession->lSendFlag, false);
//		//printf("UseSize 0\n");
//		return 0;
//	}
//
//	// 정상 로직
//	
//	iDeqSize = pLocalSession->SendQ.GetDeqSize();
//	iRemainSize = iUseSize - iDeqSize;
//	WSASendBuf[0].buf = pLocalSession->SendQ.GetFrontBufferPtr();
//	WSASendBuf[0].len = iDeqSize;
//	
//	if (iRemainSize >= 0) {
//
//		WSASendBuf[1].buf = pLocalSession->SendQ.GetBufferPtr();
//		WSASendBuf[1].len = iRemainSize;
//		NumOfBuf = 2;
//	}
//
//	pLocalSession->dwSendByte = iUseSize;
//
//	memset(&pLocalSession->sendOverlap, 0, sizeof(OVERLAPPED));
//
//	// Count 올리기
//	InterlockedIncrement((long *)&pLocalSession->IOCount);
//	//printf("[%d] Send ReqSize : pSession->dwSendByte - %d\n", pSession, pSession->dwSendByte);
//	int iRet = WSASend(pLocalSession->socket, WSASendBuf, NumOfBuf, &dwSendByte, 0, &pLocalSession->sendOverlap, NULL);
//	iError = WSAGetLastError();
//	if (iRet == SOCKET_ERROR && iError != ERROR_IO_PENDING) {
//
//		if (!InterlockedDecrement((long *)&pLocalSession->IOCount)) {
//
//			printf("[%d] Send IOCount 0 \n", GetThreadId(GetCurrentThread()));
//			closesocket(pSession->socket);
//			ReleaseSession(pSession);
//		}
//
//		else {
//
//			printf("[%d] Send SockError \n", GetThreadId(GetCurrentThread()));
//			shutdown(pSession->socket, SD_BOTH);
//		}
//	}
//
//	return iRet;
//}
//
//int SendPacket(SESSION * pSession, CPacket * pPack)
//{
//	SESSION *pLocalSession = pSession;
//	CPacket *pLocalPack = pPack;
//	int iRet = 0;
//
//	pLocalSession->SendQ.Enqueue(pLocalPack->GetBufferPtr(), pLocalPack->GetPacketSize());
//
//	//printf("SendPacket Try\n");
//	iRet = SendPost(pSession);
//	//if (iRet == -2)
//	//	printf("SendPacket False\n");
//	
//	//printf("SendPacket Success\n");
//	return iRet;
//}
//
//bool ReleaseSession(SESSION *pSession) {
//
//	bool bRet = false;
//	WCHAR wchBuf[22] = { 0, };
//	if (pSession == NULL)
//		return bRet;
//
//	EnterCriticalSection(&g_csSession);
//
//	list<SESSION *>::iterator SessionIter = g_SessionList.begin();
//	list<SESSION *>::iterator SessionIterEnd = g_SessionList.end();
//
//	for (; SessionIter != SessionIterEnd; ++SessionIter) {
//
//		if (pSession == (*SessionIter)) {
//
//			g_SessionList.erase(SessionIter);
//			
//			wprintf(L"[Release Thread] Client IP : %s, Client Port : %d\n",
//				InetNtop(AF_INET, &pSession->sockAddr.sin_addr, wchBuf, 22), ntohs(pSession->sockAddr.sin_port)); 
//			delete pSession;
//			bRet = true;
//
//			break;
//		}
//	}
//
//	LeaveCriticalSection(&g_csSession);
//
//	return bRet;
//}

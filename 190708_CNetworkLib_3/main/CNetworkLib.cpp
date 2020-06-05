
#include "CNetworkLib.h"
#include "CGameServer.h"
#include <process.h>

CNetworkLib::CNetworkLib()
{
	NetInit();
}

CNetworkLib::~CNetworkLib()
{
	DWORD dwRet = 0;

	// closesocket(listenSock) -> AcceptThread 종료 요청
	closesocket(_listenSock);

	// AcceptThread 종료
	WaitForMultipleObjects(dfMAX_ACCEPT, &_hAcceptThread, true, INFINITE);

	// WorkerThread 종료 요청
	PostQueuedCompletionStatus(_hNetCP, 0, NULL, NULL);

	// WorkerThread 종료 대기
	WaitForMultipleObjects(dfMAX_WORKER, &_hWorkerThread[0], true, INFINITE);

	// 남은 Session 있는지 확인
	printf("SessionMap Size : %d\n", (int)_SessionMap.size());
	Sleep(4000);

	// WSACleanup
	WSACleanup();

	// closehandle(hNetCP)
	CloseHandle(_hNetCP);

	// closehandle(threads)
	for (int i = 0; i < dfMAX_THREAD; i++)
		CloseHandle(_hWorkerThread[i]);
}

DWORD CNetworkLib::GetNetMaxThread()
{
	return (DWORD)dfMAX_THREAD;
}

HANDLE * CNetworkLib::GetThreadHandle()
{
	return _hWorkerThread;
}

unsigned int __stdcall CNetworkLib::AcceptThread(void * arg)
{
	SESSION		*pSession = NULL;
	SOCKET		clientSock = NULL;
	SOCKADDR_IN clientAddr;
	int			clientLen = 0;
	DWORD		dwRecvByte = 0;
	CNetworkLib *pNet = (CGameServer *)arg;
	 
	printf("Enter Accept Thread\n");

	for (;;) {

		clientLen = sizeof(clientAddr);
		clientSock = accept(pNet->_listenSock, (SOCKADDR *)&clientAddr, &clientLen);
		if (clientSock == INVALID_SOCKET) {
			printf("accept fail\n");
			break;
		}

		//=============================================
		// 정상 로직
		//=============================================

		// 세션 추가
		pSession = new SESSION;
		if (pSession == NULL) {
			// Log Error
			printf("Memory Full\n");
			pNet->Crash();
		}

		// make ID value
		pSession->SessionID = ++(pNet->_SessionID);
		pSession->socket = clientSock;
		pSession->sockAddr = clientAddr;
		pSession->lSendFlag = false;

		//printf("pNet->_dwSessionID : %d\n", pNet->_dwSessionID);

		// 정상적인 클라 세션
		SessionLock(pNet->_SrwSession);
		pNet->_SessionMap.insert(make_pair(pSession->SessionID, pSession));
		SessionUnlock(pNet->_SrwSession);

		// Create IOCP
		CreateIoCompletionPort((HANDLE)clientSock, pNet->_hNetCP, (ULONG_PTR)pSession, 0);

		// WSARecvPost
		pNet->RecvPost(pSession);

		WCHAR wchBuf[22] = { 0, };
		wprintf(L"[Accept Thread] Client IP : %s, Client Port : %d, Client ID : %d\n",
			InetNtopW(AF_INET, &clientAddr.sin_addr, wchBuf, 22), ntohs(clientAddr.sin_port), (int)pSession->SessionID);

		// OnClientJoin Call
		pNet->OnClientJoin(pSession->SessionID, clientAddr);
	}

	printf("Accept Thread Exit\n");

	return 0;
}

unsigned int __stdcall CNetworkLib::WorkerThread(void * arg)
{
	DWORD dwTransfer = 0;
	SESSION *pSession = NULL;
	OVERLAPPED *pOverlap = NULL;
	DWORD dwSendByte = 0;
	DWORD dwRecvByte = 0;
	DWORD dwFlag = 0;
	DWORD dwID = GetThreadId(GetCurrentThread());
	CNetworkLib *pNet = (CGameServer *)arg;
	
	printf("Enter Worker Thread\n");

	for (;;) {

		dwTransfer = 0;
		pSession = NULL;
		pOverlap = NULL;
		GetQueuedCompletionStatus(pNet->_hNetCP, &dwTransfer, (PULONG_PTR)&pSession, &pOverlap, INFINITE);

		//printf("Pass GQCS\n");
		// GQCS 가 false 나는 경우
		// 1) IO 작업 실패시
		// 2) IOCP가 멈춘경우

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
			//printf("[%d] Transfer 0 \n", GetThreadId(GetCurrentThread()));
			shutdown(pSession->socket, SD_BOTH);
		}

		// 정상 로직
		if (pOverlap == &pSession->sendOverlap) {

			int iRet = 0;

			pSession->SendQ.MoveFront(pSession->dwSendByte);
			InterlockedExchange(&pSession->lSendFlag, false);

			iRet = pNet->SendPost(pSession);
			//if (iRet == 1)
			//	continue;
		}

		else if (pOverlap == &pSession->recvOverlap) {

			//// Values
			//int iRet = 0;
			//int iRecvUseSize = 0;
			//int iRetDeqSize = 0;
			//HEADER Header = { 0, };
			//CPacket RecvPack(2000);	// 2000 아니어도 될 여지 있음
			//
			//// 받은 데이터 Byte만큼 RecvQ 업데이트
			//pSession->RecvQ.MoveRear(dwTransfer);
			//
			////pSession->RecvQ.PrintBuffer();
			////printf("Recv \n");
			////int i = 0;
			//
			//// Peek
			//while (1) {

			//	//printf("%d ", ++i);
			//	
			//	// init
			//	iRetDeqSize = 0;
			//	Header = { 0, };
			//	RecvPack.Clear();
			//	iRecvUseSize = pSession->RecvQ.GetUseSize();

			//	// RecvQ 안에 헤더 사이즈 만큼의 데이터도 없으면, break;
			//	if (iRecvUseSize < sizeof(HEADER))
			//		break;

			//	// 헤더 Peek
			//	//printf("sizeof Head : %d\n", sizeof(HEADER));
			//	pSession->RecvQ.Peek((char*)&Header, sizeof(HEADER));

			//	// Header에 있는 Len만큼 페이로드가 RecvQ에 있는지, 없으면 break;
			//	if (iRecvUseSize < sizeof(HEADER) + Header.shLen)
			//		break;

			//	//pSession->RecvQ.PrintBuffer();
			//	//printf("받음[%d]\n",pSession->SessionID);
			//	
			//	// 완전한 패킷이 RecvQ에 있으므로 먼저 헤더 RecvQ에서 빼기
			//	pSession->RecvQ.MoveFront(sizeof(HEADER));

			//	// 페이로드 Dequeue -> Pack

			//	iRetDeqSize = pSession->RecvQ.Dequeue(RecvPack.GetWritePtr(), Header.shLen);

			//	// Pack Size 업데이트
			//	RecvPack.MoveWritePos(iRetDeqSize);
			//	
			//	// OnRecv
			//	pNet->OnRecv(pSession->SessionID, &RecvPack);
			//}
			
			//iRet = pNet->RecvPost(pSession);

			CPacket RecvPack(2000);
			CPacket SendPack(2000);
			RecvPack.Clear();
			SendPack.Clear();
			
			pSession->RecvQ.MoveRear(dwTransfer);
			int iSize = pSession->RecvQ.Dequeue(RecvPack.GetWritePtr(), pSession->RecvQ.GetUseSize());
			
			RecvPack.MoveWritePos(iSize);
			
			pNet->OnRecv(pSession->SessionID, &RecvPack);
			pNet->RecvPost(pSession);
		}

		if (InterlockedDecrement((long *)&pSession->IOCount) == 0) {

			//if (pSession->IOCount == -1)
			//	printf("!!! IOCount -1 !!!\n!!! IOCount -1 !!!\n!!! IOCount -1 !!!\n");

			SESSION_ID SessionID = pSession->SessionID;

			//printf("[%d] IOCount 0 \n", GetThreadId(GetCurrentThread()));
			closesocket(pSession->socket);
			pNet->ReleaseSession(pSession);
			
			// Session 해제 후, ClientLeave
			pNet->OnClientLeave(SessionID);

			continue;
		}
	}

	printf("Exit Worker Thread\n");

	// 스레드 연계 종료
	PostQueuedCompletionStatus(pNet->_hNetCP, 0, NULL, NULL);

	return 0;
}

int CNetworkLib::NetInit()
{
	int iError = 0;

	// WSADATA
	WSADATA wsa;

	//ThreadID
	DWORD dwThreadID = 0;

	// Init Srw
	InitializeSRWLock(&_SrwSession);
	
	// WsaStart
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		//Log SystemError
		printf("!!!ERROR!!! WSAStartup\n");
		Crash();
	}

	// IOCP Create ()
	_hNetCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (_hNetCP == NULL) {
		//Log SystemError
		printf("!!!ERROR!!! CreateIoCompletionPort()\n");
		Crash();
	}

	// socket
	_listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenSock == INVALID_SOCKET) {
		//Log SystemError
		printf("!!!ERROR!!! socket()\n");
		Crash();
	}

	// bind
	ZeroMemory(&_ServerAddr, sizeof(_ServerAddr));
	_ServerAddr.sin_family = AF_INET;
	_ServerAddr.sin_port = htons(dfSERVER_PORT);
	_ServerAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	iError = bind(_listenSock, (SOCKADDR *)&_ServerAddr, sizeof(_ServerAddr));
	if (iError == SOCKET_ERROR) {
		//Log SystemError
		printf("!!!ERROR!!! bind()\n");
		Crash();
	}

	// listen
	iError = listen(_listenSock, SOMAXCONN);
	if (iError == SOCKET_ERROR) {
		//Log SystemError
		printf("!!!ERROR!!! listen()\n");
		Crash();
	}	
	
	// Create Thread
	_hAcceptThread = (HANDLE)_beginthreadex(NULL, 0, AcceptThread, this, 1, (unsigned int *)&dwThreadID);
	for (int i = 0; i < dfMAX_WORKER; i++) {
		_hWorkerThread[i] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, this, 1, (unsigned int *)&dwThreadID);
	}

	// Thread Check
	if (_hAcceptThread == NULL) {

		//Log System Error
		printf("!!!ERROR!!! beginthreadex()\n");
		Crash();
	}
	for (int i = 0; i < dfMAX_WORKER; i++) {
		
		if (_hWorkerThread[i] == NULL) {

			//Log System Error
			printf("!!!ERROR!!! beginthreadex()\n");
			Crash();
		}
	}

	printf("Server Init Success\n");

	return 0;
}

void CNetworkLib::Crash()
{
	int *p = NULL;
	p = 0;
}

int CNetworkLib::RecvPost(SESSION * pSession)
{
	DWORD	dwFlag = 0;
	DWORD	dwRecvByte = 0;
	WSABUF	WSARecvBuf[2];
	int		NumOfBuf = 1;
	int		iError = 0;
	int		iEnqSize = 0;
	int		iFreeSize = 0;
	int		iRemainSize = 0;

	// 정상로직
	iEnqSize = pSession->RecvQ.GetEnqSize();
	iFreeSize = pSession->RecvQ.GetFreeSize();
	iRemainSize = iFreeSize - iEnqSize;
	WSARecvBuf[0].buf = pSession->RecvQ.GetRearBufferPtr();
	WSARecvBuf[0].len = iEnqSize;

	if (iRemainSize > 0) {

		WSARecvBuf[1].buf = pSession->RecvQ.GetBufferPtr();
		WSARecvBuf[1].len = iRemainSize;
		NumOfBuf = 2;
	}

	memset(&pSession->recvOverlap, 0, sizeof(OVERLAPPED));

	InterlockedIncrement((long *)&pSession->IOCount);
	//printf("[%d] Recv ReqSize : %d\n", pSession, iFreeSize);
	int iRet = WSARecv(pSession->socket, WSARecvBuf, NumOfBuf, &dwRecvByte, &dwFlag, &pSession->recvOverlap, NULL);
	if (iRet == SOCKET_ERROR) {

		DWORD dwError = WSAGetLastError();
		if (dwError != WSA_IO_PENDING) {

			if (InterlockedDecrement((long *)&pSession->IOCount) == 0) {

				SESSION_ID SessionID = pSession->SessionID;

				//printf("[%d] Recv IOCount 0 \n", GetThreadId(GetCurrentThread()));
				closesocket(pSession->socket);
				ReleaseSession(pSession);

				// Session 해제 후, ClientLeave
				OnClientLeave(SessionID);

				// iRet = 성공시 0, 실패시 SOCKET_ERROR(-1), 유저 return 으로 1 사용
				//iRet = 1;
			}
			else {
				//printf("[%d] Recv SockError \n", GetThreadId(GetCurrentThread()));
				shutdown(pSession->socket, SD_BOTH);
			}
		}
	}

	return iRet;
}

int CNetworkLib::SendPost(SESSION * pSession)
{
	DWORD	dwSendByte = 0;
	WSABUF	WSASendBuf[2];
	int		NumOfBuf = 1;
	int		iError = 0;
	int		iDeqSize = 0;
	int		iUseSize = 0;
	int		iRemainSize = 0;

	// F->T
	if (InterlockedExchange((long *)&pSession->lSendFlag, true)) {

		return 1;
	}

	iUseSize = pSession->SendQ.GetUseSize();
	if (iUseSize <= 0) {

		InterlockedExchange((long *)&pSession->lSendFlag, false);
		//printf("UseSize 0\n");
		return 1;
	}

	// 정상 로직

	iDeqSize = pSession->SendQ.GetDeqSize();
	iRemainSize = iUseSize - iDeqSize;
	WSASendBuf[0].buf = pSession->SendQ.GetFrontBufferPtr();
	WSASendBuf[0].len = iDeqSize;

	if (iRemainSize > 0) {

		WSASendBuf[1].buf = pSession->SendQ.GetBufferPtr();
		WSASendBuf[1].len = iRemainSize;
		NumOfBuf = 2;
	}

	pSession->dwSendByte = iUseSize;

	memset(&pSession->sendOverlap, 0, sizeof(OVERLAPPED));

	// Count 올리기
	InterlockedIncrement((long *)&pSession->IOCount);
	//printf("[%d] Send ReqSize : pSession->dwSendByte - %d\n", pSession, pSession->dwSendByte);
	int iRet = WSASend(pSession->socket, WSASendBuf, NumOfBuf, &dwSendByte, 0, &pSession->sendOverlap, NULL);
	if (iRet == SOCKET_ERROR) {

		DWORD dwError = WSAGetLastError();
		if (dwError != WSA_IO_PENDING) {

			if (InterlockedDecrement((long *)&pSession->IOCount) == 0) {

				SESSION_ID SessionID = pSession->SessionID;

				//printf("[%d] Send IOCount 0 \n", GetThreadId(GetCurrentThread()));
				closesocket(pSession->socket);
				ReleaseSession(pSession);

				// Session 해제 후, ClientLeave
				OnClientLeave(SessionID);

				// iRet = 성공시 0, 실패시 SOCKET_ERROR(-1), 유저 return 으로 1 사용
				//iRet = 1;
			}

			else {

				//printf("[%d] Send SockError \n", GetThreadId(GetCurrentThread()));
				shutdown(pSession->socket, SD_BOTH);
			}
		}
	}

	return iRet;
}

bool CNetworkLib::ReleaseSession(SESSION * pSession)
{
	bool bRet = false;
	WCHAR wchBuf[22] = { 0, };
	if (pSession == NULL)
		return bRet;

	SessionLock(_SrwSession);

	map<SESSION_ID, SESSION *>::iterator SessionIDIter = _SessionMap.begin();
	map<SESSION_ID, SESSION *>::iterator SessionIDIterEnd = _SessionMap.end();

	for (; SessionIDIter != SessionIDIterEnd; ++SessionIDIter) {

		if (pSession == (*SessionIDIter).second) {

			_SessionMap.erase(SessionIDIter);

			wprintf(L"[Release Thread] Client IP : %s, Client Port : %d\n",
				InetNtopW(AF_INET, &pSession->sockAddr.sin_addr, wchBuf, 22), ntohs(pSession->sockAddr.sin_port));

			delete pSession;

			bRet = true;

			break;
		}
	}

	SessionUnlock(_SrwSession);

	return bRet;
}

SESSION * CNetworkLib::FindSession(SESSION_ID SessionID)
{
	SESSION *pSession = NULL;

	SessionLock(_SrwSession);

	map<SESSION_ID, SESSION *>::iterator SessionIDIter	 = _SessionMap.begin();
	map<SESSION_ID, SESSION *>::iterator SessionIDIterEnd = _SessionMap.end();

	for (; SessionIDIter != SessionIDIterEnd; ++SessionIDIter) {

		if (SessionID == (*SessionIDIter).first) {

			pSession = (*SessionIDIter).second;
			break;
		}
	}

	SessionUnlock(_SrwSession);

	return pSession;
}

int CNetworkLib::SendPacket(SESSION_ID SessionID, CPacket * pPack)
{
	SESSION *pSession = FindSession(SessionID);
	if (pSession == NULL) {

		printf("pLocalSession NULL\n");
		return 0;
	}
	int iRet = 0;

	// 헤더 init + 붙이기
	HEADER Header = { 0, };
	Header.shLen = pPack->GetPacketSize();

	// 헤더 + Packet -> SendQ에 Enq
	pSession->SendQ.Enqueue((char *)&Header, sizeof(HEADER));
	pSession->SendQ.Enqueue(pPack->GetReadPtr(), pPack->GetPacketSize());
	
	//pSession->SendQ.PrintBuffer();
	//printf("보내기전[%d] \n",pSession->SessionID);

	//printf("SendPacket Try\n");
	iRet = SendPost(pSession);
	return iRet;
}

int CNetworkLib::Disconnect(SESSION_ID SessionID)
{
	SESSION *pSession = NULL;
	pSession = FindSession(SessionID);
	if (pSession == NULL) {
		return false;
	}
	shutdown(pSession->socket, SD_BOTH);
	return 0;
}

//CPacket RecvPack(2000);
//CPacket SendPack(2000);
//RecvPack.Clear();
//SendPack.Clear();
//
//pSession->RecvQ.MoveRear(dwTransfer);
//int iSize = pSession->RecvQ.Dequeue(RecvPack.GetWritePtr(), pSession->RecvQ.GetUseSize());
//
//RecvPack.MoveWritePos(iSize);
//
//pNet->OnRecv(pSession->dwID, &RecvPack);
//iRet = pNet->RecvPost(pSession);
//if (iRet == 1)
//continue;


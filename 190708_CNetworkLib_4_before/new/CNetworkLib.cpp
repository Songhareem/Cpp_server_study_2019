
#include "CNetworkLib.h"
#include "CGameServer.h"

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
	WaitForMultipleObjects(dfMAX_ACCEPT, &_hNetThread[0], true, INFINITE);

	// WorkerThread 종료 요청
	PostQueuedCompletionStatus(_hNetCP, 0, NULL, NULL);

	// WorkerThread 종료 대기
	WaitForMultipleObjects(dfMAX_WORKER, &_hNetThread[1], true, INFINITE);

	// 남은 Session 있는지 확인
	printf("SessionMap Size : %d\n", (int)_SessionMap.size());
	Sleep(4000);

	// WSACleanup
	WSACleanup();

	// closehandle(hNetCP)
	CloseHandle(_hNetCP);

	// closehandle(threads)
	for (int i = 0; i < dfMAX_THREAD; i++)
		CloseHandle(_hNetThread[i]);
}

DWORD CNetworkLib::GetNetMaxThread()
{
	return (DWORD)dfMAX_THREAD;
}

HANDLE * CNetworkLib::GetThreadHandle()
{
	return _hNetThread;
}

unsigned int __stdcall CNetworkLib::AcceptThread(void * arg)
{
	SESSION		*pSession = NULL;
	SOCKET		clientSock = NULL;
	SOCKADDR_IN clientAddr;
	int			clientLen = 0;
	DWORD		dwRecvByte = 0;
	DWORD		dwFlag = 0;
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
			continue;
		}
		// make ID value
		pSession->dwID = pNet->_dwSessionID++;
		pSession->socket = clientSock;
		pSession->sockAddr = clientAddr;
		pSession->lSendFlag = false;

		//printf("pNet->_dwSessionID : %d\n", pNet->_dwSessionID);

		// Create IOCP
		CreateIoCompletionPort((HANDLE)clientSock, pNet->_hNetCP, (ULONG_PTR)pSession, 0);

		// 정상적인 클라 세션
		SessionLock(pNet->_SrwSession);
		pNet->_SessionMap.insert(make_pair(pSession->dwID, pSession));
		SessionUnlock(pNet->_SrwSession);

		// OnClientJoin Call
		pNet->OnClientJoin(pSession->dwID, clientAddr);

		// WSARecvPost
		pNet->RecvPost(pSession);

		WCHAR wchBuf[22] = { 0, };
		wprintf(L"[Accept Thread] Client IP : %s, Client Port : %d, Client ID : %d\n",
			InetNtopW(AF_INET, &clientAddr.sin_addr, wchBuf, 22), ntohs(clientAddr.sin_port), pSession->dwID);

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
		int iError = GetQueuedCompletionStatus(pNet->_hNetCP, &dwTransfer, (PULONG_PTR)&pSession, &pOverlap, INFINITE);

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
			InterlockedExchange((long *)&pSession->lSendFlag, false);

			iRet = pNet->SendPost(pSession);
			if (iRet == 1)
				continue;
		}

		else if (pOverlap == &pSession->recvOverlap) {

			int iRet = 0;

			CPacket RecvPack(2000);
			CPacket SendPack(2000);
			RecvPack.Clear();
			SendPack.Clear();

			pSession->RecvQ.MoveRear(dwTransfer);
			int iSize = pSession->RecvQ.Dequeue(RecvPack.GetWritePtr(), pSession->RecvQ.GetUseSize());

			RecvPack.MoveWritePos(iSize);

			pNet->OnRecv(pSession->dwID, &RecvPack);
			iRet = pNet->RecvPost(pSession);
			if (iRet == 1)
				continue;

			// 마샬링 미구현
		}

		if (!InterlockedDecrement((long *)&pSession->IOCount)) {

			//if (pSession->IOCount == -1)
			//	printf("!!! IOCount -1 !!!\n!!! IOCount -1 !!!\n!!! IOCount -1 !!!\n");

			DWORD dwSessionID = pSession->dwID;

			//printf("[%d] IOCount 0 \n", GetThreadId(GetCurrentThread()));
			closesocket(pSession->socket);
			pNet->ReleaseSession(pSession);
			
			// Session 해제 후, ClientLeave
			pNet->OnClientLeave(dwSessionID);

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
		return 1;
	}

	// socket
	_listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenSock == INVALID_SOCKET) {
		//Log SystemError
		printf("!!!ERROR!!! socket()\n");
		return 1;
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
		return 1;
	}

	// listen
	iError = listen(_listenSock, SOMAXCONN);
	if (iError == SOCKET_ERROR) {
		//Log SystemError
		printf("!!!ERROR!!! listen()\n");
		return 1;
	}	
	
	// IOCP Create ()
	_hNetCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (_hNetCP == NULL) {
		//Log SystemError
		printf("!!!ERROR!!! CreateIoCompletionPort()\n");
		return 1;
	}
	
	// Create Thread
	_hNetThread[0] = (HANDLE)_beginthreadex(NULL, 0, AcceptThread, this, 0, (unsigned int *)&dwThreadID);
	for (int i = 0; i < dfMAX_WORKER; i++) {
		_hNetThread[i + 1] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, this, 0, (unsigned int *)&dwThreadID);
	}

	// Thread Check
	for (int i = 0; i < dfMAX_THREAD; i++) {
		
		if (_hNetThread[i] == NULL) {

			//Log System Error
			printf("!!!ERROR!!! beginthreadex()\n");
			
			return 1;
		}
	}

	printf("Server Init Success\n");

	return 0;
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

	if (iRemainSize >= 0) {

		WSARecvBuf[1].buf = pSession->RecvQ.GetBufferPtr();
		WSARecvBuf[1].len = iRemainSize;
		NumOfBuf = 2;
	}

	memset(&pSession->recvOverlap, 0, sizeof(OVERLAPPED));

	InterlockedIncrement((long *)&pSession->IOCount);
	//printf("[%d] Recv ReqSize : %d\n", pSession, iFreeSize);
	int iRet = WSARecv(pSession->socket, WSARecvBuf, NumOfBuf, &dwRecvByte, &dwFlag, &pSession->recvOverlap, NULL);
	if (iRet == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING) {

		if (!InterlockedDecrement((long *)&pSession->IOCount)) {

			DWORD dwSessionID = pSession->dwID;

			//printf("[%d] Recv IOCount 0 \n", GetThreadId(GetCurrentThread()));
			closesocket(pSession->socket);
			ReleaseSession(pSession);

			// Session 해제 후, ClientLeave
			OnClientLeave(dwSessionID);

			// iRet = 성공시 0, 실패시 SOCKET_ERROR(-1), 유저 return 으로 1 사용
			iRet = 1;
		}
		else {
			//printf("[%d] Recv SockError \n", GetThreadId(GetCurrentThread()));
			shutdown(pSession->socket, SD_BOTH);
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
	if ((bool)InterlockedExchange((long *)&pSession->lSendFlag, true) == true) {

		return -2;
	}

	iUseSize = pSession->SendQ.GetUseSize();
	if (iUseSize <= 0) {

		InterlockedExchange((long *)&pSession->lSendFlag, false);
		//printf("UseSize 0\n");
		return -3;
	}

	// 정상 로직

	iDeqSize = pSession->SendQ.GetDeqSize();
	iRemainSize = iUseSize - iDeqSize;
	WSASendBuf[0].buf = pSession->SendQ.GetFrontBufferPtr();
	WSASendBuf[0].len = iDeqSize;

	if (iRemainSize >= 0) {

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
	if (iRet == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING) {

		if (!InterlockedDecrement((long *)&pSession->IOCount)) {

			DWORD dwSessionID = pSession->dwID;

			//printf("[%d] Send IOCount 0 \n", GetThreadId(GetCurrentThread()));
			closesocket(pSession->socket);
			ReleaseSession(pSession);

			// Session 해제 후, ClientLeave
			OnClientLeave(dwSessionID);

			// iRet = 성공시 0, 실패시 SOCKET_ERROR(-1), 유저 return 으로 1 사용
			iRet = 1;
		}

		else {

			//printf("[%d] Send SockError \n", GetThreadId(GetCurrentThread()));
			shutdown(pSession->socket, SD_BOTH);
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

	AcquireSRWLockExclusive(&_SrwSession);

	map<DWORD, SESSION *>::iterator SessionIDIter = _SessionMap.begin();
	map<DWORD, SESSION *>::iterator SessionIDIterEnd = _SessionMap.end();

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

	ReleaseSRWLockExclusive(&_SrwSession);

	return bRet;
}

SESSION * CNetworkLib::FindSession(DWORD dwSessionID)
{
	SessionLock(_SrwSession);

	SESSION *pSession = NULL;

	map<DWORD, SESSION *>::iterator SessionIDIter	 = _SessionMap.begin();
	map<DWORD, SESSION *>::iterator SessionIDIterEnd = _SessionMap.end();

	for (; SessionIDIter != SessionIDIterEnd; ++SessionIDIter) {

		if (dwSessionID == (*SessionIDIter).first) {

			pSession = (*SessionIDIter).second;
			break;
		}
	}

	SessionUnlock(_SrwSession);

	return pSession;
}

int CNetworkLib::SendPacket(DWORD dwSessionID, CPacket * pPack)
{
	SESSION *pSession = FindSession(dwSessionID);
	if (pSession == NULL) {

		printf("pLocalSession NULL\n");
	}
	int iRet = 0;

	pSession->SendQ.Enqueue(pPack->GetBufferPtr(), pPack->GetPacketSize());

	//printf("SendPacket Try\n");
	iRet = SendPost(pSession);
	//if (iRet == -2)
	//	printf("SendPacket False\n");

	//printf("SendPacket Success\n");
	return iRet;
}

int CNetworkLib::Disconnect(DWORD dwSessionID)
{
	SESSION *pSession = NULL;
	pSession = FindSession(dwSessionID);
	shutdown(pSession->socket, SD_BOTH);
	return 0;
}

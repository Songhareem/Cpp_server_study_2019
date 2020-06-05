
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
	DWORD		dwFlag = 0;
	CNetworkLib *pNet = (CNetworkLib *)arg;
	 
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
		pSession = pNet->CreateSession(clientSock, clientAddr);
		if (pSession == NULL) {
			// Log System Error
			printf("Memory Full\n");
			pNet->Crash();
		}
		
		// 세션 추가 후, OnClientJoin Call
		pNet->OnClientJoin(pSession->SessionID, clientAddr);

		// Create IOCP
		CreateIoCompletionPort((HANDLE)clientSock, pNet->_hNetCP, (ULONG_PTR)pSession, 0);

		// WSARecvPost
		pNet->RecvPost(pSession);

		/*WCHAR wchBuf[22] = { 0, };
		wprintf(L"[Accept Thread] Client IP : %s, Client Port : %d, Client ID : %d\n",
			InetNtopW(AF_INET, &clientAddr.sin_addr, wchBuf, 22), ntohs(clientAddr.sin_port), (int)pSession->SessionID);
		*/
		
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
	CNetworkLib *pNet = (CNetworkLib *)arg;
	
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

			//printf("dwTransfer :%d, dwSendByte : %d\n", dwTransfer, pSession->dwSendByte);
			pSession->SendQ.MoveFront(pSession->dwSendByte);
			//printf("AS %d\n", pSession->SendQ.GetUseSize());
			InterlockedExchange((long *)&pSession->lSendFlag, false);

			if(pSession->SendQ.GetUseSize() > 0)
				pNet->SendPost(pSession);
		}

		else if (pOverlap == &pSession->recvOverlap) {

			// Values
			int iRecvUseSize = 0;
			int iRetDeqSize = 0;
			HEADER Header = 0;
			CPacket Pack(2000);	// 2000 아니어도 될 여지 있음

			// 받은 데이터 Byte만큼 RecvQ 업데이트
			pSession->RecvQ.MoveRear(dwTransfer);
			//printf("AR %d\n", pSession->RecvQ.GetUseSize());

			//* Mass Throw test
			/*int size = pSession->RecvQ.Dequeue(Pack.GetWritePtr(), pSession->RecvQ.GetUseSize());
			Pack.MoveWritePos(size);

			pNet->SendPacket(pSession->SessionID, &Pack);*/
			//*/

			for (;;) {

				// init
				Pack.Clear();
				iRecvUseSize = pSession->RecvQ.GetUseSize();

				// RecvQ 안에 헤더 사이즈 만큼의 데이터도 없으면, break;
				if (iRecvUseSize < sizeof(HEADER))
					break;

				// 헤더 Peek
				pSession->RecvQ.Peek((char*)&Header, sizeof(HEADER));

				// Header에 있는 Len만큼 페이로드가 RecvQ에 있는지, 없으면 break;
				if (iRecvUseSize < sizeof(HEADER) + Header)
					break;

				// 완전한 패킷이 RecvQ에 있으므로 먼저 헤더 RecvQ에서 빼기
				pSession->RecvQ.MoveFront(sizeof(HEADER));

				// 페이로드 Dequeue -> Pack
				iRetDeqSize = pSession->RecvQ.Dequeue(Pack.GetWritePtr(), Header);
				/*if (iRetDeqSize != Header) {
					printf("DiffData Header : %d, iRetDequeSize : %d\n", Header, iRetDeqSize);
					break;
				}*/

				// Pack Size 업데이트
				Pack.MoveWritePos(iRetDeqSize);

				// OnRecv
				pNet->OnRecv(pSession->SessionID, &Pack);
			}
			pNet->RecvPost(pSession);
		}

		if (InterlockedDecrement((long *)&pSession->IOCount) == 0) {

			pNet->OnClientLeave(pSession->SessionID);

			pNet->ReleaseSession(pSession);
			
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
		//exit(1);
	}

	// IOCP Create ()
	_hNetCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (_hNetCP == NULL) {
		//Log SystemError
		printf("!!!ERROR!!! CreateIoCompletionPort()\n");
		Crash();
		//exit(1);
	}

	// socket
	_listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenSock == INVALID_SOCKET) {
		//Log SystemError
		printf("!!!ERROR!!! socket()\n");
		Crash();
		//exit(1);
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
		//exit(1);
	}

	// listen
	iError = listen(_listenSock, SOMAXCONN);
	if (iError == SOCKET_ERROR) {
		//Log SystemError
		printf("!!!ERROR!!! listen()\n");
		Crash();
		//exit(1);
	}	
	
	// Create Thread
	_hAcceptThread = (HANDLE)_beginthreadex(NULL, 0, AcceptThread, this, 0, (unsigned int *)&dwThreadID);
	for (int i = 0; i < dfMAX_WORKER; i++) {
		_hWorkerThread[i] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, this, 0, (unsigned int *)&dwThreadID);
	}

	// Thread Check
	if (_hAcceptThread == NULL) {

		//Log System Error
		printf("!!!ERROR!!! beginthreadex()\n");
		Crash();
		//exit(1);
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
	memset(&pSession->recvOverlap, 0, sizeof(OVERLAPPED));
	pSession->RecvQ.GetFreeEnqSize(&iFreeSize, &iEnqSize);
	iRemainSize = iFreeSize - iEnqSize;
	WSARecvBuf[0].buf = pSession->RecvQ.GetRearBufferPtr();
	WSARecvBuf[0].len = iEnqSize;

	if (iFreeSize > iEnqSize) {
		if (iRemainSize < 0) {
			int a = 0;
		}
		WSARecvBuf[1].buf = pSession->RecvQ.GetBufferPtr();
		WSARecvBuf[1].len = iRemainSize;
		NumOfBuf = 2;
	}

	InterlockedIncrement((long *)&pSession->IOCount);
	//printf("[%d] Recv ReqSize : %d\n", pSession, iFreeSize);
	//printf("BR %d\n",pSession->RecvQ.GetUseSize());
	int iRet = WSARecv(pSession->socket, WSARecvBuf, NumOfBuf, &dwRecvByte, &dwFlag, &pSession->recvOverlap, NULL);
	if (iRet == SOCKET_ERROR) {

		DWORD dwError = WSAGetLastError();
		if (dwError != ERROR_IO_PENDING) {

			shutdown(pSession->socket, SD_BOTH);

			if (InterlockedDecrement((long *)&pSession->IOCount) == 0) {

				OnClientLeave(pSession->SessionID);

				ReleaseSession(pSession);

			}
			//else {
			//	//printf("[%d] Recv SockError \n", GetThreadId(GetCurrentThread()));
			//	shutdown(pSession->socket, SD_BOTH);
			//}
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
	int		iUseSizeFront = 0;
	int		iRemainSize = 0;

	// F->T
	/*if (InterlockedCompareExchange(&pSession->lSendFlag, true, false) == 1) {

		return 1;
	}*/

	if (InterlockedExchange(&pSession->lSendFlag, true) == 1) {

		return 1;
	}

	// 정상 로직

	// Front Rear
	//printf("[Bef] Front : %d, Rear : %d\n", pSession->SendQ.GetFrontPos(), pSession->SendQ.GetRearPos());
	//printf("[Aft] Front : %d, Rear : %d\n[etc] UseSize : %d, DeqSize : %d\n", 
	//	pSession->SendQ.GetFrontPos(), pSession->SendQ.GetRearPos(), iUseSize, iDeqSize); 

	pSession->SendQ.GetUseDeqSize(&iUseSize, &iDeqSize);
	if (iUseSize <= 0) {
		//printf("return size 0\n");
		InterlockedExchange((long *)&pSession->lSendFlag, false);
		return 1;
	}
	iRemainSize = iUseSize - iDeqSize;
	WSASendBuf[0].buf = pSession->SendQ.GetFrontBufferPtr();
	WSASendBuf[0].len = iDeqSize;

	if (iUseSize > iDeqSize) {

		WSASendBuf[1].buf = pSession->SendQ.GetBufferPtr();
		WSASendBuf[1].len = iRemainSize;
		NumOfBuf = 2;
	}
	
	memset(&pSession->sendOverlap, 0, sizeof(OVERLAPPED));

	pSession->dwSendByte = iUseSize;

	// Count 올리기
	InterlockedIncrement((long *)&pSession->IOCount);
	//printf("[%d] Send ReqSize : pSession->dwSendByte - %d\n", pSession, pSession->dwSendByte);
	//printf("BS %d\n", pSession->SendQ.GetUseSize());
	int iRet = WSASend(pSession->socket, WSASendBuf, NumOfBuf, &dwSendByte, 0, &pSession->sendOverlap, NULL);
	if (iRet == SOCKET_ERROR) {

		DWORD dwError = WSAGetLastError();
		if (dwError != ERROR_IO_PENDING) {

			shutdown(pSession->socket, SD_BOTH);

			if (InterlockedDecrement((long *)&pSession->IOCount) == 0) {

				OnClientLeave(pSession->SessionID);

				ReleaseSession(pSession);

			}

			//else {

			//	//printf("[%d] Send SockError \n", GetThreadId(GetCurrentThread()));
			//	shutdown(pSession->socket, SD_BOTH);
			//}
		}
	}

	return iRet;
}

SESSION * CNetworkLib::CreateSession(SOCKET clientSock, SOCKADDR_IN clientAddr)
{
	SESSION *pSession = NULL;
	
	SessionLock(_SrwSession);

	pSession = new SESSION;
	if (pSession == NULL)
		return pSession;

	// make ID value
	pSession->SessionID = ++_SessionID;
	pSession->socket = clientSock;
	pSession->sockAddr = clientAddr;

	//SessionLock(_SrwSession);

	_SessionMap.insert(make_pair(pSession->SessionID, pSession));

	SessionUnlock(_SrwSession);

	return pSession;
}

bool CNetworkLib::ReleaseSession(SESSION_ID SessionID)
{
	WCHAR wchBuf[22] = { 0, };
	SOCKET tmpSock = NULL;
	SESSION *pSession = FindSession(SessionID);
	if (pSession == NULL) {
		printf("Session NULL\n");
		return false;
	}

	/*wprintf(L"[Release Thread] Client IP : %s, Client Port : %d\n",
		InetNtop(AF_INET, &pSession->sockAddr.sin_addr, wchBuf, 22), ntohs(pSession->sockAddr.sin_port));*/

	SessionLock(_SrwSession);
	tmpSock = pSession->socket;
	delete pSession; // 이 위치?
	_SessionMap.erase(SessionID);
	//delete pSession;
	closesocket(tmpSock);
	SessionUnlock(_SrwSession);

	return true;
}

bool CNetworkLib::ReleaseSession(SESSION * pSession)
{
	if (pSession == NULL) {

		printf("Session NULL\n");
		return false;
	}

	SessionLock(_SrwSession);

	SOCKET tmpSock = pSession->socket;

	map<SESSION_ID, SESSION *>::iterator SessionIter = _SessionMap.begin();
	map<SESSION_ID, SESSION *>::iterator SessionIterEnd = _SessionMap.end();

	for (; SessionIter != SessionIterEnd; ++SessionIter) {

		if (pSession == (*SessionIter).second) {

			_SessionMap.erase(SessionIter);
			delete pSession;
			break;
		}
	}

	closesocket(tmpSock);

	SessionUnlock(_SrwSession);

	return true;
}

SESSION * CNetworkLib::FindSession(SESSION_ID SessionID)
{
	SessionLock(_SrwSession);

	SESSION *pSession = NULL;

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
	
	pSession->SendQ.Enqueue(pPack->GetReadPtr(), pPack->GetPacketSize());
	
	SendPost(pSession);

	return true;
}

int CNetworkLib::Disconnect(SESSION_ID SessionID)
{
	SESSION *pSession = NULL;
	pSession = FindSession(SessionID);
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


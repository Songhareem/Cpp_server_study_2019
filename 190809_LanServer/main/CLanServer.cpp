
#include "CLanServer.h"
#include "CGameServer.h"
#include "CLockFreeStack.h"
#include <process.h>

CLanServer::CLanServer()
{
	NetInit();
}

CLanServer::~CLanServer()
{
	DWORD dwRet = 0;

	// closesocket(listenSock) -> AcceptThread ���� ��û
	closesocket(_listenSock);

	// AcceptThread ����
	WaitForMultipleObjects(dfMAX_ACCEPT, &_hAcceptThread, true, INFINITE);

	// WorkerThread ���� ��û
	PostQueuedCompletionStatus(_hNetCP, 0, NULL, NULL);

	// WorkerThread ���� ���
	WaitForMultipleObjects(dfMAX_WORKER, &_hWorkerThread[0], true, INFINITE);

	Sleep(4000);

	// WSACleanup
	WSACleanup();

	// closehandle(hNetCP)
	CloseHandle(_hNetCP);

	// closehandle(threads)
	for (int i = 0; i < dfMAX_THREAD; i++)
		CloseHandle(_hWorkerThread[i]);
}

DWORD CLanServer::GetNetMaxThread()
{
	return (DWORD)dfMAX_THREAD;
}

HANDLE * CLanServer::GetThreadHandle()
{
	return _hWorkerThread;
}

unsigned int __stdcall CLanServer::AcceptThread(void * arg)
{
	SESSION		*pSession = NULL;
	SOCKET		clientSock = NULL;
	SOCKADDR_IN clientAddr;
	int			clientLen = 0;
	DWORD		dwRecvByte = 0;
	DWORD		dwFlag = 0;
	CLanServer *pNet = (CLanServer *)arg;
	 
	printf("Enter Accept Thread\n");

	for (;;) {

		clientLen = sizeof(clientAddr);
		clientSock = accept(pNet->_listenSock, (SOCKADDR *)&clientAddr, &clientLen);
		if (clientSock == INVALID_SOCKET) {
			printf("accept fail\n");
			break;
		}

		//=============================================
		// ���� ����
		//=============================================

		// ���� �߰�
		pSession = pNet->CreateSession(clientSock, clientAddr);
		if (pSession == NULL) {
			// Log System Error
			printf("Memory Full\n");
			pNet->Crash();
		}
		
		// ���� �߰� ��, OnClientJoin Call
		pNet->OnClientJoin(pSession->SessionID, clientAddr);

		// Create IOCP
		CreateIoCompletionPort((HANDLE)clientSock, pNet->_hNetCP, (ULONG_PTR)pSession, 0);

		// WSARecvPost
		pNet->RecvPost(pSession);
	}

	printf("Accept Thread Exit\n");

	return 0;
}

unsigned int __stdcall CLanServer::WorkerThread(void * arg)
{
	DWORD dwTransfer = 0;
	SESSION *pSession = NULL;
	OVERLAPPED *pOverlap = NULL;
	DWORD dwSendByte = 0;
	DWORD dwRecvByte = 0;
	DWORD dwFlag = 0;
	DWORD dwID = GetThreadId(GetCurrentThread());
	CLanServer *pNet = (CLanServer *)arg;
	
	printf("Enter Worker Thread\n");

	for (;;) {

		dwTransfer = 0;
		pSession = NULL;
		pOverlap = NULL;
		GetQueuedCompletionStatus(pNet->_hNetCP, &dwTransfer, (PULONG_PTR)&pSession, &pOverlap, INFINITE);

		//printf("Pass GQCS\n");
		// GQCS �� false ���� ���
		// 1) IO �۾� ���н�
		// 2) IOCP�� ������

		// PostQueuedCompletionStatus�� ���� ������ ����
		if (dwTransfer == 0 && pSession == NULL && pOverlap == NULL)
			break;

		if (pOverlap == NULL)
			continue;

		if (pSession == NULL)
			continue;

		// dwTransfer == 0 && pOverlap != NULL
		if (dwTransfer == 0) {

			// ��������ó��
			shutdown(pSession->socket, SD_BOTH);
		}

		// ���� ����
		if (pOverlap == &pSession->sendOverlap) {

			CPacket *pPack = NULL;
			// PacketFree
			for (int i = 0; i < pSession->dwSendPackNum; i++) {

				pPack = pSession->pPackArr[i];
				CPacket::Free(pPack);
			}

			InterlockedExchange((long *)&pSession->lSendFlag, false);

			if(pSession->SendQ.GetQueSize() > 0)
				pNet->SendPost(pSession);
		}

		else if (pOverlap == &pSession->recvOverlap) {

			// Values
			int iRecvUseSize = 0;
			int iRetDeqSize = 0;
			HEADER Header = 0;
			CPacket *pPack = CPacket::Alloc();


			// ���� ������ Byte��ŭ RecvQ ������Ʈ
			pSession->RecvQ.MoveRear(dwTransfer);

			for (;;) {

				// init
				pPack->Clear();
				iRecvUseSize = pSession->RecvQ.GetUseSize();

				// RecvQ �ȿ� ��� ������ ��ŭ�� �����͵� ������, break;
				if (iRecvUseSize < sizeof(HEADER))
					break;

				// ��� Peek
				pSession->RecvQ.Peek((char*)&Header, sizeof(HEADER));

				// Header�� �ִ� Len��ŭ ���̷ε尡 RecvQ�� �ִ���, ������ break;
				if (iRecvUseSize < sizeof(HEADER) + Header)
					break;

				// ������ ��Ŷ�� RecvQ�� �����Ƿ� ���� ��� RecvQ���� ����
				pSession->RecvQ.MoveFront(sizeof(HEADER));

				// ���̷ε� Dequeue -> Pack
				iRetDeqSize = pSession->RecvQ.Dequeue(pPack->GetWritePtr(), Header);

				// Pack Size ������Ʈ
				pPack->MoveWritePos(iRetDeqSize);

				// OnRecv
				pNet->OnRecv(pSession->SessionID, pPack);
			}
			
			CPacket::Free(pPack);
			pNet->RecvPost(pSession);
		}

		if (InterlockedDecrement(&pSession->lRefCount) == 0) {

			pNet->OnClientLeave(pSession->SessionID);

			pNet->ReleaseSession(pSession);
			
			continue;
		}
	}

	printf("Exit Worker Thread\n");

	// ������ ���� ����
	PostQueuedCompletionStatus(pNet->_hNetCP, 0, NULL, NULL);

	return 0;
}

int CLanServer::NetInit()
{
	int iError = 0;

	// SessionIndex����
	for(int i = (dfMAX_SESSION - 1); i >= 0; i--) {
		_IndexStack.Push(i);
	}

	// WSADATA
	WSADATA wsa;

	//ThreadID
	DWORD dwThreadID = 0;
	
	// Init Srw
	//InitializeSRWLock(&_SrwSession);
	
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

void CLanServer::Crash()
{
	int *p = NULL;
	p = 0;
}

int CLanServer::RecvPost(SESSION * pSession)
{
	DWORD	dwFlag = 0;
	DWORD	dwRecvByte = 0;
	WSABUF	WSARecvBuf[2];
	int		NumOfBuf = 1;
	int		iError = 0;
	int		iEnqSize = 0;
	int		iFreeSize = 0;
	int		iRemainSize = 0;

	// �������
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

	InterlockedIncrement(&pSession->lRefCount);
	int iRet = WSARecv(pSession->socket, WSARecvBuf, NumOfBuf, &dwRecvByte, &dwFlag, &pSession->recvOverlap, NULL);
	if (iRet == SOCKET_ERROR) {

		DWORD dwError = WSAGetLastError();
		if (dwError != ERROR_IO_PENDING) {

			shutdown(pSession->socket, SD_BOTH);

			if (InterlockedDecrement(&pSession->lRefCount) == 0) {

				OnClientLeave(pSession->SessionID);

				ReleaseSession(pSession);

			}
		}
	}

	return iRet;
}

int CLanServer::SendPost(SESSION * pSession)
{
	DWORD	dwSendByte = 0;
	WSABUF	WSASendBuf[dfMAX_PACK_ARR];
	int		iError = 0;
	int		iPackNum = 0;
	int		iUseSizeFront = 0;
	int		iRemainSize = 0;

	if (InterlockedExchange(&pSession->lSendFlag, true) == 1) {

		return 1;
	}

	iPackNum = pSession->SendQ.GetQueSize();
	if (iPackNum <= 0) {
		InterlockedExchange((long *)&pSession->lSendFlag, false);
		return 1;
	}

	CPacket *pPack = NULL;
	for (int i = 0; i < iPackNum; i++) {
		
		pSession->SendQ.Deque(&pPack);
		pSession->pPackArr[i] = pPack;
		WSASendBuf[i].buf = pPack->GetHeadPtr();
		WSASendBuf[i].len = pPack->GetPacketSize();
	}

	memset(&pSession->sendOverlap, 0, sizeof(OVERLAPPED));

	pSession->dwSendPackNum = iPackNum;

	// Count �ø���
	InterlockedIncrement(&pSession->lRefCount);
	int iRet = WSASend(pSession->socket, WSASendBuf, iPackNum, &dwSendByte, 0, &pSession->sendOverlap, NULL);
	if (iRet == SOCKET_ERROR) {

		DWORD dwError = WSAGetLastError();
		if (dwError != ERROR_IO_PENDING) {

			shutdown(pSession->socket, SD_BOTH);

			if (InterlockedDecrement(&pSession->lRefCount) == 0) {

				OnClientLeave(pSession->SessionID);

				ReleaseSession(pSession);

			}
		}
	}

	return iRet;
}

SESSION * CLanServer::CreateSession(SOCKET clientSock, SOCKADDR_IN clientAddr)
{
	SESSION *pSession = NULL;
	DWORD	dwSessionIndex = 0;
	
	if (!_IndexStack.Pop(&dwSessionIndex))
		return pSession;

	pSession = &_SessionArr[dwSessionIndex];

	pSession->socket = clientSock;
	pSession->sockAddr = clientAddr;
	pSession->SessionID = _SessionID++;
	InterlockedExchange(&pSession->lUsed, true);

	InterlockedIncrement(&_lSessionSize);

	return pSession;
}

bool CLanServer::ReleaseSession(SESSION * pSession)
{
	if (pSession == NULL) {

		printf("Session NULL\n");
		return false;
	}

	SOCKET tmpSock = pSession->socket;

	for (int i = 0; i < dfMAX_SESSION; i++) {

		if (&_SessionArr[i] != pSession) {
			continue;
		}

		if (InterlockedCompareExchange(&pSession->lUsed, false, true)) {

			_IndexStack.Push(i);
			break;
		}
	}

	closesocket(tmpSock);

	InterlockedDecrement(&_lSessionSize);

	return true;
}

SESSION * CLanServer::FindSession(SESSION_ID SessionID)
{
	SESSION *pSession = NULL;

	for (int i = 0; i < dfMAX_SESSION; i++) {

		if (SessionID == _SessionArr[i].SessionID) {

			pSession = &_SessionArr[i];
			break;
		}
	}

	return pSession;
}

int CLanServer::SendPacket(SESSION_ID SessionID, CPacket *pPack)
{
	SESSION *pSession = FindSession(SessionID);
	if (pSession == NULL) {

		printf("pLocalSession NULL\n");
		return 0;
	}
	
	// ���� ID�� pSession->SessionID ������ Ȯ�� (���Ҵ����� Ȯ��)
	
	// RefCount ����

	pSession->SendQ.Enque(pPack);

	SendPost(pSession);

	// RefCount ����

	return true;
}

int CLanServer::Disconnect(SESSION_ID SessionID)
{
	SESSION *pSession = NULL;
	pSession = FindSession(SessionID);
	shutdown(pSession->socket, SD_BOTH);
	return 0;
}

#include "CNetworkLib.h"

CNetworkLib::CNetworkLib()
{
	NetInit();
}


CNetworkLib::~CNetworkLib()
{
	// closesocket(listenSock)
	closesocket(_listenSock);

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

DWORD * CNetworkLib::GetThreadID()
{
	return _dwNetThreadID;
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
		pSession = new SESSION;
		if (pSession == NULL) {
			// Log Error
			printf("Memory insufficiency\n");
			continue;
		}
		pSession->socket = clientSock;
		pSession->sockAddr = clientAddr;
		pSession->lSendFlag = false;

		// Create IOCP
		CreateIoCompletionPort((HANDLE)clientSock, pNet->_hNetCP, (ULONG_PTR)pSession, 0);

		// WSARecvPost
		//RecvPost(pSession);

		// 정상적인 클라 세션
		SessionLock(pNet->_SrwSession);
		pNet->_SessionMap.insert(make_pair(pNet->_dwSessionID, pSession));
		SessionUnlock(pNet->_SrwSession);

		WCHAR wchBuf[22] = { 0, };
		wprintf(L"[Accept Thread] Client IP : %s, Client Port : %d\n",
			InetNtopW(AF_INET, &clientAddr.sin_addr, wchBuf, 22), ntohs(clientAddr.sin_port));

	}

	printf("Accept Thread Exit\n");

	return 0;
}

unsigned int __stdcall CNetworkLib::WorkerThread(void * arg)
{
	return 0;
}

int CNetworkLib::NetInit()
{
	int iError = 0;

	// setlocale
	setlocale(LC_ALL,"");

	// Init Srw
	InitializeSRWLock(&_SrwSession);
	
	// WsaStart
	if (WSAStartup(MAKEWORD(2, 2), &_NetWsa) != 0) {
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
	if (_hNetCP = NULL) {
		//Log SystemError
		printf("!!!ERROR!!! CreateIoCompletionPort()\n");
		return 1;
	}
	
	// Create Thread
	_hNetThread[0] = (HANDLE)_beginthreadex(NULL, 0, AcceptThread, this, 0, (unsigned int *)&_dwNetThreadID[0]);
	for (int i = 0; i < dfMAX_WORKER; i++) {
		_hNetThread[i + 1] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, this, 0, (unsigned int *)&_dwNetThreadID[i + 1]);
	}

	// Thread Check
	for (int i = 0; i < dfMAX_THREAD; i++) {
		
		if (_hNetThread[i] == NULL) {

			//Log Error
			printf("!!!ERROR!!! beginthreadex() : %d\n");
			return 1;
		}	
	}

	printf("Server Init Success\n");

	return 0;
}


#include "pch.h"
#include "CList.h"
#include "Network.h"
#include "CRingBuffer.h"

#define SERV_IP		L"0.0.0.0"
#define SERV_PORT	9000

void Network_Update();

#pragma pack(1)
struct Session {
	SOCKET _sock;
	WCHAR _ip[INET_ADDRSTRLEN];
	u_short _port;

	CRingBuffer _sendQ;
	CRingBuffer _recvQ;

	Session() :
		_sendQ(64),
		_recvQ(64)
	{
	}
};
#pragma pack()

unsigned int g_seed = 0;

SOCKET g_listenSock;
map<SOCKET, Session *> g_SessionMap;

int _tmain(int argc, TCHAR *argv[])
{
	_wsetlocale(LC_ALL, L"");

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		err_display(L"WSAStartup()", TRUE);

	SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSock == INVALID_SOCKET)
		err_display(L"socket()", TRUE);

	SOCKADDR_IN servAddr;

	u_short netPort;
	WSAHtons(listenSock, SERV_PORT, &netPort);

	servAddr.sin_family = AF_INET;
	InetPton(AF_INET, SERV_IP, &servAddr.sin_addr.s_addr);
	servAddr.sin_port = netPort;

	int retval = bind(listenSock, (SOCKADDR *)&servAddr, sizeof(servAddr));
	if (retval == SOCKET_ERROR)
		err_display(L"bind()", TRUE);

	retval = listen(listenSock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_display(L"listen()", TRUE);

	//
	// Nagle ¿É¼Ç ²ô±â
	// ¹ÝÀÀ¼Óµµ: ¾à 2ms
	//
	// Nagle ¿É¼Ç ÄÑ±â
	// ¹ÝÀÀ¼Óµµ: ¾à 100ms
	//
	BOOL opt = TRUE;
	retval = setsockopt(listenSock, IPPROTO_TCP, TCP_NODELAY, (char *)&opt, sizeof(opt));
	if (retval == SOCKET_ERROR)
		err_display(L"setsockopt(TCP_NODELAY)", TRUE);

	u_long on = TRUE;
	retval = ioctlsocket(listenSock, FIONBIO, &on);
	if (retval == SOCKET_ERROR)
		err_display(L"ioctlsocket(NON_BLOCKING)", TRUE);

	{
		g_listenSock = listenSock;
	}

	for (;;) {
		srand(g_seed);
		wprintf(L"[CHANGE] SEED %u\n", g_seed);
		g_seed++;

		Network_Update();
	}

	Session *pSession;
	map<SOCKET, Session *>::iterator iterEnd = g_SessionMap.end();
	for (auto iter = g_SessionMap.begin(); iter != iterEnd; ) {
		pSession = iter->second;
		iter = g_SessionMap.erase(iter);
		delete pSession;
	}

	closesocket(listenSock);
	WSACleanup();
	return 0;
}

void Network_Update()
{
	fd_set readSet;
	fd_set writeSet;

	FD_ZERO(&readSet);
	FD_ZERO(&writeSet);
	FD_SET(g_listenSock, &readSet);

	timeval tv = { 0, 0 };

	Session *pSession;
	auto Session_IterEnd = g_SessionMap.end();
	for (auto Session_Iter = g_SessionMap.begin(); Session_Iter != Session_IterEnd; ++Session_Iter) {
		pSession = Session_Iter->second;
		if (pSession) {
			FD_SET(pSession->_sock, &readSet);
			FD_SET(pSession->_sock, &writeSet);
		}
	}
	
	int retval = select(0, &readSet, &writeSet, NULL, NULL);
	if (retval == SOCKET_ERROR)
		err_display(L"select()", FALSE);
	else if (0 < retval) {
		int count = 0;
		do {
			if (FD_ISSET(g_listenSock, &readSet)) {
				count++;

				if (g_SessionMap.size() == (FD_SETSIZE + 1))
				//if (g_SessionMap.size() == 1)
					break;

				SOCKET sock;
				SOCKADDR_IN clntAddr;
				int addrlen = sizeof(clntAddr);

				sock = accept(g_listenSock, (SOCKADDR *)&clntAddr, &addrlen);
				if (sock == INVALID_SOCKET) {
					if (WSAGetLastError() != WSAEWOULDBLOCK) {
						err_display(L"accept()", TRUE);
					}
					break;
				}

				if (g_SessionMap.find(sock) != g_SessionMap.end()) {
					wprintf(L"[%d] ÀÌ¹Ì Á¸ÀçÇÏ´Â ¼ÒÄÏ...\n", sock);
					closesocket(sock);
					break;
				}

				Session *pSession = new Session;
				
				pSession->_sock = sock;
				InetNtop(clntAddr.sin_family, &clntAddr.sin_addr.s_addr, pSession->_ip, INET_ADDRSTRLEN);
				WSANtohs(sock, clntAddr.sin_port, &pSession->_port);

				g_SessionMap.insert(make_pair(sock, pSession));

				wprintf(L"[%d] ·Î±×ÀÎ ÇÏ¿³½À´Ï´Ù. [%s:%u] (ÃÑ %u¸í)\n", pSession->_sock, pSession->_ip, pSession->_port, g_SessionMap.size());
			}
		} while (0);

		map<SOCKET, Session *>::iterator iter;
		map<SOCKET, Session *>::iterator iterEnd = g_SessionMap.end();

		for (int i = 0; i < FD_SETSIZE && count < retval; ++i) {
			// recv
			iter = g_SessionMap.find(readSet.fd_array[i]);
			do {
				if (iter != iterEnd) {
					pSession = iter->second;
					if (pSession && FD_ISSET(pSession->_sock, &readSet)) {
						count++;

						//char buf[64];
						//int len = recv(pSession->_sock, (char *)buf, 64-1, 0);
						int len = recv(pSession->_sock, pSession->_sendQ.frontPtr(), pSession->_sendQ.gaplessPutSize(), 0);
						if (len == 0 || len == SOCKET_ERROR) {
							int err = WSAGetLastError();
							if (err == WSAEWOULDBLOCK)
								break;
							else if (err == ERROR_SUCCESS)
								break;

							closesocket(pSession->_sock);
							g_SessionMap.erase(pSession->_sock);
							wprintf(L"[%d] ·Î±×¾Æ¿ô ÇÏ¿³½À´Ï´Ù. [%s:%u] (ÃÑ %u¸í)\n", pSession->_sock, pSession->_ip, pSession->_port, g_SessionMap.size());
							delete pSession;
							break;
						}
						//printf("[%d] recv len: %d\n", pSession->_sock, len);
						pSession->_sendQ.moveFront(len);

						//int ret = pSession->_sendQ.enqueue(buf, len);
						//if (ret != len) {
						//	closesocket(pSession->_sock);
						//	g_SessionMap.erase(pSession->_sock);
						//	wprintf(L"[%d] ·Î±×¾Æ¿ô ÇÏ¿³½À´Ï´Ù. [%s:%u] (ÃÑ %u¸í)\n", pSession->_sock, pSession->_ip, pSession->_port, g_SessionMap.size());
						//}

						//int len = recv(pSession->_sock, (char *)pSession->_recvQ.frontPtr(), pSession->_recvQ.gaplessPutSize(), 0);
						//if (len == 0 || len == SOCKET_ERROR) {
						//	if (WSAGetLastError() != WSAEWOULDBLOCK) {
						//		FD_CLR(pSession->_sock, &readSet);
						//		FD_CLR(pSession->_sock, &writeSet);
						//		closesocket(pSession->_sock);
						//		g_SessionMap.erase(pSession->_sock);
						//		wprintf(L"[%d] ·Î±×¾Æ¿ô ÇÏ¿³½À´Ï´Ù. [%s:%u] (ÃÑ %u¸í)\n", pSession->_sock, pSession->_ip, pSession->_port, g_SessionMap.size());
						//	}
						//	break;
						//}
						//pSession->_recvQ.moveFront(len);

						//char buff[64];
						//int recvLen = pSession->_recvQ.gaplessPutSize();
						//int remain = len - recvLen;
						//len = pSession->_recvQ.peek(buff, len);
						//buff[len] = 0x00;
						//printf("[recv] %s (%d)\n", buff, len);

						//// echo ¿ë
						//len = pSession->_recvQ.dequeue(pSession->_sendQ.frontPtr(), len);
						//pSession->_sendQ.moveFront(len);
					}
				}
			} while (0);

			// send
			iter = g_SessionMap.find(writeSet.fd_array[i]);
			do {
				if (iter != iterEnd) {
					pSession = iter->second;
					if (pSession && FD_ISSET(pSession->_sock, &writeSet)) {
						count++;

						if (!pSession->_sendQ.empty()) {
							do {
								//char buf[64];
								//int ret = pSession->_sendQ.peek(buf, pSession->_sendQ.gaplessGetSize());
								//int len = send(pSession->_sock, (char *)buf, ret, 0);
								int len = send(pSession->_sock, (char *)pSession->_sendQ.rearPtr(), pSession->_sendQ.gaplessGetSize(), 0);
								if (len == SOCKET_ERROR) {
									int err = WSAGetLastError();
									if (err == WSAEWOULDBLOCK)
										continue;
									else if (err == ERROR_SUCCESS)
										continue;

									closesocket(pSession->_sock);
									g_SessionMap.erase(pSession->_sock);
									wprintf(L"[%d] ·Î±×¾Æ¿ô ÇÏ¿³½À´Ï´Ù. [%s:%u] (ÃÑ %u¸í)\n", pSession->_sock, pSession->_ip, pSession->_port, g_SessionMap.size());
									delete pSession;
									break;
								}
								//printf("[%d] send len: %d\n", pSession->_sock, len);
								pSession->_sendQ.moveRear(len);
							} while (pSession->_sendQ.activateSize());
						}

						//if (!pSession->_sendQ.empty()) {
						//	char buff[65];
						//	int gap = pSession->_sendQ.gaplessGetSize();
						//	pSession->_sendQ.peek(buff, gap);
						//	buff[gap] = 0x00;
						//	printf("[send] %s\n", buff);

						//	int len = send(pSession->_sock, (char *)pSession->_sendQ.rearPtr(), pSession->_sendQ.gaplessGetSize(), 0);
						//	if (len == SOCKET_ERROR) {
						//		if (WSAGetLastError() != WSAEWOULDBLOCK) {
						//			FD_CLR(pSession->_sock, &writeSet);
						//			FD_CLR(pSession->_sock, &writeSet);
						//			closesocket(pSession->_sock);
						//			g_SessionMap.erase(pSession->_sock);
						//			wprintf(L"[%d] ·Î±×¾Æ¿ô ÇÏ¿³½À´Ï´Ù. [%s:%u] (ÃÑ %u¸í)\n", pSession->_sock, pSession->_ip, pSession->_port, g_SessionMap.size());
						//		}
						//		break;
						//	}
						//	pSession->_sendQ.moveRear(len);
						//}
					}
				}
			} while (0);
		}
	}
}
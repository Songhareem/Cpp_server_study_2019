#pragma once

#ifndef __CLIENT__
#define __CLIENT__

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <list>
#include "Protocol.h"

#define SERVERIP	L"127.0.0.1"

WSADATA g_wsa;

SOCKADDR_IN g_ServerAddr;

bool NetworkInit() {

	int Retval;

	// 윈속 시작
	if (WSAStartup(MAKEWORD(2, 2), &g_wsa) != 0)
		return false;

	// 서버 세션 초기화
	ZeroMemory(&g_ServerAddr, sizeof(SOCKADDR_IN));
	g_ServerAddr.sin_family = AF_INET;
	InetPton(AF_INET,SERVERIP,&g_ServerAddr.sin_addr);
	g_ServerAddr.sin_port = htons(dfNETWORK_PORT);
	
	// socket 생성 1500개까지
	SOCKET *pClientSock = new SOCKET;
	*pClientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (*pClientSock == INVALID_SOCKET) {
		printf("Socket() Error\n");
		return false;
	}

	// 1500개 연결
	Retval = connect(*pClientSock, (SOCKADDR *)&g_ServerAddr, sizeof(g_ServerAddr));
	if (Retval == SOCKET_ERROR) {
		printf("connect() Error\n");
		return false;
	}

	return true;
}

void NetworkClose() {

	WSACleanup();
}

bool NetworkProc() {

	FD_SET ReadSet;
	FD_SET WriteSet;
	FD_ZERO(&ReadSet);
	FD_ZERO(&WriteSet);


}

#endif
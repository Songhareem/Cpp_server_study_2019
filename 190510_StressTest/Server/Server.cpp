//

#include "pch.h"
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Server.h"

#pragma comment(lib,"ws2_32")

int main()
{
	setlocale(LC_ALL, " ");

	if (!NetworkInitial()) {

		wprintf(L"네트워크 초기화 에러\n");
		return 0;
	}

	for (;;) {

		NetworkProcess();

		// 특정 키 및 상황에 따른 종료 처리
		//if (KeyProc()) 
		//	break;
	}

	// 서버 마무리 종료작업
	NetworkClose();
	return 0;
}

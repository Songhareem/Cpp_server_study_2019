// main.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <iostream>

#define SERVERPORT 9000
#define BUFSIZE    512

// 소켓함수 오류출력 후 종료
void ErrQuit(WCHAR *msg) {

	LPVOID IpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&IpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPTSTR)IpMsgBuf, msg, MB_ICONERROR);
	LocalFree(IpMsgBuf);
	exit(1);
}

// 소켓함수 오류출력
void ErrDisplay(WCHAR *msg) {

	LPVOID IpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&IpMsgBuf, 0, NULL);
	wprintf(L"[%s] %s", msg, (WCHAR *)IpMsgBuf);
	LocalFree(IpMsgBuf);
}

int main()
{
	// 유니코드 표기를 위해 필요한 초기화
	setlocale(LC_ALL, "");

	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {

		return 1;
	}

	// socket()
	SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSock == INVALID_SOCKET) ErrQuit((WCHAR *)L"socket()");

	// bind() 내 소켓과 포트 연결
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	InetPton(AF_INET, L"0,0,0,0", &serverAddr.sin_addr);
	//serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(SERVERPORT);
	retval = bind(listenSock, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR) ErrQuit((WCHAR *)L"bind()");

	//listen()
	retval = listen(listenSock, SOMAXCONN);
	if (retval == SOCKET_ERROR) ErrQuit((WCHAR *)L"listen()");

	// 데이터 통신에 사용할 변수
	SOCKET clientSock;
	SOCKADDR_IN clientAddr;
	WCHAR szClientIP[16] = { 0 };
	int addrLen;
	WCHAR buf[BUFSIZE + 1];

	wprintf(L"Echo Server\n\n");

	while (1) {

		//accept()
		addrLen = sizeof(clientAddr);
		clientSock = accept(listenSock, (SOCKADDR *)&clientAddr, &addrLen);
		if (clientSock == INVALID_SOCKET) {

			ErrDisplay((WCHAR *)L"accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		InetNtop(AF_INET, &clientAddr.sin_addr, szClientIP, 16);
		wprintf(L"\n[TCP 서버] 클라이언트 접속 : IP 주소 = %s, 포트 번호 = %d\n",
			szClientIP, ntohs(clientAddr.sin_port));

		// 클라이언트와 데이터 통신
		while (1) {

			ZeroMemory(&buf, sizeof(buf));

			// 데이터 받기
			retval = recv(clientSock, (char *)buf, BUFSIZE+1, 0);
			if (retval == SOCKET_ERROR) {
				ErrDisplay((WCHAR *)L"recv()");
				break;
			}

			else if (retval == 0) // recv 의 리턴값은 성공시 수신한 바이트 수 리턴
				break;

			// 받은 데이터 출력
			buf[retval] = '\0';
			wprintf(L"[TCP/%s:%d] %s\n", szClientIP, 
				ntohs(clientAddr.sin_port), buf);

			// 데이터 보내기
			retval = send(clientSock, (char *)buf, retval, 0);
			if (retval == SOCKET_ERROR) {

				ErrDisplay((WCHAR *)L"send()");
				break;
			}
		}


		// closeSocket()
		closesocket(clientSock);
		wprintf(L"[TCP 서버] 클라이언트 종료: IP 주소 = %s, 포트번호 = %d\n",
			szClientIP, ntohs(clientAddr.sin_port));
	}

	// closeSocket()
	closesocket(listenSock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.

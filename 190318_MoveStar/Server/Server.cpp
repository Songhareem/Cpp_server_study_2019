// server.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

// port 문제는 REUSE_PORT 옵션으로 해결해볼 것

// Disconnect
// list보다는 map이 낫다 ( STL 공부 필요, map 은 레드블랙트리 )
// iter를 인자로 하지 말것, ID 같은걸 받고 다시 iterator 검색을 해줄 것 

#include "pch.h"
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <list>

#pragma comment(lib,"ws2_32")

using namespace std;

#define SERVERIP	L"0.0.0.0"
#define SERVERPORT	3000
#define SESSIONLEN	63
#define INIT_X		40
#define INIT_Y		11

// -------------------------
// 구조체 선언
// -------------------------
typedef struct st_PACKET {

	int Type;
	int ID;
	int X;
	int Y;
} PACKET;

typedef struct st_SESSION {

	SOCKET sock;
	int ID;
	int X;
	int Y;

} SESSION;

// -------------------------
// 함수 선언
// -------------------------
void ErrQuit(WCHAR *msg);
void ErrDisplay(WCHAR *msg);
void Init();
void NetworkProc();
void SetStar(int iX, int iY);
void Render();

// -------------------------
// 전역변수 선언
// -------------------------
SOCKET g_listenSock;
list<SESSION> g_SessionList;
unsigned int g_ID = 0;

int main()
{
	// 초기화
	int retval;

	Init();

	// 윈속 시작
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// listen sock()
	g_listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_listenSock == INVALID_SOCKET) {
		ErrQuit((WCHAR *)"sock()");
		return 1;
	}

	// bind()
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	InetPton(AF_INET, L"0,0,0,0", &serverAddr.sin_addr);
	serverAddr.sin_port = htons(SERVERPORT);

	retval = bind(g_listenSock, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR) {
		ErrQuit((WCHAR *)"bind()");
		return 1;
	}

	// listen()
	retval = listen(g_listenSock, SOMAXCONN);
	if (retval == SOCKET_ERROR) {
		ErrQuit((WCHAR *)"listen()");
		return 1;
	}

	for (;;) {

		// 키입력

		// 네트워크 처리
		NetworkProc();

		// 랜더링
		Render();
	}

	// closeSock(listen)
	closesocket(g_listenSock);

	// 윈속 종료
	WSACleanup();

	return 0;
}

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

void Init() {

	system("mode con: cols=80 lines=23");

	CONSOLE_CURSOR_INFO cursorInfo = { 0, };

	cursorInfo.dwSize = 1;

	cursorInfo.bVisible = FALSE;

	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

	setlocale(LC_ALL, "");
}

int send_Unicast(SOCKET clientSock, PACKET Pack) {

	int retval = send(clientSock, (char *)&Pack, sizeof(PACKET), 0);
	if (retval == SOCKET_ERROR) {
		ErrDisplay((WCHAR *)L"send_Unicast()");
	}

	return 0;
}

int send_Broadcast(SOCKET clientSock, PACKET Pack) {

	int retval;

	list<SESSION>::iterator iter = g_SessionList.begin();

	for (; iter != g_SessionList.end(); ++iter) {

		if ((*iter).sock == clientSock)
			continue;

		retval = send((*iter).sock, (char *)&Pack, sizeof(PACKET), 0);
		if (retval == SOCKET_ERROR) {
			ErrDisplay((WCHAR *)L"send_Broadcast()");
		}
	}

	return 0;
}

// list보다는 map이 낫다
// iter를 인자로 하지 말것, ID 같은걸 받고 다시 iterator 검색을 해줄 것 
list<SESSION>::iterator Disconnect(list<SESSION>::iterator iter) {

	list<SESSION>::iterator In_iter;

	PACKET Pack;
	ZeroMemory(&Pack, sizeof(Pack));

	Pack.Type = 2;
	Pack.ID = (*iter).ID;

	closesocket((*iter).sock);
	In_iter = g_SessionList.erase(iter);

	send_Broadcast(NULL, Pack);
	return In_iter;
}

void MoveStar(int ID, int X, int Y) {

	SESSION clientSession;
	PACKET Pack;
	ZeroMemory(&Pack, sizeof(Pack));

	list<SESSION>::iterator iter;

	for (iter = g_SessionList.begin(); iter != g_SessionList.end(); ++iter) {
		
		if ((*iter).ID == ID) {
			clientSession = (*iter);
			
			(*iter).X = X;
			(*iter).Y = Y;
			
			break;
		}
	}

	for (iter = g_SessionList.begin(); iter != g_SessionList.end(); ++iter) {

		Pack.Type = 3;
		Pack.ID = ID;
		Pack.X = X;
		Pack.Y = Y;
		
		// Move 날린 소켓 제외
		send_Broadcast(clientSession.sock, Pack);

		break;
	}
}

void CreateStar(SESSION clientSession) {

	PACKET Pack;
	ZeroMemory(&Pack, sizeof(Pack));

	list<SESSION>::iterator iter = g_SessionList.begin();

	clientSession.ID = g_ID;
	clientSession.X = INIT_X;
	clientSession.Y = INIT_Y;
	g_SessionList.push_back(clientSession);

	int size = g_SessionList.size();

	// 신규 클라이언트 ID 할당
	Pack.Type = 0;
	Pack.ID = g_ID;

	send_Unicast(clientSession.sock, Pack);

	// 나머지 클라이언트에게 신규 클라이언트 생성 정보 뿌리기
	Pack.Type = 1;

	// 기존 클라는 신규 클라 정보 받음
	for (; iter != g_SessionList.end(); ++iter) {

		if ((*iter).ID == g_ID)
			continue;
		
		Pack.X = INIT_X;
		Pack.Y = INIT_Y;

		send_Unicast((*iter).sock, Pack);
	}

	// 신규 클라는 모두의 정보 받음
	for (iter = g_SessionList.begin(); iter != g_SessionList.end(); ++iter) {

		Pack.ID = (*iter).ID;
		Pack.X = (*iter).X;
		Pack.Y = (*iter).Y;

		send_Unicast(clientSession.sock, Pack);
	}

	g_ID++;
}

void NetworkProc() {

	for (;;) {
		
		FD_SET Readset;
		FD_ZERO(&Readset);
		timeval time;
		time.tv_sec = 0;
		time.tv_usec = 0;
		FD_SET(g_listenSock, &Readset);

		list<SESSION>::iterator iter;

		for (iter = g_SessionList.begin(); iter != g_SessionList.end(); ++iter) {

			FD_SET((*iter).sock, &Readset);
		}

		int retval = select(0, &Readset, NULL, NULL, &time);
		if (retval <= 0)
			return;

		if (g_SessionList.size() > 0) {

			for (iter = g_SessionList.begin(); iter != g_SessionList.end();) {

				if (FD_ISSET((*iter).sock, &Readset)) {

					PACKET Pack;
					ZeroMemory(&Pack, sizeof(Pack));

					// 수신부
					retval = recv((*iter).sock, (char *)&Pack, sizeof(Pack), 0);

					// 예외처리
					if (retval == 0) {

						iter = Disconnect(iter);
						continue;
					}
					else if (retval == SOCKET_ERROR) {

						int ErrCode = WSAGetLastError();
						// Log 남기기
						iter = Disconnect(iter);
						continue;
					}

					// 제대로 온 데이터 처리
					switch (Pack.Type) {
					case 3:
						MoveStar(Pack.ID, Pack.X, Pack.Y);
						break;
					}

					++iter;
				}

				else
					++iter;
			}
		}

		// accept()
		if (FD_ISSET(g_listenSock, &Readset)) {

			SESSION clientSession;
			ZeroMemory(&clientSession, sizeof(clientSession));
			SOCKADDR_IN clientAddr;
			int addrLen = sizeof(clientAddr);

			clientSession.sock = accept(g_listenSock, (SOCKADDR *)&clientAddr, &addrLen);
			if (clientSession.sock == INVALID_SOCKET) {
				ErrDisplay((WCHAR *)L"accept()");
			}

			CreateStar(clientSession);
		}
	}
}

void SetStar(int iX, int iY) {

	COORD pos = { (SHORT)iX, (SHORT)iY };

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

	printf("*");
}

void Render() {

	system("cls");

	// 별 표시
	if (g_SessionList.size() != 0) {
		list<SESSION>::iterator iter = g_SessionList.begin();

		for (; iter != g_SessionList.end(); ++iter) {
			SetStar((*iter).X, (*iter).Y);
		}
	}

	// 클라이언트 사이즈 표시
	COORD pos = { 0, 0 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
	printf("Connect Client : %d", g_SessionList.size());
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
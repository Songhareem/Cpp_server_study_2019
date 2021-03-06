// main.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

// ws2def.h 에러가 엄청 나왔다.
// 이유 : windows.h안의 winsock.h (구버전 winsock)이 포함되어 Winsock2와 재정의 충돌이 났기 때문.
// 해결 : windows.h과 winsock.h과 Winsock2의 충돌을 없애야 함
//		  WinSock2.h 를 Windows.h 보다 위에 선언하면 된다.

// Run-Time Check Failure #2 - Stack around the variable 'tmpIP' was corrupted. 에러가 발생
// 이유 : 입력을 받을 때, 받는 변수값 형식(%s %d etc..)이 값을 받을 변수(char, int etc..)보다 클 때 발생
// 해결 : 입력받는 변수값 형식과 받을 변수를 맞춰주면 됨 ( wscanf_s 사용시, _countof() 함수 사용 )

// scanf / fgets 차이
// scanf 는 입력값을 그대로 받음
// fgets 는 입력값 + '\n' 을 받음

#include "pch.h"
#include "main.h"
#include <list>
#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>

#pragma comment(lib,"ws2_32")

#define SERVERPORT	3000
#define SERVERIP	L"127.0.0.1"

using namespace std;

// ---------------------------------
// 자료형 정리
// ---------------------------------
typedef struct st_PACKET {

	int Type;
	int ID;
	int X;
	int Y;
} PACKET;

typedef struct st_SESSION {

	int ID;
	int X;
	int Y;

} SESSION;

// ----------------------------------
// 함수 정리
// ----------------------------------
void ErrQuit(WCHAR *msg);
void ErrDisplay(WCHAR *msg);
void GetServerIP(WCHAR *serverIP);
void Init();
void GetKey();
bool CreateStar(int ID, int X, int Y);
bool RemoveStar(int ID);
bool MoveStar(int ID, int X, int Y);
void NetworkSequence();
void SetStar(int iX, int iY);
void Render();

// ---------------------------------
// 전역변수 선언
// ---------------------------------
list<SESSION> g_SessionList;
int g_MyID;
SOCKET g_sock;

int main()
{
	// 초기화 작업
	Init();

	WCHAR serverIP[20] = { 0, };
	GetServerIP(serverIP);

	// winsock_start()
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	int retval = 0;

	// sock()
	g_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_sock == INVALID_SOCKET)
		ErrQuit((WCHAR *)L"sock()");

	// connect()
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	//InetPton(AF_INET, SERVERIP, &serverAddr.sin_addr);
	InetPton(AF_INET, serverIP, &serverAddr.sin_addr);
	serverAddr.sin_port = htons(SERVERPORT);

	retval = connect(g_sock, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR)
		ErrQuit((WCHAR *)L"connect()");
		
	while (1) {

		// 키 입력
		GetKey();

		// 네트워크 연결
		NetworkSequence();

		// 랜더링
		Render();
	}

	closesocket(g_sock);

	// winsock_close
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

// 서버 IP 얻기
void GetServerIP(WCHAR *serverIP) {

	WCHAR tmpIP[20];
	wprintf(L"접속할 IP 주소를 입력하세요 : ");
	int size = _countof(tmpIP);
	wscanf_s(L"%s", tmpIP, size);

	wcscpy_s(serverIP, 20, tmpIP);
}

// 초기화
void Init() {

	system("mode con: cols=80 lines=23");

	CONSOLE_CURSOR_INFO cursorInfo = { 0, };
	cursorInfo.dwSize = 1;
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

	setlocale(LC_ALL, "");

	g_MyID = NULL;
}

// 키입력 함수
void GetKey() {

	list<SESSION>::iterator iter = g_SessionList.begin();

	SESSION *pPlayer = NULL;

	for (; iter != g_SessionList.end(); ++iter) {

		if ((*iter).ID == g_MyID) {

			pPlayer = &(*iter);
			break;
		}
	}

	if (pPlayer == NULL)
		return;

	int oldX = pPlayer->X;
	int oldY = pPlayer->Y;

	if (GetKeyState(VK_UP) & 0x8000) {
		pPlayer->Y -= 1;
	}

	if (GetKeyState(VK_DOWN) & 0x8000) {
		pPlayer->Y += 1;
	}

	if (GetKeyState(VK_LEFT) & 0x8000) {
		pPlayer->X -= 1;
	}

	if (GetKeyState(VK_RIGHT) & 0x8000) {
		pPlayer->X += 1;
	}

	int newX = pPlayer->X;
	int newY = pPlayer->Y;

	// 수정필요 계속 보낼 필요 X
	if (newX != oldX || newY != oldY) {

		PACKET Pack;
		ZeroMemory(&Pack, sizeof(Pack));
		Pack.Type = 3;
		Pack.ID = g_MyID;
		Pack.X = pPlayer->X;
		Pack.Y = pPlayer->Y;

		// send()
		int retval = send(g_sock, (char *)&Pack, sizeof(Pack), 0);
		if (retval == SOCKET_ERROR)
			ErrDisplay((WCHAR *)L"send()");
	}
}

bool CreateStar(int ID, int X, int Y) {

	list<SESSION>::iterator iter = g_SessionList.begin();

	for (; iter != g_SessionList.end(); ++iter) {

		if ((*iter).ID == ID) {
			return 1;
		}
	}

	SESSION tmp;

	tmp.ID = ID;
	tmp.X = X;
	tmp.Y = Y;

	g_SessionList.push_back(tmp);

	return 0;
}

bool RemoveStar(int ID) {

	if (g_SessionList.empty() == true)
		return 1;

	list<SESSION>::iterator iter = g_SessionList.begin();

	for (; iter != g_SessionList.end();) {

		if ((*iter).ID == ID)
			iter = g_SessionList.erase(iter);
		else
			++iter;
	}

	return 0;
}

bool MoveStar(int ID, int X, int Y) {

	list<SESSION>::iterator iter = g_SessionList.begin();

	for (; iter != g_SessionList.end(); ++iter) {

		if ((*iter).ID == ID) {

			(*iter).X = X;
			(*iter).Y = Y;

			return 0;
		}
	}

	return 1;
}

void NetworkSequence() {

	for (;;) {
		
		FD_SET ReadSet;
		FD_ZERO(&ReadSet);
		FD_SET(g_sock, &ReadSet);
		timeval time;
		time.tv_sec = 0;
		time.tv_usec = 0;

		int retval = 0;
	
		retval = select(0, &ReadSet, NULL, NULL, &time);
		if (retval == 0 || retval == SOCKET_ERROR)
			return;

		if (FD_ISSET(g_sock, &ReadSet)) {

			PACKET Pack;
			retval = recv(g_sock, (char *)&Pack, sizeof(Pack), 0);

			switch (Pack.Type) {

			case 0:
				// GetID
				g_MyID = Pack.ID;
				break;
			case 1:
				// CreateStar()
				CreateStar(Pack.ID, Pack.X, Pack.Y);
				break;
			case 2:
				// RemoveStar()
				RemoveStar(Pack.ID);
				break;
			case 3:
				// MoveStar()
				MoveStar(Pack.ID, Pack.X, Pack.Y);
				break;
			}
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

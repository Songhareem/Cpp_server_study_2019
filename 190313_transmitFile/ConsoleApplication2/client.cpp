 // ConsoleApplication2.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include "CSocketAPI.h"
#include <iostream>

#define SERVERDOMEIN	L"procademyserver.iptime.org"
#define SERVERPORT		10010
#define BUFSIZE			1000

typedef struct st_PACKET_HEADER {

	DWORD dwPacketCode;		// 0x11223344

	WCHAR szName[32];		// 본인이름, 유니코드 NULL문자 끝
	WCHAR szFileName[128];	// 파일이름, 유니코드 NULL문자 끝
	int iFileSize;

} PACKET_H;

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

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, WCHAR *buf, int len, int flags)
{
	int received;
	WCHAR *ptr = buf;
	int left = len;

	while (left > 0) {

		received = recv(s, (char *)ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;

		else if (received == 0)
			break;

		left -= received;
		ptr += received;
	}

	return(len - left);
}

int main()
{
	// 유니코드 초기화
	setlocale(LC_ALL, "");

	// 변수 초기화
	int retval;
	
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		ErrQuit((WCHAR *)L"socket()");

	// 서버 접속 //
	
	// connect()
	IN_ADDR Addr;
	CSocketAPI SockApi;

	SockApi.DomainToIP((WCHAR *)SERVERDOMEIN, &Addr);

	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr = Addr;
	serverAddr.sin_port = htons(SERVERPORT);

	retval = connect(sock, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR)
		ErrQuit((WCHAR *)L"connect()");

	// 특정파일 오픈 // 
	FILE *fp = NULL;
	fopen_s(&fp, "Beach.bmp", "rb");

	if (fp == NULL)
		return 1;

	// 사이즈 체크 //
	int FileSize = 0;

	fseek(fp, 0, SEEK_END);
	FileSize = ftell(fp);

	// 헤더 생성 //
	PACKET_H PacketHeader;

	ZeroMemory(&PacketHeader, sizeof(PacketHeader));

	PacketHeader.dwPacketCode = 0x11223344;
	wsprintf(PacketHeader.szName, L"송하림");
	wsprintf(PacketHeader.szFileName, L"Beach.bmp");
	PacketHeader.iFileSize = FileSize;

	// 데이터 보내기
	retval = send(sock, (char *)&PacketHeader, sizeof(PacketHeader), 0);
	if (retval == SOCKET_ERROR) {
		ErrDisplay((WCHAR *)L"send()");
	}

	// 서버로 파일 데이터 전송 1000단위 쪼개서 보내기 //
	
	// 데이터 통신에 사용할 변수
	//char buf[BUFSIZE];
	WCHAR buf[BUFSIZE];

	fseek(fp, 0, SEEK_SET);

	while (1) {
		
		ZeroMemory(&buf, sizeof(buf));

		// 데이터 입력
		fread(buf, 1, sizeof(buf), fp);
		FileSize -= 1000;
		
		// 데이터 보내기
		retval = send(sock, (char *)buf, sizeof(buf), 0);
		if (retval == SOCKET_ERROR) {
			ErrDisplay((WCHAR *)L"send()");
			break;
		}

		wprintf(L"[TCP 클라이언트] %d 바이트를 보냈습니다.\n", retval);

		if (FileSize <= 0)
			break;
	}

	// 파일 전송 성공시 서버에서 클라이언트로 0xdddddddd 보내줌
	
	// 데이터 받기
	int buf2;
	retval = recv(sock, (char *)&buf2, 4, 0);
	if (retval == SOCKET_ERROR)
		ErrDisplay((WCHAR *)L"recv()");

	// 받은 데이터 출력
	wprintf(L"[TCP 클라이언트] %d 바이트를 받았습니다.\n", retval);
	wprintf(L"[받은 데이터] 0x%x\n", buf2);

	// closeSocket
	closesocket(sock);

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

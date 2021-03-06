#pragma once

#include "resource.h"
#include "CRingBuffer.h"
#include "CLogMaker.h"
#include <list>

#define SERVERPORT  25000
#define SERVERIP	L"192.168.10.14"

#define UM_NETWORK  (WM_USER + 1)

// -----------------------------
// 2 byte - 패킷길이 16
// 4 byte - Start X
// 4 byte - Start Y
// 4 byte - End X
// 4 byte - End Y
// -----------------------------

// 헤더
typedef struct st_DRAW_HEADER {

	unsigned short	Len;

} HEADER;

// 패킷
typedef struct st_DRAW_PACKET {

	int		iStartX;
	int		iStartY;
	int		iEndX;
	int		iEndY;

} PACKET;

// 전역변수
HDC g_mainHDC;
HWND g_hWnd;
SOCKET g_sock;
CRingBuffer g_RecvQ(10000);
CRingBuffer g_SendQ(10000);
CLogMaker CLogMaker::Log;
PACKET g_RecvPack;
PACKET g_SendPack;
bool g_SendFlag = true;
bool g_ClickFlag = false;
bool g_DrawFlag = false;
std::list<PACKET> PackList;

// 함수
void ChangeWindowSize();
void Init();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

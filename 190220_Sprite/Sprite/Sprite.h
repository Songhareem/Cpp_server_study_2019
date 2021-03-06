#pragma once

#include "stdafx.h"
#include "resource.h"
#include "e_SPRITE.h"
#include "e_OBJECT_TYPE.h"
#include "ScreenDib.h"
#include "CSpriteDib.h"
#include "CBaseObject.h"
#include "CPlayerObject.h"
#include "CEffectObject.h"
//#include "List.h"
#include <list>
#include "CLogMaker.h"
#include "CRingBuffer.h"
#include "PacketDefine.h"

#pragma comment(lib,"Imm32.lib")
#pragma comment(lib,"winmm.lib")

using namespace std;

//-------------------------
// enum 선언
//-------------------------
enum {

	TILTE,
	GAME,
	END
};

//-------------------------
// 전역변수 선언
//-------------------------
CSpriteDib CSpriteDib::g_SpriteDib;
ScreenDib  ScreenDib::g_ScreenDib;

HDC g_mainHDC;
HWND g_hWnd;

BYTE g_GameState = GAME;
BOOL g_IsGameActivate = false;

CBaseObject *g_pPlayerObject = NULL;
list<CBaseObject *> g_ObjectList;

SOCKET g_sock;
BOOL g_netConnect = false;
BOOL g_netSendPossible = false;

CLogMaker CLogMaker::Log;
CRingBuffer			g_RecvQ(MAX_Q_LEN); // char [10000]
CRingBuffer			g_SendQ(MAX_Q_LEN); // char [10000]

//-------------------------
// 함수 선언부
//-------------------------
bool GameInit();

void Update();
void Update_Title();
void Update_Game();

BOOL FrameSkip();
void PrintFrame(int dwCounterLogic, int dwCounterRender);
void KeyProcess();
void Action();
void Draw();
void changeWindowSize();

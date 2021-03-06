// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 특정 포함 파일이 들어 있는
// 포함 파일입니다.
//

#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
#include <Windowsx.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// 여기서 프로그램에 필요한 추가 헤더를 참조합니다.
#include <stdio.h>
#include <mmsystem.h>
#include <winsock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
//--------------------------
// Define 선언
//--------------------------
#define dfFRAME		 50
#define dfONESEC	 1000
#define dfSLEEP_TIME dfONESEC/dfFRAME

#define dfRANGE_MOVE_TOP	50
#define dfRANGE_MOVE_LEFT	10
#define dfRANGE_MOVE_RIGHT	630
#define dfRANGE_MOVE_BOTTOM	470

#define dfACTION_STAND   0
#define dfACTION_MOVE_LL 1
#define dfACTION_MOVE_LU 2
#define dfACTION_MOVE_UU 3
#define dfACTION_MOVE_RU 4
#define dfACTION_MOVE_RR 5
#define dfACTION_MOVE_RD 6
#define dfACTION_MOVE_DD 7
#define dfACTION_MOVE_LD 8
#define dfACTION_ATTACK1 9
#define dfACTION_ATTACK2 10
#define dfACTION_ATTACK3 11

#define LEFT  0
#define RIGHT 4

#define dfDELAY_STAND	5
#define dfDELAY_MOVE	4
#define dfDELAY_ATTACK1	3
#define dfDELAY_ATTACK2	4
#define dfDELAY_ATTACK3	4
#define dfDELAY_EFFECT	3

#define MAX_IMAGE 100
#define COLOR_KEY 0xffffffff

#define MAX_Q_LEN	10000
#define UM_NETWORK  (WM_USER + 1)

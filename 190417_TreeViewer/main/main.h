#pragma once

#include "resource.h"

HWND g_hWnd;
int g_KeyState;
int g_Value = 1;

void changeWindowSize();
void PrintMenualText(HDC hdc, int PosX, int PosY);
void KeyProcess();
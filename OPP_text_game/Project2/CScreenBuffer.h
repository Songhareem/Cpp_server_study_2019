#pragma once

#ifndef __CSCREENBUFFER__
#define __CSCREENBUFFER__

#include "pch.h"
#include <Windows.h>

class CScreenBuffer
{
public:

	enum {
		
		WIDTH  = 127,
		HEIGHT = 32
	};

	static CScreenBuffer *GetInstance() { return &ScreenBuff; };
	
	void Clear();
	void Flip();
	void Flip(int HEIGHT);
	void cs_MoveCursor(int iPosX, int iPosY);
	void cs_Initial();
	void Render(int ObjType, int iY, int iX, bool bDivision = 0, char c = ' ');

private:
	
	char m_Back[HEIGHT][WIDTH];
	static CScreenBuffer ScreenBuff;

	CScreenBuffer();
	~CScreenBuffer();
};

#endif
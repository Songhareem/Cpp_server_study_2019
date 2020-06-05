#pragma once

#ifndef __SCREEN_DIB__
#define __SCREEN_DIB__

class ScreenDib
{
public:
	ScreenDib(int iWidth, int iHeight, int iColorBit);
	virtual ~ScreenDib();

protected:
	void CreateDibBuffer(int iWidth, int iHeight, int iColorBit);
	void ReleaseDibBuffer();

public:

	void Flip(HWND hWnd, int iX = 0, int iY = 0);

	BYTE *GetDibBuffer();
	int GetWidth();
	int getHeight();
	int GetPitch();

protected:
	
	BITMAPINFO	m_stDibInfo;
	BYTE		*m_byteBuffer;

	int			m_iWidth;
	int			m_iHeight;
	int			m_iPitch;
	int			m_iColorBit;
	int			m_iBufferSize;
};

#endif
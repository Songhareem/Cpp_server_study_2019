#pragma once

#ifndef __SCREEN_DIB__
#define __SCREEN_DIB__

#define WIDTH  640
#define HEIGHT 480
#define PIXEL  32

class ScreenDib
{
public:

	static ScreenDib *GetInstance() { return &g_ScreenDib; };

	void Flip(HWND hWnd, int iX = 0, int iY = 0);

	BYTE *GetDibBuffer();
	int GetWidth();
	int getHeight();
	int GetPitch();

protected:
	void CreateDibBuffer(int iWidth, int iHeight, int iColorBit);
	void ReleaseDibBuffer();

private:
	
	ScreenDib(int iWidth = WIDTH, int iHeight = HEIGHT, int iColorBit = PIXEL);
	virtual ~ScreenDib();

protected:

	BITMAPINFO	m_stDibInfo;
	BYTE		*m_byteBuffer;

	int			m_iWidth;
	int			m_iHeight;
	int			m_iPitch;
	int			m_iColorBit;
	int			m_iBufferSize;

private:
	
	static ScreenDib g_ScreenDib;

};

#endif
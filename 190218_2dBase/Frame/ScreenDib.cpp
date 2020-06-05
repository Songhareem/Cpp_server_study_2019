#include "stdafx.h"
#include "ScreenDib.h"


ScreenDib::ScreenDib(int iWidth, int iHeight, int iColorBit) : m_iWidth(iWidth), m_iHeight(iHeight), m_iColorBit(iColorBit)
{
	m_iPitch = (m_iWidth * (iColorBit / 8) + 3) & ~3;
	m_iBufferSize = m_iPitch * iHeight * (iColorBit / 8);

	CreateDibBuffer(iWidth, iHeight,iColorBit);
}

ScreenDib::~ScreenDib()
{
	ReleaseDibBuffer();
}

void ScreenDib::CreateDibBuffer(int iWidth, int iHeight, int iColorBit) {

	//------------------------
	// BitmapInfoHeader init
	//------------------------
	m_stDibInfo.bmiHeader.biSize = sizeof(BITMAPINFO);
	m_stDibInfo.bmiHeader.biWidth = m_iWidth;
	m_stDibInfo.bmiHeader.biHeight = m_iHeight;
	m_stDibInfo.bmiHeader.biPlanes = 1;
	m_stDibInfo.bmiHeader.biBitCount = m_iColorBit;
	m_stDibInfo.bmiHeader.biCompression = 0;
	m_stDibInfo.bmiHeader.biSizeImage = m_iBufferSize;
	m_stDibInfo.bmiHeader.biXPelsPerMeter = 0;
	m_stDibInfo.bmiHeader.biYPelsPerMeter = 0;
	m_stDibInfo.bmiHeader.biClrUsed = 0;
	m_stDibInfo.bmiHeader.biClrImportant = 0;

	m_byteBuffer = new BYTE[m_iBufferSize];
}
void ScreenDib::ReleaseDibBuffer() {

	delete[] m_byteBuffer;
}

void ScreenDib::Flip(HWND hWnd, int iX, int iY) {

	HDC hdc = GetDC(hWnd);
	RECT wndRect;
	
	GetClientRect(hWnd, &wndRect);

	StretchDIBits(hdc, iX, iY, m_iWidth, m_iHeight, 0, 0, m_iWidth, m_iHeight, m_byteBuffer, &m_stDibInfo, DIB_RGB_COLORS, SRCCOPY);

	ReleaseDC(hWnd, hdc);
}

BYTE *ScreenDib::GetDibBuffer() { return m_byteBuffer; }
int ScreenDib::GetWidth() { return m_iWidth; }
int ScreenDib::getHeight() { return m_iHeight; }
int ScreenDib::GetPitch() { return m_iPitch; }
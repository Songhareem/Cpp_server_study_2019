#include "stdafx.h"
#include "Frame.h"
#include "ScreenDib.h"
#include <stdio.h>

using namespace std;

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int Cnt;

ScreenDib g_cScreenDib(640,480,32);
HWND g_hWnd;

void Run();
void Run2();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	
	// 응용 프로그램 초기화를 수행합니다:
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FRAME));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDI_FRAME);
	wcex.lpszClassName = L"FramePractice";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_FRAME));

	RegisterClassExW(&wcex);

	HWND hWnd = CreateWindowW(L"FramePractice", L"FramePractice", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	g_hWnd = hWnd;

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;

	while (1) {

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

			if (msg.message == WM_QUIT) 
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 기본 로직
		else {

			Run();
			//Sleep(100);
		}
	}

	return (int)msg.wParam;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다:
		switch (wmId)
		{
		case IDM_ABOUT:
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_CREATE:
	{
	
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_LBUTTONDOWN:

		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void Run() {

	BYTE *Dest = g_cScreenDib.GetDibBuffer();
	int iDestWidth = g_cScreenDib.GetWidth();
	int iDestHeight = g_cScreenDib.getHeight();
	int iDestPitch = g_cScreenDib.GetPitch();

	BYTE byGrayColor = 0;
	for (int i = 0; i < 480; i++) {

		memset(Dest, byGrayColor, 640 * 4);
		Dest += iDestPitch;
		byGrayColor++;
	}

	Dest = g_cScreenDib.GetDibBuffer();
	Dest += (iDestWidth/2)*4;
	int byRedColor = 0x0000ff00;
	for (int i = 0; i < 480; i++) {

		*Dest		= 0x00;
		*(Dest + 1) = 0x00;
		*(Dest + 2) = 0xff;
		
		Dest += iDestPitch;
	}
	g_cScreenDib.Flip(g_hWnd);
}

void Run2() {

	// read Bitmap
	FILE *fp = NULL;
	BITMAPFILEHEADER bmfh;
	DWORD dwFileSize, dwDibSize;
	BYTE * pDib = NULL;
	
	fopen_s(&fp, "test.bmp", "rb");
	fseek(fp, 0, SEEK_END);
	dwFileSize = (DWORD)ftell(fp);
	dwDibSize = dwFileSize - sizeof(BITMAPFILEHEADER);
	pDib = new BYTE[dwDibSize];

	fseek(fp, 0, SEEK_SET);
	fread(&bmfh, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(pDib, dwDibSize, 1, fp);

	fclose(fp);

	// print Bitmap
	BITMAPINFOHEADER * lpbmih;
	BYTE* lpvBits;
	int w, h, c;

	lpbmih = (BITMAPINFOHEADER*)pDib;  //pDib는 BITMAPINFOHEADER의 시작주소를 가르키고 있음
	w = lpbmih->biWidth;  //가로크기 저장
	h = lpbmih->biHeight;  //세로크기 저장
	c = lpbmih->biBitCount;  //비트수 저장

	//비트맵 정보 시작 위치를 계산한다.

	if (c == 24)  //색상테이블이 존재하지 않음
		lpvBits = (BYTE*)pDib + sizeof(BITMAPINFOHEADER);  //픽셀 더이터의 시작주소를 저장


	else  //색상테이블이 존재
		lpvBits = (BYTE*)pDib + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*(1 << c);

	//DC를 얻어온다.
	//CClientDC dc(this);

	//SetDIBitsToDevice(dc.m_hDC, point.x, point.y, w, h, 0, 0, 0, h, lpvBits, (BITMAPINFO*)pDib, DIB_RGB_COLORS);
	// 비트맵을 화면에 출력하는 함수 
	//디바이스 컨텍스트의 핸들 , 마우스를 클릭한 x 좌표, y좌표 , ......

    //동적 할당한 메모리 해제
	delete[] pDib;

}
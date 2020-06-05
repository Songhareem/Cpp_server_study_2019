// WindowsProject1.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "PenDrawing.h"

using namespace std;

typedef struct {

	int x;
	int y;

}Location;

list<Location> pos;
bool g_clickFlag = false;
bool g_colorFlag = false;

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	//rand 초기화
	srand(time(NULL));
	// 응용 프로그램 초기화를 수행합니다:
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PENDRAWING));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDI_PENDRAWING);
	wcex.lpszClassName = L"SimpleDrawing";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

	HWND hWnd = CreateWindowW(L"SimpleDrawing", L"PenDrawing", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;

	// 기본 메시지 루프입니다:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
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
	case WM_RBUTTONDOWN:
	{
		g_colorFlag = true;
	}
	break;
	case WM_LBUTTONDOWN:
	{
		g_clickFlag = true;

		Location start;
		start.x = GET_X_LPARAM(lParam);
		start.y = GET_Y_LPARAM(lParam);
		pos.push_back(start);
	}
	break;
	case WM_LBUTTONUP:
	{
		g_clickFlag = false;

		pos.clear();
	}
	break;
	case WM_MOUSEMOVE:
	{
		if (g_clickFlag != true) break;
		Location end;
		end.x = GET_X_LPARAM(lParam);
		end.y = GET_Y_LPARAM(lParam);
		pos.push_back(end);

		InvalidateRect(hWnd, NULL, FALSE);
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		/*static HPEN tmpPen;

		if (g_colorFlag == true) {
		
			int penWidth = rand() % 10 + 1;
			int red = rand() % 255 + 1;
			int green = rand() % 255 + 1;
			int blue = rand() % 255 + 1;

			HPEN hPen = CreatePen(PS_SOLID, penWidth, RGB(red, green, blue));
			tmpPen = hPen;
			g_colorFlag = false;
		}

		HPEN hOldPen = (HPEN)SelectObject(hdc, tmpPen);
		DeleteObject(hOldPen);*/

		do {
			if (g_clickFlag != true) break;

			Location start;
			Location end;
			start = pos.front();
			pos.pop_front();
			end = pos.front();

			MoveToEx(hdc, start.x, start.y, NULL);
			LineTo(hdc, end.x, end.y);

		} while (0);

		EndPaint(hWnd, &ps);
	}
	break;
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

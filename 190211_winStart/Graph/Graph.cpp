#include "stdafx.h"
#include "Graph.h"

using namespace std;

list<int>intList;

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	// intList 초기화

	srand((unsigned int)time(NULL));
	for (int i = 0; i < 100; i++) {

		static int tmp;
		int ModulateY = 400;

		if (i == 0) {
			tmp = rand() % 20 + ModulateY;
			intList.push_back(tmp);
		}

		if (rand() % 2 == 0) {
			tmp = tmp + rand() % 30 + 1;
			if (tmp > 550) tmp = 500;
		}
		else {
			tmp = tmp - rand() % 30 + 1;
			if (tmp < 0) tmp = 50;
		}

		intList.push_back(tmp);
	}

	// 응용 프로그램 초기화를 수행합니다:
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GRAPH));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDI_GRAPH);
	wcex.lpszClassName = L"SimpleGraph";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

	HWND hWnd = CreateWindowW(L"SimpleGraph", L"GraphDrawing", WS_OVERLAPPEDWINDOW,
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
		SetTimer(hWnd, 1, 20, NULL);
	}
	break;
	case WM_TIMER:
	{
		switch (wParam) {

		case 1:
		{
			InvalidateRect(hWnd, NULL, TRUE);

			int tmp = intList.back();
	
			if (rand() % 2 == 0) {
				tmp = tmp + rand() % 30 + 1;
				if (tmp > 550) tmp = 500;
			}
			else {
				tmp = tmp - rand() % 30 + 1;
				if (tmp < 0) tmp = 50;
			}

			intList.pop_front();
			intList.push_back(tmp);
		}
		break;
		}

	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		list<int>::iterator iter = intList.begin();

		int AmplifyX = 10;
		
		int oldX = 0;
		int oldY = *iter;
		iter++;

		int newX;
		int newY;

		for (; iter != intList.end(); ++iter) {

			newX = oldX + 1;
			newY = *iter;

			MoveToEx(hdc, oldX*AmplifyX, oldY, NULL);
			LineTo(hdc, newX*AmplifyX, newY);

			oldX = newX;
			oldY = newY;
		}

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		KillTimer(hWnd, 1);
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


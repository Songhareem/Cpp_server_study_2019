// main.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "main.h"
#include "CJumpPoint.h"
#include <ctime>
#include <algorithm>
#include <list>

using namespace std;

#define MAX_LOADSTRING 100

CJumpPoint JumpPoint;
HWND g_hWnd;
HBITMAP hBit;
int g_ColorFlag;
list<NODE *> OpenList;
list<NODE *> CloseList;

void MakeFild();
void OnTimer();
bool isNodeNotInList(NODE *pNewNode);
void CreateNode(int ParentX, int ParentY);
//void PathFinder(int strX, int strY, int endX, int endY);
void OpenListIn(NODE *pNewNode);
void CloseListIn(NODE *pNewNode);
bool Compare(const NODE *pObject1, const NODE *pObject2);
void changeWindowSize();

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MAIN, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MAIN));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MAIN);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   g_hWnd = hWnd;

   changeWindowSize();

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
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
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_LBUTTONDOWN:
	{
		/*if (g_KeyIn == true)
			break;
*/
		int X = LOWORD(lParam);
		int Y = HIWORD(lParam);
		int Color = 0;
		JumpPoint.GetNodePos(&X, &Y);

		Color = JumpPoint.GetColor(X, Y);

		// 마우스 눌리는 순간만 플래그 한 번 세워져야함
		if (Color == WHITE) {
			g_ColorFlag = WHITE;
			//JumpPoint.SetColor(X, Y, GRAY);
		}
		else if (Color == GRAY) {
			g_ColorFlag = GRAY;
			//JumpPoint.SetColor(X, Y, WHITE);
		}
	}
	break;
	case WM_LBUTTONUP:
	{
		g_ColorFlag = NULL;
	}
	break;
	case WM_MOUSEMOVE:
	{
		if (g_ColorFlag == WHITE) {

			JumpPoint.WallUp(LOWORD(lParam), HIWORD(lParam));
		}

		else if (g_ColorFlag == GRAY) {

			JumpPoint.WallDown(LOWORD(lParam), HIWORD(lParam));
		}
	}
	break;
	case WM_LBUTTONDBLCLK:
	{
		JumpPoint.MoveStartNode(LOWORD(lParam), HIWORD(lParam));
		NODE *pStartNode = JumpPoint.GetStartPtr();
		OpenList.clear();
		OpenList.push_back(pStartNode);
	}
	break;
	case WM_RBUTTONDOWN:
	{
		JumpPoint.MoveEndNode(LOWORD(lParam), HIWORD(lParam));
	}
	break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			RECT Rect;
			// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
			GetClientRect(hWnd, &Rect);
			HDC hMemDC = CreateCompatibleDC(hdc);
			HBITMAP OldBit = (HBITMAP)SelectObject(hMemDC, hBit);
			BitBlt(hdc, 0, 0, Rect.right, Rect.bottom, hMemDC, 0, 0, SRCCOPY);
			SelectObject(hMemDC, OldBit);
			DeleteDC(hMemDC);
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_TIMER:
	{
		switch (wParam) {
		case 1:
			OnTimer();
			break;
		}
	}
	break;
	case WM_KEYDOWN :
	{
		if (GetKeyState(VK_SPACE) & 0x8000) {

			//if (OpenList.empty() == true || CloseList.empty() == true)
			//	OpenListIn(JumpPoint.GetStartPtr());

			// Run
			if (OpenList.empty() != true) {

				// 노드가 비어있지 않다면 노드 꺼내기
				NODE *pNowNode = OpenList.front();
				OpenList.pop_front();

				CloseListIn(pNowNode);

				// 꺼낸 노드가 목적지 노드 아니라면
				if (pNowNode != JumpPoint.GetEndPtr()) {

					CreateNode(pNowNode->iX, pNowNode->iY);

					// 노드 넣었으니 정렬
					OpenList.sort(Compare);
				}
			}
		}

		// C Key = Clear
		if (GetKeyState(0x43) & 0x8000) {

			OpenList.clear();
			CloseList.clear();
			JumpPoint.NodeInit();
			NODE *pNode = JumpPoint.GetStartPtr();
			OpenList.push_back(pNode);
		}
	}
	break;
	case WM_CREATE:
	{
		SetTimer(hWnd, 1, 100, NULL);
		srand(time(NULL));
		NODE *pNode = JumpPoint.GetStartPtr();
		OpenList.push_back(pNode);
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

void MakeFild() {

	RECT Rect;
	HDC hdc, hMemDC;
	HBITMAP OldBit;
	HBRUSH hBrush = NULL;
	HBRUSH OldBrush = NULL;
	HPEN hPen = NULL;
	HPEN OldPen = NULL;

	GetClientRect(g_hWnd, &Rect);
	hdc = GetDC(g_hWnd);
	if (hBit == NULL) {
		hBit = CreateCompatibleBitmap(hdc, Rect.right, Rect.bottom);
	}
	hMemDC = CreateCompatibleDC(hdc);
	OldBit = (HBITMAP)SelectObject(hMemDC, hBit);
	FillRect(hMemDC, &Rect, GetSysColorBrush(COLOR_WINDOW));

	/*if (g_LineFlag == true) {

		for (;;) {
			if (g_pLastNode == NULL)
				break;

			g_pLastNode->iColor = BLACK;
			g_pLastNode = g_pLastNode->pParent;
		}
	}*/

	for (int Y = 0; Y < NODE_SIZE::FILD_Y; Y++) {
		for (int X = 0; X < NODE_SIZE::FILD_X; X++) {

			int left = 0;
			int right = 0;
			int top = 0;
			int bottom = 0;

			JumpPoint.GetNodeRect(X, Y, left, right, top, bottom);

			// 노드 색 설정
			DWORD iColor = JumpPoint.GetColor(X, Y);
			switch (iColor) {
			case NODE_COLOR::WHITE:
				hBrush = CreateSolidBrush(RGB(255, 255, 255));
				break;
			case NODE_COLOR::BLACK:
				hBrush = CreateSolidBrush(RGB(0, 0, 0));
				break;
			case NODE_COLOR::GRAY:
				hBrush = CreateSolidBrush(RGB(128, 128, 128));
				break;
			case NODE_COLOR::RED:
				hBrush = CreateSolidBrush(RGB(255, 0, 0));
				break;
			case NODE_COLOR::GREEN:
				hBrush = CreateSolidBrush(RGB(0, 255, 0));
				break;
			case NODE_COLOR::BLUE:
				hBrush = CreateSolidBrush(RGB(0, 0, 255));
				break;
			case NODE_COLOR::YELLOW:
				hBrush = CreateSolidBrush(RGB(247, 230, 0));
				break;
			default:
				hBrush = CreateSolidBrush(iColor);
				break;
			}

			// 붓 교체 후 노드 색칠
			OldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
			Rectangle(hMemDC, left, top, right, bottom);

			// 색칠 후 되돌리기
			DeleteObject(SelectObject(hMemDC, OldBrush));
		}
	}

	SelectObject(hMemDC, OldBit);
	DeleteDC(hMemDC);
	ReleaseDC(g_hWnd, hdc);
}

void OpenListIn(NODE *pNewNode) {

	pNewNode->iColor = BLUE;
	OpenList.push_back(pNewNode);
}

void CloseListIn(NODE *pNewNode) {

	pNewNode->iColor = YELLOW;
	CloseList.push_back(pNewNode);
}

void OnTimer() {
	
	// Draw
	MakeFild();

	// Render
	InvalidateRect(g_hWnd, NULL, FALSE);
}

bool isNodeNotInList(NODE *pNewNode) {

	list<NODE *>::iterator iter;
	bool isInOpenList = false;
	bool isInCloseList = false;
	iter = find(OpenList.begin(), OpenList.end(), pNewNode);
	if (iter != OpenList.end())
		isInOpenList = true;
	iter = find(CloseList.begin(), CloseList.end(), pNewNode);
	if (iter != CloseList.end())
		isInCloseList = true;

	if (isInOpenList == false && isInCloseList == false)
		return true;

	return false;
}

// 부모의 노드만 찾아서 설정해줌 + 검색한 노드 색 변경, Jump함수 이용해서 노드 찾기, 코너 확인
void CreateNode(int ParentX, int ParentY) {

	if (ParentX < 0 || ParentX >= FILD_X)
		return;

	if (ParentY < 0 || ParentY >= FILD_Y)
		return;

	NODE *pNowNode = JumpPoint.GetNodePtr(ParentX, ParentY);
	NODE *pFindNode = NULL;

	// 검색된 노드 색칠할 Color 만들기
	BYTE Red = rand() % 255;
	BYTE Green = rand() % 255;
	BYTE Blue = rand() % 255;
	DWORD dwColor = RGB(Red, Green, Blue);
	bool doCreate;

	// 노드가 시작노드 일경우 8방으로 Jump
	if (pNowNode == JumpPoint.GetStartPtr()) {
		
		// 8방 검사
		doCreate = JumpPoint.JumpLL(ParentX - 1, ParentY, pNowNode, &pFindNode, dwColor);
		if (doCreate == true) {
			bool isNotIn = isNodeNotInList(pFindNode);
			if(isNotIn == true)
				OpenListIn(pFindNode);
		}

		doCreate = JumpPoint.JumpRR(ParentX + 1, ParentY, pNowNode, &pFindNode, dwColor);
		if (doCreate == true) {
			bool isNotIn = isNodeNotInList(pFindNode);
			if (isNotIn == true)
				OpenListIn(pFindNode);
		}

		doCreate = JumpPoint.JumpUU(ParentX, ParentY - 1, pNowNode, &pFindNode, dwColor);
		if (doCreate == true) {
			bool isNotIn = isNodeNotInList(pFindNode);
			if (isNotIn == true)
				OpenListIn(pFindNode);
		}

		doCreate = JumpPoint.JumpDD(ParentX, ParentY + 1, pNowNode, &pFindNode, dwColor);
		if (doCreate == true) {
			bool isNotIn = isNodeNotInList(pFindNode);
			if (isNotIn == true)
				OpenListIn(pFindNode);
		}

		//JumpPoint.Jump(ParentX - 1, ParentY, DIR_LL, pNowNode, &pFindNode, dwColor);
		//if (pFindNode != NULL) {
		//	// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
		//	bool isNotIn = isNodeNotInList(pFindNode);
		//	if(isNotIn == true)
		//		OpenListIn(pFindNode);
		//}

		//pFindNode = NULL;
		//JumpPoint.Jump(ParentX + 1, ParentY, DIR_RR, pNowNode, &pFindNode, dwColor);
		//if (pFindNode != NULL) {
		//	// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
		//	bool isNotIn = isNodeNotInList(pFindNode);
		//	if (isNotIn == true)
		//		OpenListIn(pFindNode);
		//}

		//pFindNode = NULL;
		//JumpPoint.Jump(ParentX, ParentY - 1, DIR_UU, pNowNode, &pFindNode, dwColor);
		//if (pFindNode != NULL) {
		//	// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
		//	bool isNotIn = isNodeNotInList(pFindNode);
		//	if (isNotIn == true)
		//		OpenListIn(pFindNode);
		//}
		
		//pFindNode = NULL;
		//JumpPoint.Jump(ParentX, ParentY + 1, DIR_DD, pNowNode, &pFindNode, dwColor);
		//if (pFindNode != NULL) {
		//	// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
		//	bool isNotIn = isNodeNotInList(pFindNode);
		//	if (isNotIn == true)
		//		OpenListIn(pFindNode);
		//}

		//pFindNode = NULL;
		//JumpPoint.Jump(ParentX - 1, ParentY - 1, DIR_LU, pNowNode, &pFindNode, dwColor);
		//if (pFindNode != NULL) {
		//	// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
		//	bool isNotIn = isNodeNotInList(pFindNode);
		//	if (isNotIn == true)
		//		OpenListIn(pFindNode);
		//}

		//pFindNode = NULL;
		//JumpPoint.Jump(ParentX + 1, ParentY - 1, DIR_UR, pNowNode, &pFindNode, dwColor);
		//if (pFindNode != NULL) {
		//	// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
		//	bool isNotIn = isNodeNotInList(pFindNode);
		//	if (isNotIn == true)
		//		OpenListIn(pFindNode);
		//}

		//pFindNode = NULL;
		//JumpPoint.Jump(ParentX + 1, ParentY + 1, DIR_RD, pNowNode, &pFindNode, dwColor);
		//if (pFindNode != NULL) {
		//	// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
		//	bool isNotIn = isNodeNotInList(pFindNode);
		//	if (isNotIn == true)
		//		OpenListIn(pFindNode);
		//}

		//pFindNode = NULL;
		//JumpPoint.Jump(ParentX - 1, ParentY + 1, DIR_DL, pNowNode, &pFindNode, dwColor);
		//if (pFindNode != NULL) {
		//	// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
		//	bool isNotIn = isNodeNotInList(pFindNode);
		//	if (isNotIn == true)
		//		OpenListIn(pFindNode);
		//}

		return;
	}

	// 노드가 시작노드가 아닐 경우, 부모의 방향에 따라 검사
//	BYTE byDir = pNowNode->byParentDir;
//
//	switch (byDir) {
//
//	case DIR_LL:
//	{
//		// 예외 범위 확인
//		if (ParentX - 1 < 0)
//			break;
//
//		// 기본 Jump
//		pFindNode = NULL;
//		JumpPoint.Jump(ParentX - 1, ParentY, DIR_LL, pNowNode, &pFindNode, dwColor);
//		if (pFindNode != NULL) {
//			// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//			bool isNotIn = isNodeNotInList(pFindNode);
//			if (isNotIn == true)
//				OpenListIn(pFindNode);
//		}
//
//		// 옵션 Jump
//		// UU 막히고, LU 뚫렸으면, LU로 Jump
//		if(ParentY - 1 >= 0) {
//			bool UpWall = JumpPoint.GetIsWall(ParentX, ParentY - 1);
//			bool LeftUpWall = JumpPoint.GetIsWall(ParentX - 1, ParentY - 1);
//			if (UpWall == true && LeftUpWall == false) {
//				pFindNode = NULL;
//				JumpPoint.Jump(ParentX - 1, ParentY - 1, DIR_LU, pNowNode, &pFindNode, dwColor);
//				if (pFindNode != NULL) {
//					// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//					bool isNotIn = isNodeNotInList(pFindNode);
//					if (isNotIn == true)
//						OpenListIn(pFindNode);
//				}
//			}
//		}
//
//		// DD 막히고, DL 뚫렸으면, DL로 Jump
//		if (ParentY + 1 < FILD_Y) {
//			bool DownWall = JumpPoint.GetIsWall(ParentX, ParentY + 1);
//			bool LeftDownWall = JumpPoint.GetIsWall(ParentX - 1, ParentY + 1);
//			if (DownWall == true && LeftDownWall == false) {
//				pFindNode = NULL;
//				JumpPoint.Jump(ParentX - 1, ParentY + 1, DIR_DL, pNowNode, &pFindNode, dwColor);
//				if (pFindNode != NULL) {
//					// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//					bool isNotIn = isNodeNotInList(pFindNode);
//					if (isNotIn == true)
//						OpenListIn(pFindNode);
//				}
//			}
//		}
//	}
//	break;
//	case DIR_LU:
//	{
//		// 기본 Jump LU LL UU 방향
//		// LL
//		if (ParentX - 1 >= 0) {
//			pFindNode = NULL;
//			JumpPoint.Jump(ParentX - 1, ParentY, DIR_LL, pNowNode, &pFindNode, dwColor);
//			if (pFindNode != NULL) {
//				// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//				bool isNotIn = isNodeNotInList(pFindNode);
//				if (isNotIn == true)
//					OpenListIn(pFindNode);
//			}
//		}
//		// UU
//		if (ParentY - 1 >= 0) {
//			pFindNode = NULL;
//			JumpPoint.Jump(ParentX, ParentY - 1, DIR_UU, pNowNode, &pFindNode, dwColor);
//			if (pFindNode != NULL) {
//				// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//				bool isNotIn = isNodeNotInList(pFindNode);
//				if (isNotIn == true)
//					OpenListIn(pFindNode);
//			}
//		}
//		// LU
//		if (ParentX - 1 >= 0 && ParentY - 1 >= 0) {
//			pFindNode = NULL;
//			JumpPoint.Jump(ParentX - 1, ParentY - 1, DIR_LU, pNowNode, &pFindNode, dwColor);
//			if (pFindNode != NULL) {
//				// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//				bool isNotIn = isNodeNotInList(pFindNode);
//				if (isNotIn == true)
//					OpenListIn(pFindNode);
//			}
//		}
//		
//		// 옵션 Jump DL RU
//		// DD 막히고, DL 뚫렸으면, DL로 Jump
//		if (ParentY + 1 < FILD_Y && ParentX - 1 >= 0) {
//			bool UpWall = JumpPoint.GetIsWall(ParentX, ParentY + 1);
//			bool LeftUpWall = JumpPoint.GetIsWall(ParentX - 1, ParentY + 1);
//			if (UpWall == true && LeftUpWall == false) {
//				pFindNode = NULL;
//				JumpPoint.Jump(ParentX - 1, ParentY + 1, DIR_DL, pNowNode, &pFindNode, dwColor);
//				if (pFindNode != NULL) {
//					// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//					bool isNotIn = isNodeNotInList(pFindNode);
//					if (isNotIn == true)
//						OpenListIn(pFindNode);
//				}
//			}
//		}
//
//		// RR 막히고, UR 뚫렸으면, UR로 Jump
//		if (ParentX + 1 < FILD_X && ParentY - 1 >= 0) {
//			bool RightWall = JumpPoint.GetIsWall(ParentX + 1, ParentY);
//			bool RightUpWall = JumpPoint.GetIsWall(ParentX + 1, ParentY - 1);
//			if (RightWall == true && RightUpWall == false) {
//				pFindNode = NULL;
//				JumpPoint.Jump(ParentX + 1, ParentY - 1, DIR_UR, pNowNode, &pFindNode, dwColor);
//				if (pFindNode != NULL) {
//					// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//					bool isNotIn = isNodeNotInList(pFindNode);
//					if (isNotIn == true)
//						OpenListIn(pFindNode);
//				}
//			}
//		}
//	}
//	break;
//	case DIR_UU:
//	{
//		// 예외 범위 확인
//		if (ParentY - 1 < 0)
//			break;
//
//		// 기본 Jump UU 방향
//		pFindNode = NULL;
//		JumpPoint.Jump(ParentX, ParentY - 1, DIR_UU, pNowNode, &pFindNode, dwColor);
//		if (pFindNode != NULL) {
//			// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//			bool isNotIn = isNodeNotInList(pFindNode);
//			if (isNotIn == true)
//				OpenListIn(pFindNode);
//		}
//
//		// 옵션 Jump
//		// LL 막히고, LU 뚫렸으면, LU로 Jump
//		if (ParentX - 1 >= 0) {
//			bool LeftWall = JumpPoint.GetIsWall(ParentX - 1, ParentY);
//			bool LeftUpWall = JumpPoint.GetIsWall(ParentX - 1, ParentY - 1);
//			if (LeftWall == true && LeftUpWall == false) {
//				pFindNode = NULL;
//				JumpPoint.Jump(ParentX - 1, ParentY - 1, DIR_LU, pNowNode, &pFindNode, dwColor);
//				if (pFindNode != NULL) {
//					// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//					bool isNotIn = isNodeNotInList(pFindNode);
//					if (isNotIn == true)
//						OpenListIn(pFindNode);
//				}
//			}
//		}
//
//		// RR 막히고, UR 뚫렸으면, UR로 Jump
//		if (ParentX + 1 < FILD_X) {
//			bool RightWall = JumpPoint.GetIsWall(ParentX + 1, ParentY);
//			bool RightUpWall = JumpPoint.GetIsWall(ParentX + 1, ParentY - 1);
//			if (RightWall == true && RightUpWall == false) {
//				pFindNode = NULL;
//				JumpPoint.Jump(ParentX + 1, ParentY - 1, DIR_UR, pNowNode, &pFindNode, dwColor);
//				if (pFindNode != NULL) {
//					// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//					bool isNotIn = isNodeNotInList(pFindNode);
//					if (isNotIn == true)
//						OpenListIn(pFindNode);
//				}
//			}
//		}
//	}
//	break;
//	case DIR_UR:
//	{
//		// 기본 Jump UR UU RR 방향
//		// UU
//		if(ParentY - 1 >= 0) {
//			pFindNode = NULL;
//			JumpPoint.Jump(ParentX, ParentY - 1, DIR_UU, pNowNode, &pFindNode, dwColor);
//			if (pFindNode != NULL) {
//				// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//				bool isNotIn = isNodeNotInList(pFindNode);
//				if (isNotIn == true)
//					OpenListIn(pFindNode);
//			}
//		}
//		// RR
//		if (ParentX + 1 < FILD_X) {
//			pFindNode = NULL;
//			JumpPoint.Jump(ParentX + 1, ParentY, DIR_RR, pNowNode, &pFindNode, dwColor);
//			if (pFindNode != NULL) {
//				// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//				bool isNotIn = isNodeNotInList(pFindNode);
//				if (isNotIn == true)
//					OpenListIn(pFindNode);
//			}
//		}
//		// UR
//		if (ParentY - 1 >= 0 && ParentX + 1 < FILD_X) {
//			pFindNode = NULL;
//			JumpPoint.Jump(ParentX + 1, ParentY - 1, DIR_UR, pNowNode, &pFindNode, dwColor);
//			if (pFindNode != NULL) {
//				// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//				bool isNotIn = isNodeNotInList(pFindNode);
//				if (isNotIn == true)
//					OpenListIn(pFindNode);
//			}
//		}
//
//		// 옵션 Jump LU RD
//		// LL 막히고, LU 뚫렸으면, LU로 Jump
//		if (ParentX - 1 >= 0 && ParentY - 1 >= 0) {
//			bool LeftWall = JumpPoint.GetIsWall(ParentX - 1, ParentY);
//			bool LeftUpWall = JumpPoint.GetIsWall(ParentX - 1, ParentY - 1);
//			if (LeftWall == true && LeftUpWall == false) {
//				pFindNode = NULL;
//				JumpPoint.Jump(ParentX - 1, ParentY - 1, DIR_LU, pNowNode, &pFindNode, dwColor);
//				if (pFindNode != NULL) {
//					// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//					bool isNotIn = isNodeNotInList(pFindNode);
//					if (isNotIn == true)
//						OpenListIn(pFindNode);
//				}
//			}
//		}
//
//		// DD 막히고, RD 뚫렸으면, RD로 Jump
//		if (ParentY + 1 < FILD_Y && ParentX + 1 < FILD_X) {
//			bool RightWall = JumpPoint.GetIsWall(ParentX, ParentY + 1);
//			bool RightDownWall = JumpPoint.GetIsWall(ParentX + 1, ParentY + 1);
//			if (RightWall == true && RightDownWall == false) {
//				pFindNode = NULL;
//				JumpPoint.Jump(ParentX + 1, ParentY + 1, DIR_RD, pNowNode, &pFindNode, dwColor);
//				if (pFindNode != NULL) {
//					// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//					bool isNotIn = isNodeNotInList(pFindNode);
//					if (isNotIn == true)
//						OpenListIn(pFindNode);
//				}
//			}
//		}
//	}
//	break;
//	case DIR_RR:
//	{
//		// 예외 범위 확인
//		if (ParentX + 1 >= FILD_X)
//			break;
//
//		// 기본 Jump RR 방향
//		pFindNode = NULL;
//		JumpPoint.Jump(ParentX + 1, ParentY, DIR_RR, pNowNode, &pFindNode, dwColor);
//		if (pFindNode != NULL) {
//			// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//			bool isNotIn = isNodeNotInList(pFindNode);
//			if (isNotIn == true)
//				OpenListIn(pFindNode);
//		}
//
//		// 옵션 Jump
//		// UU 막히고, UR 뚫렸으면, UR로 Jump
//		if (ParentY - 1 >= 0) {
//			bool UpWall = JumpPoint.GetIsWall(ParentX, ParentY - 1);
//			bool RightUpWall = JumpPoint.GetIsWall(ParentX + 1, ParentY - 1);
//			if (UpWall == true && RightUpWall == false) {
//				pFindNode = NULL;
//				JumpPoint.Jump(ParentX + 1, ParentY - 1, DIR_UR, pNowNode, &pFindNode, dwColor);
//				if (pFindNode != NULL) {
//					// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//					bool isNotIn = isNodeNotInList(pFindNode);
//					if (isNotIn == true)
//						OpenListIn(pFindNode);
//				}
//			}
//		}
//
//		// DD 막히고, RD 뚫렸으면, RD로 Jump
//		if (ParentY + 1 < FILD_Y) {
//			bool RightWall = JumpPoint.GetIsWall(ParentX, ParentY + 1);
//			bool RightDownWall = JumpPoint.GetIsWall(ParentX + 1, ParentY + 1);
//			if (RightWall == true && RightDownWall == false) {
//				pFindNode = NULL;
//				JumpPoint.Jump(ParentX + 1, ParentY + 1, DIR_RD, pNowNode, &pFindNode, dwColor);
//				if (pFindNode != NULL) {
//					// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//					bool isNotIn = isNodeNotInList(pFindNode);
//					if (isNotIn == true)
//						OpenListIn(pFindNode);
//				}
//			}
//		}
//	}
//	break;
//	case DIR_RD:
//	{
//		// 기본 Jump RD DD RR 방향
//		// DD
//		if (ParentY + 1 < FILD_Y) {
//			pFindNode = NULL;
//			JumpPoint.Jump(ParentX, ParentY + 1, DIR_DD, pNowNode, &pFindNode, dwColor);
//			if (pFindNode != NULL) {
//				// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//				bool isNotIn = isNodeNotInList(pFindNode);
//				if (isNotIn == true)
//					OpenListIn(pFindNode);
//			}
//		}
//		// RR
//		if (ParentX + 1 < FILD_X) {
//			pFindNode = NULL;
//			JumpPoint.Jump(ParentX + 1, ParentY, DIR_RR, pNowNode, &pFindNode, dwColor);
//			if (pFindNode != NULL) {
//				// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//				bool isNotIn = isNodeNotInList(pFindNode);
//				if (isNotIn == true)
//					OpenListIn(pFindNode);
//			}
//		}
//		// RD
//		if (ParentX + 1 < FILD_X && ParentY + 1 < FILD_Y) {
//			pFindNode = NULL;
//			JumpPoint.Jump(ParentX + 1, ParentY + 1, DIR_RD, pNowNode, &pFindNode, dwColor);
//			if (pFindNode != NULL) {
//				// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//				bool isNotIn = isNodeNotInList(pFindNode);
//				if (isNotIn == true)
//					OpenListIn(pFindNode);
//			}
//		}
//
//		// 옵션 Jump DL UR
//		// LL 막히고, DL 뚫렸으면, DL로 Jump
//		if (ParentX - 1 >= 0) {
//			bool LeftWall = JumpPoint.GetIsWall(ParentX - 1, ParentY);
//			bool LeftDownWall = JumpPoint.GetIsWall(ParentX - 1, ParentY + 1);
//			if (LeftWall == true && LeftDownWall == false) {
//				pFindNode = NULL;
//				JumpPoint.Jump(ParentX - 1, ParentY + 1, DIR_DL, pNowNode, &pFindNode, dwColor);
//				if (pFindNode != NULL) {
//					// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//					bool isNotIn = isNodeNotInList(pFindNode);
//					if (isNotIn == true)
//						OpenListIn(pFindNode);
//				}
//			}
//		}
//
//		// UU 막히고, UR 뚫렸으면, UR로 Jump
//		if (ParentY - 1 >= 0) {
//			bool UpWall = JumpPoint.GetIsWall(ParentX, ParentY - 1);
//			bool RightUpWall = JumpPoint.GetIsWall(ParentX + 1, ParentY - 1);
//			if (UpWall == true && RightUpWall == false) {
//				pFindNode = NULL;
//				JumpPoint.Jump(ParentX + 1, ParentY - 1, DIR_UR, pNowNode, &pFindNode, dwColor);
//				if (pFindNode != NULL) {
//					// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//					bool isNotIn = isNodeNotInList(pFindNode);
//					if (isNotIn == true)
//						OpenListIn(pFindNode);
//				}
//			}
//		}
//	}
//	break;
//	case DIR_DD:
//	{
//		// 예외 범위 확인
//		if (ParentY + 1 >= FILD_Y)
//			break;
//
//		// 기본 Jump DD 방향
//		pFindNode = NULL;
//		JumpPoint.Jump(ParentX, ParentY + 1, DIR_DD, pNowNode, &pFindNode, dwColor);
//		if (pFindNode != NULL) {
//			// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//			bool isNotIn = isNodeNotInList(pFindNode);
//			if (isNotIn == true)
//				OpenListIn(pFindNode);
//		}
//
//		// 옵션 Jump
//		// LL 막히고, DL 뚫렸으면, DL로 Jump
//		if (ParentX - 1 >= 0 ) {
//			bool LeftWall = JumpPoint.GetIsWall(ParentX - 1, ParentY);
//			bool LeftDownWall = JumpPoint.GetIsWall(ParentX - 1, ParentY + 1);
//			if (LeftWall == true && LeftDownWall == false) {
//				pFindNode = NULL;
//				JumpPoint.Jump(ParentX - 1, ParentY + 1, DIR_DL, pNowNode, &pFindNode, dwColor);
//				if (pFindNode != NULL) {
//					// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//					bool isNotIn = isNodeNotInList(pFindNode);
//					if (isNotIn == true)
//						OpenListIn(pFindNode);
//				}
//			}
//		}
//
//		// RR 막히고, RD 뚫렸으면, RD로 Jump
//		if (ParentX + 1 < FILD_X) {
//			bool RightWall = JumpPoint.GetIsWall(ParentX + 1, ParentY);
//			bool RightDownWall = JumpPoint.GetIsWall(ParentX + 1, ParentY + 1);
//			if (RightWall == true && RightDownWall == false) {
//				pFindNode = NULL;
//				JumpPoint.Jump(ParentX + 1, ParentY + 1, DIR_RD, pNowNode, &pFindNode, dwColor);
//				if (pFindNode != NULL) {
//					// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//					bool isNotIn = isNodeNotInList(pFindNode);
//					if (isNotIn == true)
//						OpenListIn(pFindNode);
//				}
//			}
//		}
//	}
//	break;
//	case DIR_DL:
//	{
//		// 기본 Jump DL LL DD 방향
//		// LL
//		if (ParentX - 1 >= 0) {
//			pFindNode = NULL;
//			JumpPoint.Jump(ParentX - 1, ParentY, DIR_LL, pNowNode, &pFindNode, dwColor);
//			if (pFindNode != NULL) {
//				// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//				bool isNotIn = isNodeNotInList(pFindNode);
//				if (isNotIn == true)
//					OpenListIn(pFindNode);
//			}
//		}
//
//		// DD
//		if (ParentY + 1 < FILD_Y) {
//			pFindNode = NULL;
//			JumpPoint.Jump(ParentX, ParentY + 1, DIR_DD, pNowNode, &pFindNode, dwColor);
//			if (pFindNode != NULL) {
//				// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//				bool isNotIn = isNodeNotInList(pFindNode);
//				if (isNotIn == true)
//					OpenListIn(pFindNode);
//			}
//		}
//
//		// DL
//		if (ParentX - 1 >= 0 && ParentY + 1 < FILD_Y) {
//			pFindNode = NULL;
//			JumpPoint.Jump(ParentX - 1, ParentY + 1, DIR_DL, pNowNode, &pFindNode, dwColor);
//			if (pFindNode != NULL) {
//				// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//				bool isNotIn = isNodeNotInList(pFindNode);
//				if (isNotIn == true)
//					OpenListIn(pFindNode);
//			}
//		}
//
//		// 옵션 Jump
//		// RR 막히고, RD 뚫렸으면, RD로 Jump
//		if (ParentX + 1 < FILD_X && ParentY + 1 < FILD_Y) {
//			bool RightWall = JumpPoint.GetIsWall(ParentX + 1, ParentY);
//			bool RightDownWall = JumpPoint.GetIsWall(ParentX + 1, ParentY + 1);
//			if (RightWall == true && RightDownWall == false) {
//				pFindNode = NULL;
//				JumpPoint.Jump(ParentX + 1, ParentY + 1, DIR_RD, pNowNode, &pFindNode, dwColor);
//				if (pFindNode != NULL) {
//					// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//					bool isNotIn = isNodeNotInList(pFindNode);
//					if (isNotIn == true)
//						OpenListIn(pFindNode);
//				}
//			}
//		}
//
//		// UU 막히고, LU 뚫렸으면, LU로 Jump
//		if (ParentY - 1 >= 0 && ParentY - 1 >= 0) {
//			bool UpWall = JumpPoint.GetIsWall(ParentX, ParentY - 1);
//			bool LeftUpWall = JumpPoint.GetIsWall(ParentX - 1, ParentY - 1);
//			if (UpWall == true && LeftUpWall == false) {
//				pFindNode = NULL;
//				JumpPoint.Jump(ParentX - 1, ParentY - 1, DIR_LU, pNowNode, &pFindNode, dwColor);
//				if (pFindNode != NULL) {
//					// 새노드 존재하면, OpenList에 넣어줌(속성은 Jump에서 다 바뀌었음)
//					bool isNotIn = isNodeNotInList(pFindNode);
//					if (isNotIn == true)
//						OpenListIn(pFindNode);
//				}
//			}
//		}
//	}
//	break;
//	default:
//		break;
//	}
}

bool Compare(const NODE *pObject1, const NODE *pObject2) {

	return pObject1->F < pObject2->F;
}

void changeWindowSize() {

	//-------------------------
	// 윈도우 창 변경
	//-------------------------
	RECT WindowRect;
	WindowRect.top = 0;
	WindowRect.left = 0;
	WindowRect.right = 1500;
	WindowRect.bottom = 750;

	AdjustWindowRectEx(&WindowRect, GetWindowStyle(g_hWnd), GetMenu(g_hWnd) != NULL, GetWindowExStyle(g_hWnd));

	//int iX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (640 / 2);
	//int iY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (480 / 2);

	MoveWindow(g_hWnd, 10, 10, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, true);

}

//for (;;) {
//
//	NODE *pNowNode = JumpPoint.GetNodePtr(iX, iY);
//	NODE *pStartNode = JumpPoint.GetStartPtr();
//	NODE *pEndNode = JumpPoint.GetEndPtr();
//
//	/*pNowNode->byParentDir = ParentDir;
//	pNowNode->pParent = pParent;
//	pNowNode->G = JumpPoint.GetDistance(pNowNode->iX, pNowNode->iY, pStartNode->iX, pStartNode->iY);
//	pNowNode->H = JumpPoint.GetDistance(pNowNode->iX, pNowNode->iY, pEndNode->iX, pEndNode->iY);
//	pNowNode->F = pNowNode->G + pNowNode->H;
//	*/
//
//	// 코너가 맞다면 
//	bool isCorner = JumpPoint.isCorner(iX, iY, ParentDir);
//	if (isCorner == true)
//		return true;
//
//	// Jump 함수가 한계치 이상 호출됬다면
//	if (200 <= LimitCnt++)
//		return false;
//
//	// 벽을 만났다면
//	if (iX < 0 || iX >= FILD_X)
//		return false;
//
//	if (iY < 0 || iY >= FILD_Y)
//		return false;
//}
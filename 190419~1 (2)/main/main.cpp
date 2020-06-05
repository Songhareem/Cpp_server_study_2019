// main.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//
	//for (;;) {
	//	Node = openList(F가장 작은거);
	//	if (Node 가 목적지 ? ) {
	//		true : Node->parent = 경로;
	//		false : 길 못찾고 종료;
	//	}

	//	closeList Insert(Node);
	//	// 팔방 노드 생성 Node 는 부모노드
	//	노드 생성(Node, NodeX + 1, NodeY + 1);
	//	노드 생성(Node, NodeX + 1, NodeY + 1);
	//	노드 생성(Node, NodeX + 1, NodeY + 1);
	//	노드 생성(Node, NodeX + 1, NodeY + 1); 
	//	노드 생성(Node, NodeX + 1, NodeY + 1);
	//	노드 생성(Node, NodeX + 1, NodeY + 1);
	//	노드 생성(Node, NodeX + 1, NodeY + 1);
	//	노드 생성(Node, NodeX + 1, NodeY + 1);
	//}

//노드 생성(parent, x, y) {
//
//	x, y 범위예외처리(필드 벗어나면) -> return;
//	if (openList x, y 검색) {
//
//		if (Node->G > 부모 Node->G + 1) {
//
//			Node->parent = 부모 노드;
//			Node->G = 재계산;
//			Node->F = 재계산;
//		}
//	}
//	else if (closeList x, y 검색) {
//
//		if (Node->G > 부모 Node->G + 1) {
//
//			Node->parent = 부모 노드;
//			Node->G = 재계산;
//			Node->F = 재계산;
//		}
//	}
//	// open, close에 없으니 정말
//	노드 생성;
//}

// 더블클릭 사용을 위해 메인 윈도 설정에서 스타일에 CS_DBLCLKS 추가 해줘야함

#include "stdafx.h"
#include "main.h"
#include "CAstar.h"
#include <list>
#include <algorithm>

using namespace std;

CAstar Astar;
HBITMAP hBit;
int g_ColorFlag;
bool g_KeyIn = false;
bool g_KeyPushOnce = false;
bool g_LineFlag = false;
NODE * g_pLastNode = NULL;
list<NODE *> OpenList;
list<NODE *> CloseList;

void MakeFild();
void OnTimer();
bool PathFinder(int strX, int strY, int endX, int endY);
void CreateNode(int ParentX, int ParentY);	// 노드 생성(PathFinder에서 사용)
bool Compare(const NODE *pObject1, const NODE *pObject2); // F 정렬시 사용
void changeWindowSize();

#define MAX_LOADSTRING 100

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



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
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

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   g_hWnd = hWnd;

   if (!hWnd)
   {
      return FALSE;
   }

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
	case WM_RBUTTONDOWN:
	{
		if(g_KeyIn != true)
			Astar.MoveEndNode(LOWORD(lParam), HIWORD(lParam));
	}
	break;
	case WM_LBUTTONDBLCLK:
	{
		if (g_KeyIn != true) {
			Astar.MoveStartNode(LOWORD(lParam), HIWORD(lParam));
			NODE *pStartNode = Astar.GetStartPtr();
			OpenList.clear();
			OpenList.push_back(pStartNode);
			OpenList.push_back(pStartNode);
		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		if (g_KeyIn == true)
			break;

		int X = LOWORD(lParam);
		int Y = HIWORD(lParam);
		int Color = 0;
		Astar.GetNodePos(&X, &Y);

		Color = Astar.GetColor(X, Y);

		// 마우스 눌리는 순간만 플래그 한 번 세워져야함
		if (Color == WHITE) {
			g_ColorFlag = WHITE;
		}
		else if (Color == GRAY) {
			g_ColorFlag = GRAY;
		}
	}
	break;
	case WM_LBUTTONUP:
	{
		g_ColorFlag = NULL;
	}
	break;
	case WM_MOUSEMOVE :
	{
		if (g_ColorFlag == WHITE) {

			Astar.WallUp(LOWORD(lParam), HIWORD(lParam));
		}

		else if (g_ColorFlag == GRAY) {

			Astar.WallDown(LOWORD(lParam), HIWORD(lParam));
		}
	}
	break;
	case WM_KEYDOWN :
	{
		g_KeyIn = true;

		if (g_KeyPushOnce == true) {
			Astar.NodeInit();
			OpenList.clear();
			CloseList.clear();
			g_KeyPushOnce = false;
		}
		else {
			if (GetKeyState(VK_SPACE) & 0x8000) {
				NODE *pStartNode = Astar.GetStartPtr();
				NODE *pEndNode = Astar.GetEndPtr();
			
				PathFinder(pStartNode->iX, pStartNode->iY, pEndNode->iX, pEndNode->iY);
				g_KeyPushOnce = true;
			}
		}
		g_KeyIn = false;
	}
	break;
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
	case WM_CREATE:
	{
		OpenList.push_back(Astar.GetStartPtr());
		OpenList.push_back(Astar.GetStartPtr());
		SetTimer(hWnd, 1, 50, NULL);
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
    case WM_DESTROY:
        PostQuitMessage(0);
		KillTimer(hWnd, 1);
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
	HBRUSH OldBrush;
	HPEN hPen, OldPen;

	GetClientRect(g_hWnd, &Rect);
	hdc = GetDC(g_hWnd);
	if (hBit == NULL) {
		hBit = CreateCompatibleBitmap(hdc, Rect.right, Rect.bottom);
	}
	hMemDC = CreateCompatibleDC(hdc);
	OldBit = (HBITMAP)SelectObject(hMemDC, hBit);
	FillRect(hMemDC, &Rect, GetSysColorBrush(COLOR_WINDOW));

	if (g_LineFlag == true) {

		for (;;) {
			if (g_pLastNode == NULL)
				break;

			g_pLastNode->iColor = BLACK;
			g_pLastNode = g_pLastNode->pParent;
		}
	}

	for (int Y = 0; Y < NODE_SIZE::FILD_Y; Y++) {
		for (int X = 0; X < NODE_SIZE::FILD_X; X++) {
			
			int left = 0;
			int right = 0;
			int top = 0;
			int bottom = 0;

			Astar.GetNodeRect(X, Y, left, right, top, bottom);
			
			// 노드 색 설정
			int iColor = Astar.GetColor(X, Y);
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
				break;
			}

			// 붓 교체 후 노드 색칠
			OldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
			Rectangle(hMemDC, left, top, right, bottom);

			// 색칠 후 되돌리기
			DeleteObject(SelectObject(hMemDC, OldBrush));
		}
	}

	//if (g_LineFlag == true) {

	//	// 출발 도착 노드 색 재설정
	//	NODE *pStartNode = Astar.GetStartPtr();
	//	NODE *pEndNode = Astar.GetEndPtr();
	//	pStartNode->iColor = GREEN;
	//	pEndNode->iColor = RED;

	//	hPen = CreatePen(PS_INSIDEFRAME, 5, RGB(255, 0, 0));
	//	OldPen = (HPEN)SelectObject(hMemDC, hPen);
	//	
	//	// 그리기
	//	for (;;) {

	//		if (g_pLastNode->pParent == NULL)
	//			break;
	//		
	//		int strX;
	//		int strY;
	//		int endX;
	//		int endY;

	//		NODE *pParentNode = g_pLastNode->pParent;
	//		
	//		if(g_pLastNode == Astar.GetEndPtr()) {
	//			
	//			strX = g_pLastNode->iLeftX + FILD_SIZE / 2;
	//			strY = g_pLastNode->iTopY + FILD_SIZE / 2;
	//			endX = pParentNode->iLeftX + FILD_SIZE / 2;
	//			endY = pParentNode->iTopY + FILD_SIZE / 2;

	//			MoveToEx(hMemDC, strX, strY, NULL);
	//			LineTo(hMemDC, endX, endY);
	//		}

	//		g_pLastNode = pParentNode;
	//	}

	//	DeleteObject(SelectObject(hMemDC, OldPen));
	//}

	SelectObject(hMemDC, OldBit);
	DeleteDC(hMemDC);
	ReleaseDC(g_hWnd, hdc);
}

void OnTimer() {

	MakeFild();

	InvalidateRect(g_hWnd, NULL, FALSE);
}

bool PathFinder(int strX, int strY, int endX, int endY) {

	for(;;) {

		// OpenList에서 맨위에것 빼기
		NODE *pGetNode = OpenList.front();
		NODE *pEndNode = Astar.GetEndPtr();
		OpenList.pop_front();
		if(OpenList.empty() == true)
			return false;

		// CloseList로 옮김
		pGetNode->iColor = YELLOW;
		CloseList.push_back(pGetNode);

		// 노드 생성(openList에 넣기)
		CreateNode(pGetNode->iX, pGetNode->iY);

		if (pEndNode->iX == pGetNode->iX && pEndNode->iY == pGetNode->iY) {

			g_pLastNode = pGetNode;
			g_LineFlag = true;
			break;
		}
	}

	return true;
}

void CreateNode(int ParentX, int ParentY) {

	int NodeX[8] = { ParentX - 1, ParentX , ParentX + 1,
					ParentX - 1, ParentX + 1,
					ParentX - 1, ParentX , ParentX + 1 };

	int NodeY[8] = { ParentY - 1, ParentY - 1 , ParentY - 1,
					ParentY, ParentY,
					ParentY + 1, ParentY + 1, ParentY + 1 };

	int Index = 0;
	NODE *pParentNode = Astar.GetNodePtr(ParentX, ParentY);
	NODE *pEndNode = Astar.GetEndPtr();
	NODE *pStartNode = Astar.GetStartPtr();

	for (int i = 0; i < 8; i++) {

		int X = NodeX[i];
		int Y = NodeY[i];

		NODE *pNowNode = Astar.GetNodePtr(X, Y);

		// 0<X<FILD_X 아니라면 패스
		if (X < 0 || X >= FILD_X)
			continue;

		// 0<Y<FILD_Y 아니라면 패스
		if (Y < 0 || Y >= FILD_Y)
			continue;

		// 벽이라면 패스
		if (pNowNode->isWall == true)
			continue;
		
		// 이미 오픈 리스트에 값이 있다면 패스
		list<NODE *>::iterator iter = find(OpenList.begin(), OpenList.end(), pNowNode);
		if (iter != OpenList.end())
			continue;

		// 이미 클로즈 리스트에 값이 있다면 패스
		iter = find(CloseList.begin(), CloseList.end(), pNowNode);
		if (iter != CloseList.end())
			continue;

		// G,H,F 값 넣기
		pNowNode->G = 1 + Astar.GetDistance(pParentNode->iX, pParentNode->iY, pStartNode->iX, pStartNode->iY);
		pNowNode->H = Astar.GetDistance(pNowNode->iX, pNowNode->iY, pEndNode->iX, pEndNode->iY);
		pNowNode->F = pNowNode->G + pNowNode->H;

		// 부모를 포인터 가르키기
		pNowNode->pParent = pParentNode;

		// 색변경
		pNowNode->iColor = BLUE;

		// 노드 배열에 넣기
		OpenList.push_back(pNowNode);
	}

	// F정렬(오름차순)
	OpenList.sort(Compare);
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
	WindowRect.right = 1300;
	WindowRect.bottom = 700;

	AdjustWindowRectEx(&WindowRect, GetWindowStyle(g_hWnd), GetMenu(g_hWnd) != NULL, GetWindowExStyle(g_hWnd));

	//int iX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (640 / 2);
	//int iY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (480 / 2);

	MoveWindow(g_hWnd, 10, 10, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, true);

}
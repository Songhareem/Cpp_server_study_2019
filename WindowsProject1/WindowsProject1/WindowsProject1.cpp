// WindowsProject1.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "WindowsProject1.h"

 g_Array[100];

//#define MAX_LOADSTRING 100

// 전역 변수:
//HINSTANCE hInst;                                // 현재 인스턴스입니다.
//WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
//WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
//ATOM                MyRegisterClass(HINSTANCE hInstance);
//BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM); // 메인 윈도우 용
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);   // 다이알로그 용

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    //UNREFERENCED_PARAMETER(hPrevInstance);
    //UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    //LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    //LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    //MyRegisterClass(hInstance);
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
	wcex.lpszClassName = L"ABCD"; //szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

    /*/ 응용 프로그램 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }
	//*/
	HWND hWnd = CreateWindowW(L"ABCD", L"WindowTest", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);


    //HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        //if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        //{
            TranslateMessage(&msg);
            DispatchMessage(&msg); // 메시지 해석 후, 윈도우 프로시저 호출
        //}
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
/*/
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
	wcex.lpszClassName = L"ABCD"; //szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
//*/
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

/*/
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(L"ABCD",L"WindowTest", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}
//*/

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
	/*/
	typedef struct tagMSG {
		HWND        hwnd;
		UINT        message;
		WPARAM      wParam;		// 주소값이 들어갈수 있는 크기의 변수(포인터형 아님) 키보드 - 버추얼 키코드					// 마우스 -
		LPARAM      lParam;		// 주소값이 들어갈수 있는 크기의 변수(포인터형 아님) 키보드 - 각각 비트에 따라 용도가 다름  // 마우스 - LOW : PosX HIGH : PosY
		DWORD       time;
		POINT       pt;
	}
	//*/
    switch (message)
    {
		// 스타일 창 등 알고 싶을때, 도구 -> spy++ -> 창찾기 -> 다른 윈도우 프로그램을 확인 가능
		// 메시지 등 알고 싶을때, 도구 -> spy++ -> 로그 -> 다른 윈도우 프로그램을 확인 가능
		// 윈도우 메시지에는 우선순위가 있음 ( 우선순위 큐? )
	case WM_LBUTTONDOWN:
	{
		// 좌표를 기억만 그림은 페인트에서

		// -1 로 채움
		memset(g_Array, 0xff, sizeof(g_Array));
		
		int PosX = GET_X_LPARAM(lParam);
		int PosY = GET_Y_LPARAM(lParam);

		for (int i = 0; i < 100; i++) {

			if (g_Array[i].x == -1) {

				g_Array[i].x = PosX;
				g_Array[i].y = PosY;
			}
		}

		// 강제적으로 페인트 발생 (TRUE 전체갱신 + 깜빡임, FALSE 새로운거만 갱신, 기존거 없음)  
		InvalidateRect(hWnd, NULL, FALSE);
		//MoveToEx();
		//LineTo();
		//*/
	}
		break;
    case WM_COMMAND:
        {
			// 블록을 잡은 이유는 case 내부에 지역변수를 만들기 위해
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT: // 메뉴창의 도움말의 속성값, 단축키 
                //DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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
			// Begin, End Paint 갱신이 안되므로 윈도우는 계속 WM_PAINT 메시지 발생
			// 갱신해야할 타이밍에 랜더링 다시 요청
            PAINTSTRUCT ps; // rcPaint = 갱신해야할 부분 및 좌표 저장, PAINTSTRUCT는 사실상 사용안함
            HDC hdc = BeginPaint(hWnd, &ps); // hdc = 클라이언트 공간 ( DC는 중간 매개체 -> 어느 장치랑 연결될것 이냐만 결정해줌 프린트기, 모니터 등 )
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);

			// dc를 사용하는 법 ( 가능한한 짧게 쓸 것 ), 꼭 WM_PAINT에서만 쓰는건 아님 
			// dc 권한 획득 hdc = GetDC(hWnd);
			//dc 사용
			// dc 권한 상실 ReleaseDC(hWnd, hdc);
        }
        break;
	case WM_CREATE:
		// 윈도우에서 사용할 초기화 작업
		// 컨트롤 위치, 핸들 조정, 윈도우 생성시 변수 초기화 등
		// 주의점 : DispatchMessage가 아닌, CreateWindow에서 호출시키는 메시지!
		break;
	case WM_DESTROY: // 필수
		// 윈도우가 닫힐때(창만 사라지고 어플리케이션이 꺼지는게 아님) 실행되는 작업
        PostQuitMessage(0); // 프로세스 종료 유도 (GetMSG return false 발생시켜서 프로세스 종료 유도)
        break;
    
	default:
		// 수많은 메시지를 다 대응되게 만들 수 없기때문에 필요
        return DefWindowProc(hWnd, message, wParam, lParam); // 필수, 처리못한 메시지 자동처리  
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

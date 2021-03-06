// Client.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
// 384p 참고
// AsyncSelect 단점
// 1) 검색이 들어감 (사용자 많아지면 성능 떨어짐)
// 2) 싱글스레드로 많이 사용 (멀티스레드는 힘듦)
// socket / accept 만 INVALID_SOCKET 에러, 나머지는 SOCKET_ERROR
// WSAAsyncSelect 사용시 include 전에 #define _WINSOCK_DEPRECATED_NO_WARNINGS 선언해줘야함
// 윈도우 종료 WM_CLOSE / WM_DESTROY / WM_QUIT 차이 : http://ssb777.blogspot.com/2009/07/mfc-wmclose-wmdestroy-wmquit.html

#include "stdafx.h"
#include "Client.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CLIENT);
	wcex.lpszClassName = L"NetDrawClient";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	   
	RegisterClassExW(&wcex);
	  
	// CreateWindow / CreateWindowW 호출시 WM_CREATE 메시지 호출
	HWND hWnd = CreateWindowW(L"NetDrawClient", L"NetDraw_Client", WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	// main Window DC 및 handle 값 저장
	g_mainHDC = GetDC(hWnd);
	g_hWnd = hWnd;

	if (!hWnd)
	{
		return FALSE;
	}

	// ---------------------------
	// 초기화 (창 사이즈 조절 등)
	// ---------------------------
	Init();

	// ---------------------------
	// 네트워크 연결 준비 및 연결
	// ---------------------------
	int retval = 0;
	
	// 윈속 시작
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	
	// sock()
	g_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_sock == INVALID_SOCKET) {
		int ErrCode = WSAGetLastError();
		if (ErrCode == WSAEWOULDBLOCK)
			CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"socket() : WSAEWOULDBLOCK"));
		else {
			CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"socket()", ErrCode));
			return 1;
		}
	}

	// WSAAsyncSelect 적용
	WSAAsyncSelect(g_sock, g_hWnd, UM_NETWORK,
		FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE );

	// connect()
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	InetPton(AF_INET, SERVERIP, &serverAddr.sin_addr);
	serverAddr.sin_port = htons(SERVERPORT);
	retval = connect(g_sock, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR) {
		int ErrCode = WSAGetLastError();
		if (ErrCode == WSAEWOULDBLOCK)
			CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"connect() : WSAEWOULDBLOCK"));
		else {
			CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"connect()", ErrCode));
		}
	}
	
	// ---------------------------
	// 연결 후, 윈도우 창 생성
	// ---------------------------
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	   
	// 기본 메시지 루프입니다:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// closeSocket()
	closesocket(g_sock);

	// 윈속 종료
	WSACleanup();

	return (int)msg.wParam;
}

void ChangeWindowSize() {

	//-------------------------
	// 윈도우 창 변경
	//-------------------------
	RECT WindowRect;
	WindowRect.top = 0;
	WindowRect.left = 0;
	WindowRect.right = 640;
	WindowRect.bottom = 480;

	AdjustWindowRectEx(&WindowRect, GetWindowStyle(g_hWnd), GetMenu(g_hWnd) != NULL, GetWindowExStyle(g_hWnd));

	int iX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (640 / 2);
	int iY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (480 / 2);

	MoveWindow(g_hWnd, iX, iY, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, true);
}

void Init() {

	// ---------------------------
	// 윈도우 사이즈 변경 
	// ---------------------------
	ChangeWindowSize();

	//CLogMaker::GetInstance()->Rename((char *)"Log_1_");

	setlocale(LC_ALL, "");
}
void Disconnet() {
	
	//Disconnet(세션) {
	// 세션이 입력으로 들어오므로 한번 더 전체를 찾게 되지만 그게 맞음
	//	SOCKET_ERR ? 소켓 API 호출시 일어날 수 있는 에러
	//		소켓 API 호출하는 함수 ? send(), recv()

	//		closesocket(세션->sock); // 멀티스레드에서는 좀 더 신경써서 에러처리 해야함 (리소스만 반환하고 sock 내에 값이 남으므로)
	//	Pack.Type = 2;
	//	Pack.ID = 세션->ID;
	//	SessionList에서 세션삭제

	//		send_Broadcast(NULL, &Pack);
	//}
}

bool ProcRecv(SOCKET sock) {

	// ------------------------
	// 큐 상태 확인
	// ------------------------
	if (g_RecvQ.GetFreeSize() >= 0) {
		CLogMaker::GetInstance()->WriteLog((char *)"RecvQ.Enqeue() NotEnough");
		return 1;
	}

	// ------------------------
	// 데이터 넣기
	// ------------------------
	char *pRear = g_RecvQ.GetRearBufferPtr();
	int FreeSize = g_RecvQ.GetFreeSize();
	int NotBrokenPutSize = g_RecvQ.GetNotBrokenPutSize();
	int retval;

	if (FreeSize > NotBrokenPutSize) {

		retval = recv(sock, pRear, NotBrokenPutSize, 0);
		if (retval == SOCKET_ERROR) {
			int ErrCode = WSAGetLastError();
			if (ErrCode == WSAEWOULDBLOCK)
				CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"recv() : WSAEWOULDBLOCK"));
			else {
				CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"recv()", ErrCode));
			}
			return 0;
		}
		g_RecvQ.MoveRear(NotBrokenPutSize);

		pRear = g_RecvQ.GetRearBufferPtr();
		retval = recv(sock, pRear, FreeSize - NotBrokenPutSize, 0);
		if (retval == SOCKET_ERROR) {
			int ErrCode = WSAGetLastError();
			if (ErrCode == WSAEWOULDBLOCK)
				CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"recv() : WSAEWOULDBLOCK"));
			else {
				CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"recv()", ErrCode));
			}
			return 0;
		}
		g_RecvQ.MoveRear(FreeSize - NotBrokenPutSize);
	}

	else if (FreeSize == NotBrokenPutSize) {

		retval = recv(sock, pRear, NotBrokenPutSize, 0);
		if (retval == SOCKET_ERROR) {
			int ErrCode = WSAGetLastError();
			if (ErrCode == WSAEWOULDBLOCK)
				CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"recv2() : WSAEWOULDBLOCK"));
			else {
				CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"recv2()", ErrCode));
			}
			return 0;
		}
		g_RecvQ.MoveRear(NotBrokenPutSize);
	}

	else {
		CLogMaker::GetInstance()->WriteLog((char *)"is it posible? recv3()");
	}
	
	// ------------------------------
	// 패킷 뽑아서 리스트 만들기
	// ------------------------------
	for (;;) {
		// recvQ 안에 헤더 사이즈(2) 만큼도 안남았다면 반복 종료
		if (g_RecvQ.GetUseSize() < 2)
			break;
		
		HEADER Header;
		retval = g_RecvQ.Peek((char *)&Header, sizeof(Header));
		if (retval || Header.Len != 16) {
			CLogMaker::GetInstance()->WriteLog((char *)"HeaderErr()");
			break;
		}

		// recvQ안에 PACKET LEN 만큼의 길이가 없다면 반복 종료
		if (g_RecvQ.GetUseSize() < sizeof(PACKET) + sizeof(HEADER))
			break;

		// 헤더크기 만큼 FRONT옮기고
		g_RecvQ.MoveFront(sizeof(HEADER));
		// PACKET값 Dequeue
		retval = g_RecvQ.Dequeue((char *)&g_RecvPack, sizeof(PACKET));
		if (retval) {
			CLogMaker::GetInstance()->WriteLog((char *)"DequeueErr()");
			break;
		}

		CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"Header Recv", Header.Len));
		CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"pPack->StartX", g_RecvPack.iStartX));
		CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"pPack->StartY", g_RecvPack.iStartY));
		CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"pPack->EndX", g_RecvPack.iEndX));
		CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"pPack->EndY", g_RecvPack.iEndY));

		PackList.push_back(g_RecvPack);
	}
	
	// WM_PAINT 호출
	InvalidateRect(g_hWnd, NULL, FALSE);
	return 0;
}
bool ProcRecvOld(SOCKET sock) {

	char buf[1000];

	int retval = recv(sock, buf, sizeof(buf), 0);
	if (retval == SOCKET_ERROR) {
		int ErrCode = WSAGetLastError();
		if (ErrCode == WSAEWOULDBLOCK)
			CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"recv() : WSAEWOULDBLOCK"));
		else {
			CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"recv()", ErrCode));
		}
		return 0;
	}

	bool isRecv = g_RecvQ.Enqueue(buf, retval);
	if (isRecv) {
		// recvQ가 꽉 찼다는건 로직 어딘가 문제 있다는것, 프로그램 종료
		CLogMaker::GetInstance()->WriteLog((char *)"RecvQ.Enqeue()");
		return 1;
	}

	for (;;) {
		// recvQ 안에 헤더 사이즈(2) 만큼도 안남았다면 반복 종료
		if (g_RecvQ.GetUseSize() < 2)
			break;

		HEADER Header;
		retval = g_RecvQ.Peek((char *)&Header, sizeof(Header));
		if (retval || Header.Len != 16) {
			CLogMaker::GetInstance()->WriteLog((char *)"HeaderErr()");
			break;
		}

		// recvQ안에 PACKET LEN 만큼의 길이가 없다면 반복 종료
		if (g_RecvQ.GetUseSize() < sizeof(PACKET) + sizeof(HEADER))
			break;

		// 헤더크기 만큼 FRONT옮기고
		g_RecvQ.MoveFront(sizeof(HEADER));
		// PACKET값 Dequeue
		retval = g_RecvQ.Dequeue((char *)&g_RecvPack, sizeof(PACKET));
		if (retval) {
			CLogMaker::GetInstance()->WriteLog((char *)"DequeueErr()");
			break;
		}

		CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"Header Recv", Header.Len));
		CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"pPack->StartX", g_RecvPack.iStartX));
		CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"pPack->StartY", g_RecvPack.iStartY));
		CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"pPack->EndX", g_RecvPack.iEndX));
		CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"pPack->EndY", g_RecvPack.iEndY));

		PackList.push_back(g_RecvPack);
	}

	// WM_PAINT 호출
	InvalidateRect(g_hWnd, NULL, FALSE);
	return 0;
}

void ProcSend(SOCKET sock) {

	g_SendFlag = true;

	if (g_SendQ.GetFreeSize() < 18) {
		CLogMaker::GetInstance()->WriteLog((char *)"SendQ() : NOT_ENOUGH");
		return;
	}

	char buf[18];
	for (;;) {

		int retval = g_SendQ.Peek(buf, 18);
		if (retval)
			break;

		retval = send(g_sock, buf, 18, 0);
		if (retval == SOCKET_ERROR) {
			int ErrCode = WSAGetLastError();
			if (ErrCode == WSAEWOULDBLOCK)
				CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"send() : WSAEWOULDBLOCK"));
			else {
				CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"send()", ErrCode));
			}
		}

		g_SendQ.MoveFront(retval);
	}
}
void ProcSendOld(SOCKET sock) {

	g_SendFlag = true;

	if (g_SendQ.GetFreeSize() < 18) {
		CLogMaker::GetInstance()->WriteLog((char *)"SendQ() : NOT_ENOUGH");
		return;
	}

	char buf[18];
	for (;;) {

		int retval = g_SendQ.Peek(buf, 18);
		if (retval)
			break;

		retval = send(g_sock, buf, 18, 0);
		if (retval == SOCKET_ERROR) {
			int ErrCode = WSAGetLastError();
			if (ErrCode == WSAEWOULDBLOCK)
				CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"send() : WSAEWOULDBLOCK"));
			else {
				CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"send()", ErrCode));
			}
		}

		g_SendQ.MoveFront(retval);
	}
}

void SendPack(PACKET *pPack) {

	HEADER Header;
	Header.Len = 16;
	bool isSend = g_SendQ.Enqueue((char *)&Header, sizeof(HEADER));
	if (isSend)
		CLogMaker::GetInstance()->WriteLog((char *)"isSend_Header_ERROR()");
	isSend = g_SendQ.Enqueue((char *)pPack, sizeof(PACKET));
	if (isSend)
		CLogMaker::GetInstance()->WriteLog((char *)"isSend_Packet_ERROR()");

	if (g_SendFlag == false)
		return;

	char buf[18];
	for (;;) {

		int retval = g_SendQ.Peek(buf, 18);
		if (retval)
			break;

		// Send Test(Log)
		HEADER *pHeader = (HEADER *)&buf[0];
		PACKET *pPack = (PACKET *)&buf[2];
		CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"Header", pHeader->Len));
		CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"pPack->StartX", pPack->iStartX));
		CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"pPack->StartY", pPack->iStartY));
		CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"pPack->EndX", pPack->iEndX));
		CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"pPack->EndY", pPack->iEndY));

		retval = send(g_sock, buf, 18, 0);
		if (retval == SOCKET_ERROR) {
			int ErrCode = WSAGetLastError();
			if(ErrCode == WSAEWOULDBLOCK)
				CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"send() : WSAEWOULDBLOCK"));
			else {
				CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"send()", ErrCode));
			}
		}

		g_SendQ.MoveFront(retval);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	//case WM_LBUTTONDOWN :
	//{
	//	static int x = 0;
	//	static int y = 0;
	//	g_RecvPack.iStartX = x;
	//	g_RecvPack.iStartY = y;
	//	x += 10;
	//	y += 10;
	//	g_RecvPack.iEndX = x;
	//	g_RecvPack.iEndY = y;
	//	
	//	// WM_PAINT 호출
	//	InvalidateRect(g_hWnd, NULL, FALSE);
	//}
	//break;
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

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		
		std::list<PACKET>::iterator iter = PackList.begin();
		
		for(;iter != PackList.end(); ++iter) {
			MoveToEx(hdc, (*iter).iStartX, (*iter).iStartY, NULL);
			LineTo(hdc, (*iter).iEndX, (*iter).iEndY);
		}
		PackList.clear();

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		g_ClickFlag = true;

		g_SendPack.iStartX = GET_X_LPARAM(lParam);
		g_SendPack.iStartY = GET_Y_LPARAM(lParam);
	}
	break;
	case WM_LBUTTONUP:
	{
		g_ClickFlag = false;

		ZeroMemory(&g_SendPack, sizeof(g_SendPack));
	}
	break;
	case WM_MOUSEMOVE:
	{
		if (g_ClickFlag != true)
			break;

		g_SendPack.iEndX = GET_X_LPARAM(lParam);
		g_SendPack.iEndY = GET_Y_LPARAM(lParam);

		//CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"StartX", g_SendPack.iStartX));
		//CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"StartY", g_SendPack.iStartY));
		//CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"EndX", g_SendPack.iEndX));
		//CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"EndY", g_SendPack.iEndY));

		SendPack(&g_SendPack);

		g_SendPack.iStartX = g_SendPack.iEndX;
		g_SendPack.iStartY = g_SendPack.iEndY;
	}
	break;
	case UM_NETWORK:
	{
		//true : ERROR
		if (WSAGETSELECTERROR(lParam)) {
			CLogMaker::GetInstance()->WriteLog((char *)"UM_NETWORK : WSAGetSelectError");
			break;
		}
		switch (WSAGETSELECTEVENT(lParam)) {
		case FD_CONNECT:
		{
		}
		break;
		case FD_READ:
		{
			// wParam = 이벤트 발생한 소켓
			ProcRecvOld(wParam);//if(ProcRecv(wParam))
			//PostMessage(hWnd, WM_DESTROY, NULL, NULL);
		}
		break;
		case FD_WRITE:
		{
			// wParam = 이벤트 발생한 소켓
			ProcSendOld(wParam);
		}
		break;
		case FD_CLOSE:
		{
			CLogMaker::GetInstance()->WriteLog((char *)"FD_CLOSE");
			PostMessage(hWnd, WM_DESTROY, NULL, NULL);
		}
		break;
		}
	}
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
	case WM_INITDIALOG :
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


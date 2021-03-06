// Sprite.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Sprite.h"

#define SERVERIP	L"127.0.0.1"
#define SERVERPORT	5000

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	//test
	/*HEADER head;
	head.byCode = 0x5a;
	head.bySize = 2;
	head.byType = 0x02;
	head.byTemp = 0x00;
	CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"sizeof(HEADER)", sizeof(HEADER)));
	CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"sizeof(WORD)", sizeof(WORD)));
	CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"sizeof(DWORD)", sizeof(DWORD)));
	CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"sizeof(BYTE)", sizeof(BYTE)));
	return 1;*/

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SPRITE));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL; // MAKEINTRESOURCEW(IDC_SPRITE); -> 메뉴바
	wcex.lpszClassName = L"Sprite";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

	HWND hWnd = CreateWindowW(L"Sprite", L"Sprite", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	g_mainHDC = GetDC(hWnd);
	g_hWnd = hWnd;

	if (!hWnd)
	{
		return FALSE;
	}
	
	// ------------------------
	// NetWork 연결
	// ------------------------

	int retval;

	//윈속 시작
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		CLogMaker::GetInstance()->WriteLog((char *)"WSAStartup()");
		return 1;
	}

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

	// WSAAsyncSelect() 적용
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

	// 윈도우 창 크기 변경 및 보이기
	changeWindowSize();

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// 게임자료 초기화
	GameInit();

	MSG msg;

	// 기본 메시지 루프입니다:
	while (1) {
	
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		else
		{
			Update();
		}
	}

	// closesock()
	closesocket(g_sock);

	// 윈속 종료
	WSACleanup();

	return (int)msg.wParam;
}

//------------------------------
// Recv 관련 함수
//------------------------------
bool SC_CreateMyCharacter(char *pPack) {

	stPACKET_CREATE_CHARACTER *pPacket = (stPACKET_CREATE_CHARACTER *)pPack;

	int ID = pPacket->ID;
	int Dir = pPacket->Direction;
	int X = pPacket->X;
	int Y = pPacket->Y;
	int HP = pPacket->HP;

	CPlayerObject *pPlayer = new CPlayerObject(ID, static_cast<int>(e_OBJECT_TYPE::eTYPE_PLAYER), true, Dir, HP);
	pPlayer->ActionInput(dfACTION_STAND);
	pPlayer->SetPosition(X, Y);
	pPlayer->SetActionStand();
	g_pPlayerObject = pPlayer;
	g_ObjectList.push_back(g_pPlayerObject);

	return true;
}

bool SC_CreateOtherCharacter(char *pPack) {

	stPACKET_CREATE_CHARACTER *pPacket = (stPACKET_CREATE_CHARACTER *)pPack;

	int ID = pPacket->ID;
	int Dir = pPacket->Direction;
	int X = pPacket->X;
	int Y = pPacket->Y;
	int HP = pPacket->HP;

	CBaseObject *pObject;
	CPlayerObject *pPlayer = new CPlayerObject(ID, static_cast<int>(e_OBJECT_TYPE::eTYPE_PLAYER), false, Dir, HP);
	pPlayer->ActionInput(dfACTION_STAND);
	pPlayer->SetPosition(X, Y);
	pPlayer->SetActionStand();
	pObject = pPlayer;
	g_ObjectList.push_back(pObject);

	return true;
}

bool SC_DeleteCharacter(char *pPack) {

	stPACKET_DELETE_CHARACTER *pPacket = (stPACKET_DELETE_CHARACTER *)pPack;

	int ID = pPacket->ID;

	list<CBaseObject *>::iterator iter;

	for (iter = g_ObjectList.begin(); iter != g_ObjectList.end(); ++iter) {

		if ((*iter)->GetObjectID() == ID) {

			g_ObjectList.erase(iter);
			break;
		}
	}

	return true;
}

bool SC_MoveStart(char *pPack) {

	stPACKET_SC_MOVE *pPacket = (stPACKET_SC_MOVE *)pPack;

	int ID = pPacket->ID;
	int Action = pPacket->Direction; // 8방
	int X = pPacket->X;
	int Y = pPacket->Y;

	int Dir;
	switch (Action) {

	case dfPACKET_MOVE_DIR_LL:
	case dfPACKET_MOVE_DIR_LU:
	case dfPACKET_MOVE_DIR_LD:
		Dir = LEFT;
		break;
	case dfPACKET_MOVE_DIR_RR:
	case dfPACKET_MOVE_DIR_RU:
	case dfPACKET_MOVE_DIR_RD:
		Dir = RIGHT;
		break;
	default:
		Dir = -1;
	}

	list<CBaseObject *>::iterator iter;

	for (iter = g_ObjectList.begin(); iter != g_ObjectList.end(); ++iter) {

		if ((*iter)->GetObjectID() == ID) {

			CPlayerObject *pPlayer = (CPlayerObject *)(*iter);
			pPlayer->ActionInput(Action + 1);
			if (Dir != -1)
				pPlayer->SetDirection(Dir);
			pPlayer->SetDirection(Dir);
			pPlayer->SetPosition(X, Y);
			pPlayer->SetActionMove();

			break;
		}
	}

	return true;
}

bool SC_MoveStop(char *pPack) {

	stPACKET_SC_MOVE *pPacket = (stPACKET_SC_MOVE *)pPack;

	int ID = pPacket->ID;
	int Dir = pPacket->Direction;
	int X = pPacket->X;
	int Y = pPacket->Y;

	list<CBaseObject *>::iterator iter;

	for (iter = g_ObjectList.begin(); iter != g_ObjectList.end(); ++iter) {

		if ((*iter)->GetObjectID() == ID) {

			CPlayerObject *pPlayer = (CPlayerObject *)(*iter);
			pPlayer->ActionInput(dfACTION_STAND);
			pPlayer->SetDirection(Dir);
			pPlayer->SetPosition(X, Y);
			pPlayer->SetActionStand();

			break;
		}
	}

	return true;
}

bool SC_Attack1(char *pPack) {

	stPACKET_SC_ATTACK *pPacket = (stPACKET_SC_ATTACK *)pPack;

	int ID = pPacket->ID;
	int Dir = pPacket->Direction;
	int X = pPacket->X;
	int Y = pPacket->Y;

	list<CBaseObject *>::iterator iter;

	for (iter = g_ObjectList.begin(); iter != g_ObjectList.end(); ++iter) {

		if ((*iter)->GetObjectID() == ID) {

			CPlayerObject *pPlayer = (CPlayerObject *)(*iter);
			pPlayer->ActionInput(dfACTION_ATTACK1);
			pPlayer->SetDirection(Dir);
			pPlayer->SetPosition(X, Y);
			pPlayer->SetActionAttack1();

			break;
		}
	}

	return true;
}

bool SC_Attack2(char *pPack) {

	stPACKET_SC_ATTACK *pPacket = (stPACKET_SC_ATTACK *)pPack;

	int ID = pPacket->ID;
	int Dir = pPacket->Direction;
	int X = pPacket->X;
	int Y = pPacket->Y;

	list<CBaseObject *>::iterator iter;

	for (iter = g_ObjectList.begin(); iter != g_ObjectList.end(); ++iter) {

		if ((*iter)->GetObjectID() == ID) {

			CPlayerObject *pPlayer = (CPlayerObject *)(*iter);
			pPlayer->ActionInput(dfACTION_ATTACK2);
			pPlayer->SetDirection(Dir);
			pPlayer->SetPosition(X, Y);
			pPlayer->SetActionAttack2();

			break;
		}
	}

	return true;
}

bool SC_Attack3(char *pPack) {

	stPACKET_SC_ATTACK *pPacket = (stPACKET_SC_ATTACK *)pPack;

	int ID = pPacket->ID;
	int Dir = pPacket->Direction;
	int X = pPacket->X;
	int Y = pPacket->Y;

	list<CBaseObject *>::iterator iter;

	for (iter = g_ObjectList.begin(); iter != g_ObjectList.end(); ++iter) {

		if ((*iter)->GetObjectID() == ID) {

			CPlayerObject *pPlayer = (CPlayerObject *)(*iter);
			pPlayer->ActionInput(dfACTION_ATTACK3);
			pPlayer->SetDirection(Dir);
			pPlayer->SetPosition(X, Y);
			pPlayer->SetActionAttack3();

			break;
		}
	}

	return true;
}

bool SC_Damage(char *pPack) {

	stPACKET_DAMAGE *pPacket = (stPACKET_DAMAGE *)pPack;

	int  AtkID = pPacket->AttackID;	// 이펙트를 주기 위한 ID, 일단 제외
	int  DmgID = pPacket->DamageID;
	BYTE DmgHP = pPacket->DamageHP;

	list<CBaseObject *>::iterator iter;

	for (iter = g_ObjectList.begin(); iter != g_ObjectList.end(); ++iter) {

		if ((*iter)->GetObjectID() == DmgID) {

			CPlayerObject *pDmgPlayer = (CPlayerObject *)(*iter);
			pDmgPlayer->SetHP(DmgHP);

			break;
		}
	}

	return true;
}

void PacketProc(BYTE HeaderType, char *pPack) {

	switch (HeaderType) {

	case dfPACKET_SC_CREATE_MY_CHARACTER:
		SC_CreateMyCharacter(pPack);
		break;
	case dfPACKET_SC_CREATE_OTHER_CHARACTER:
		SC_CreateOtherCharacter(pPack);
		break;
	case dfPACKET_SC_DELETE_CHARACTER:
		SC_DeleteCharacter(pPack);
		break;
	case dfPACKET_SC_MOVE_START:
		SC_MoveStart(pPack);
		break;
	case dfPACKET_SC_MOVE_STOP:
		SC_MoveStop(pPack);
		break;
	case dfPACKET_SC_ATTACK1:
		SC_Attack1(pPack);
		break;
	case dfPACKET_SC_ATTACK2:
		SC_Attack2(pPack);
		break;
	case dfPACKET_SC_ATTACK3:
		SC_Attack3(pPack);
		break;
	case dfPACKET_SC_DAMAGE:
		SC_Damage(pPack);
		break;
	default:
		CLogMaker::GetInstance()->WriteLog((char *)"default Header Type");
		break;
	}
}

bool RecvEvent(SOCKET sock) {

	char buf[1000] = { 0, };

	// recv 호출 및 리턴값에 따른 종료 및 에러처리
	int retval = recv(sock, buf, sizeof(buf), 0);
	if (retval == SOCKET_ERROR) {
		int ErrCode = WSAGetLastError();
		if (ErrCode == WSAEWOULDBLOCK)
			CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"recv() : WSAEWOULDBLOCK"));
		else {
			CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"recv()", ErrCode));
		}
		return false;
	}

	// recvQ에 넣고 에러처리
	bool isRecv = g_RecvQ.Enqueue(buf, retval);
	if (isRecv == false) {
		CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"recv() : isRecv() FAIL"));
		// DISCONNECT()
	}

	// 완료패킷 처리부
	HEADER Head;
	char *Pack;
	int PackSize = 0;

	while (1) {

		// RecvQ에 최소한의 사이즈가 있는가? (헤더사이즈 초과)
		int QSize = g_RecvQ.GetUseSize();
		if (QSize < sizeof(HEADER)) {
			CLogMaker::GetInstance()->WriteLog((char *)"Header Size Error");
			break;
		}

		// RecvQ에서 헤더 Peek 및 헤더 코드 확인
		g_RecvQ.Peek((char *)&Head, sizeof(HEADER));
		if (Head.byCode != dfNETWORK_PACKET_CODE) {
			CLogMaker::GetInstance()->WriteLog((char *)"Header Code Error");
			g_RecvQ.MoveFront(1);
			continue;
		}

		// 헤더의 Len값(endCode 제외 사이즈) 과 RecvQ의 데이터 사이즈 비교
		PackSize = Head.bySize + 1;
		if (g_RecvQ.GetUseSize() < PackSize) {
			// 문제 여지 있음 clear
			CLogMaker::GetInstance()->WriteLog((char *)"Garbage Packet");
			g_RecvQ.ClearBuffer();
			break;
		}

		// 데이터 Peek했던 헤더 지우고, PACKET 뽑아내고, EndCode 확인
		g_RecvQ.MoveFront(sizeof(HEADER));
		Pack = new char[PackSize];
		g_RecvQ.Peek(Pack, PackSize);
		//g_RecvQ.Peek((char *)&PackView, PackSize);								// for Debug
		if (Pack[PackSize - 1] != dfNETWORK_PACKET_END) {
			CLogMaker::GetInstance()->WriteLog((char *)"Header End Error");
			g_RecvQ.MoveFront(PackSize);
			continue;
		}

		g_RecvQ.MoveFront(PackSize);

		int save = 0;

		// 헤더 type에 따른 패킷처리
		PacketProc(Head.byType, Pack);

		delete[] Pack;

	}

	return true;
}

//-----------------------------------
// Send 관련 함수
//-----------------------------------
void CS_MoveStart(HEADER *pHead, stPACKET_CS_MOVE *pPack, BYTE byDir, WORD wX, WORD wY) {

	pHead->byCode = dfNETWORK_PACKET_CODE;
	pHead->bySize = sizeof(stPACKET_CS_MOVE);
	pHead->byType = dfPACKET_CS_MOVE_START;

	pPack->Direction = byDir;
	pPack->X = wX;
	pPack->Y = wY;
}

void CS_MoveStop(HEADER *pHead, stPACKET_CS_MOVE *pPack, BYTE byDir, WORD wX, WORD wY) {

	pHead->byCode = dfNETWORK_PACKET_CODE;
	pHead->bySize = sizeof(stPACKET_CS_MOVE);
	pHead->byType = dfPACKET_CS_MOVE_STOP;

	pPack->Direction = byDir;
	pPack->X = wX;
	pPack->Y = wY;
}

void CS_Attack1(HEADER *pHead, stPACKET_CS_ATTACK *pPack, BYTE byDir, WORD wX, WORD wY) {

	pHead->byCode = dfNETWORK_PACKET_CODE;
	pHead->bySize = sizeof(stPACKET_CS_ATTACK);
	pHead->byType = dfPACKET_CS_ATTACK1;

	pPack->Direction = byDir;
	pPack->X = wX;
	pPack->Y = wY;
}

void CS_Attack2(HEADER *pHead, stPACKET_CS_ATTACK *pPack, BYTE byDir, WORD wX, WORD wY) {

	pHead->byCode = dfNETWORK_PACKET_CODE;
	pHead->bySize = sizeof(stPACKET_CS_ATTACK);
	pHead->byType = dfPACKET_CS_ATTACK2;

	pPack->Direction = byDir;
	pPack->X = wX;
	pPack->Y = wY;
}

void CS_Attack3(HEADER *pHead, stPACKET_CS_ATTACK *pPack, BYTE byDir, WORD wX, WORD wY) {

	pHead->byCode = dfNETWORK_PACKET_CODE;
	pHead->bySize = sizeof(stPACKET_CS_ATTACK);
	pHead->byType = dfPACKET_CS_ATTACK3;

	pPack->Direction = byDir;
	pPack->X = wX;
	pPack->Y = wY;
}

bool SendEvent(SOCKET socket) {

	// SendQ에 남은 데이터 없을때까지 or Send 실패까지 계속 보냄
	// 보낼수 없는 상태 될때까지 다 보내야함

	// 보내기용 임시버퍼
	char buf[1000];

	// Send 가능여부 확인
	if (g_netSendPossible == false)
		return false;

	// SendQ에 보낼 데이터 있는지 확인
	if (g_SendQ.GetUseSize() < sizeof(HEADER)) {
		g_SendQ.ClearBuffer();
		return 1;
	}

	while (1) {

		int UseSize = g_SendQ.GetUseSize();

		if (UseSize < sizeof(HEADER)) {
			g_SendQ.ClearBuffer();
			break;
		}

		if (UseSize < sizeof(buf)) {

			g_SendQ.Peek(buf, UseSize);
			int retval = send(socket, buf, UseSize, 0);

			if (retval == SOCKET_ERROR) {
				int ErrCode = WSAGetLastError();
				if (ErrCode == WSAEWOULDBLOCK) {
					CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"send() : WSAEWOULDBLOCK"));
					g_netSendPossible = false;
				}
				else {
					CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"send()", ErrCode));
				}
			}

			else if (retval == UseSize) {
				g_SendQ.MoveFront(UseSize);
			}
		}
		else {

			g_SendQ.Peek(buf, sizeof(buf));
			int retval = send(socket, buf, sizeof(buf), 0);

			if (retval == SOCKET_ERROR) {
				int ErrCode = WSAGetLastError();
				if (ErrCode == WSAEWOULDBLOCK) {
					CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"send() : WSAEWOULDBLOCK"));
					g_netSendPossible = false;
				}
				else {
					CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"send()", ErrCode));
				}
			}

			else if (retval == sizeof(buf)) {
				g_SendQ.MoveFront(sizeof(buf));
			}
		}
	}

	return true;
}

bool SendPacket(HEADER *pHead, char *pPack, int iPackSize) {

	// 접속 상태 예외처리
	if (g_netConnect == false) {
		CLogMaker::GetInstance()->WriteLog((char *)"ProcSend : Connect False Yet");
		return false;
	}

	// 에러나면 DISCONNECT?
	bool isFull = false;

	// SendQ에 헤더넣기
	isFull = g_SendQ.Enqueue((char *)pHead, sizeof(HEADER));
	if (isFull == false) {
		CLogMaker::GetInstance()->WriteLog((char *)"SendQ is Full, Header");
		return false;
	}
	// SendQ에 패킷넣기
	isFull = g_SendQ.Enqueue((char *)pPack, iPackSize);
	if (isFull == false) {
		CLogMaker::GetInstance()->WriteLog((char *)"SendQ is Full, Packet");
		return false;
	}

	// SendQ에 EndCode 넣기
	char EndCode = dfNETWORK_PACKET_END;
	isFull = g_SendQ.Enqueue(&EndCode, sizeof(EndCode));
	if (isFull == false) {
		CLogMaker::GetInstance()->WriteLog((char *)"SendQ is Full, EndCode");
		return false;
	}

	bool isSend = SendEvent(g_sock);
	if (isSend == false)
		return false;

	return true;
}

// 에러 및 종료 return 
bool netNetWorkProc(WPARAM wParam, LPARAM lParam) {

	//true : ERROR
	if (WSAGETSELECTERROR(lParam)) {
		CLogMaker::GetInstance()->WriteLog((char *)"UM_NETWORK : WSAGetSelectError()");
		return false;
	}

	switch (WSAGETSELECTEVENT(lParam)) {
	case FD_CONNECT:
	{
		CLogMaker::GetInstance()->WriteLog((char *)"FD_CONNECT");
		g_netConnect = true;
	}
	break;
	case FD_CLOSE:
	{
		CLogMaker::GetInstance()->WriteLog((char *)"FD_CLOSE");
		return false;
	}
	break;
	case FD_READ:
	{
		// wParam = 이벤트 발생한 소켓
		if(!RecvEvent(wParam))
			CLogMaker::GetInstance()->WriteLog((char *)"FD_READ ERROR");
	}
	break;
	case FD_WRITE:
	{
		g_netSendPossible = true;
		// wParam = 이벤트 발생한 소켓
		if (!SendEvent(wParam))
			CLogMaker::GetInstance()->WriteLog((char *)"FD_WRITE ERROR");

	}
	break;
	}

	return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HIMC hIMC;

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
    case WM_PAINT:
    {
            PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        EndPaint(hWnd, &ps);
    }
    break;
	case WM_LBUTTONDOWN:
	{
		//Run3();
	}
	break;
	case WM_ACTIVATEAPP:
	{
		g_IsGameActivate = (BOOL)wParam;
	}
	break;
	case WM_IME_NOTIFY:
	{
		if (hIMC == NULL) {

			hIMC = ImmGetContext(hWnd);

			ImmSetConversionStatus(hIMC, IME_CMODE_ALPHANUMERIC, IME_SMODE_NONE);
			ImmReleaseContext(hWnd, hIMC);
			hIMC = NULL;
		}
	}
	break;
	case WM_CREATE:
	{
		timeBeginPeriod(1);
	}
	break;
	case WM_DESTROY:
	{
		timeEndPeriod(1);
		PostQuitMessage(0);
	}
    break;
	case UM_NETWORK:
	{
		if (!netNetWorkProc(wParam, lParam)) {

			MessageBox(hWnd, L"접속이 종료되었습니다", L"끊김", MB_OK);
			SendMessage(g_hWnd, WM_DESTROY, NULL, NULL);
		}
	}
	break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 게임 초기화 함수
bool GameInit() {

	//Map
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::eMAP), "Sprite_Data\\_Map.bmp", 0, 0);
	
	//Stand_L
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_STAND_L_01), "Sprite_Data\\Stand_L_01.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_STAND_L_02), "Sprite_Data\\Stand_L_02.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_STAND_L_03), "Sprite_Data\\Stand_L_03.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_STAND_L_MAX), "Sprite_Data\\Stand_L_02.bmp", 71, 90);
	
	//Stand_R
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_STAND_R_01), "Sprite_Data\\Stand_R_01.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_STAND_R_02), "Sprite_Data\\Stand_R_02.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_STAND_R_03), "Sprite_Data\\Stand_R_03.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_STAND_R_MAX), "Sprite_Data\\Stand_R_02.bmp", 71, 90);

	//Move_L
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_L_01), "Sprite_Data\\Move_L_01.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_L_02), "Sprite_Data\\Move_L_02.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_L_03), "Sprite_Data\\Move_L_03.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_L_04), "Sprite_Data\\Move_L_04.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_L_05), "Sprite_Data\\Move_L_05.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_L_06), "Sprite_Data\\Move_L_06.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_L_07), "Sprite_Data\\Move_L_07.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_L_08), "Sprite_Data\\Move_L_08.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_L_09), "Sprite_Data\\Move_L_09.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_L_10), "Sprite_Data\\Move_L_10.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_L_11), "Sprite_Data\\Move_L_11.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_L_MAX), "Sprite_Data\\Move_L_12.bmp", 71, 90);
	
	//Move_R
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_R_01), "Sprite_Data\\Move_R_01.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_R_02), "Sprite_Data\\Move_R_02.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_R_03), "Sprite_Data\\Move_R_03.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_R_04), "Sprite_Data\\Move_R_04.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_R_05), "Sprite_Data\\Move_R_05.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_R_06), "Sprite_Data\\Move_R_06.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_R_07), "Sprite_Data\\Move_R_07.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_R_08), "Sprite_Data\\Move_R_08.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_R_09), "Sprite_Data\\Move_R_09.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_R_10), "Sprite_Data\\Move_R_10.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_R_11), "Sprite_Data\\Move_R_11.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_R_MAX), "Sprite_Data\\Move_R_12.bmp", 71, 90);

	//Attack1_L
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK1_L_01), "Sprite_Data\\Attack1_L_01.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK1_L_02), "Sprite_Data\\Attack1_L_02.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK1_L_03), "Sprite_Data\\Attack1_L_03.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK1_L_MAX), "Sprite_Data\\Attack1_L_04.bmp", 71, 90);
	
	//Attack1_R
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK1_R_01), "Sprite_Data\\Attack1_R_01.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK1_R_02), "Sprite_Data\\Attack1_R_02.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK1_R_03), "Sprite_Data\\Attack1_R_03.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK1_R_MAX), "Sprite_Data\\Attack1_R_04.bmp", 71, 90);
	
	//Attack2_L
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK2_L_01), "Sprite_Data\\Attack2_L_01.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK2_L_02), "Sprite_Data\\Attack2_L_02.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK2_L_03), "Sprite_Data\\Attack2_L_03.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK2_L_MAX), "Sprite_Data\\Attack2_L_04.bmp", 71, 90);

	//Attack2_R
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK2_R_01), "Sprite_Data\\Attack2_R_01.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK2_R_02), "Sprite_Data\\Attack2_R_02.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK2_R_03), "Sprite_Data\\Attack2_R_03.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK2_R_MAX), "Sprite_Data\\Attack2_R_04.bmp", 71, 90);

	//Attack3_L
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK3_L_01), "Sprite_Data\\Attack3_L_01.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK3_L_02), "Sprite_Data\\Attack3_L_02.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK3_L_03), "Sprite_Data\\Attack3_L_03.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK3_L_04), "Sprite_Data\\Attack3_L_04.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK3_L_MAX), "Sprite_Data\\Attack3_L_05.bmp", 71, 90);

	//Attack3_R
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK3_R_01), "Sprite_Data\\Attack3_R_01.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK3_R_02), "Sprite_Data\\Attack3_R_02.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK3_R_03), "Sprite_Data\\Attack3_R_03.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK3_R_04), "Sprite_Data\\Attack3_R_04.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK3_R_MAX), "Sprite_Data\\Attack3_R_05.bmp", 71, 90);
	
	//Effect
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::eEFFECT_SPARK_01), "Sprite_Data\\xSpark_1.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::eEFFECT_SPARK_02), "Sprite_Data\\xSpark_2.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::eEFFECT_SPARK_03), "Sprite_Data\\xSpark_3.bmp", 71, 90);
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::eEFFECT_SPARK_MAX), "Sprite_Data\\xSpark_4.bmp", 71, 90);

	//Shadow
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::eSHADOW), "Sprite_Data\\Shadow.bmp", 32, 4);

	//HPGuage
	CSpriteDib::GetInstance()->LoadDibSprite(static_cast<int>(e_SPRITE::eGUAGE_HP), "Sprite_Data\\HPGuage.bmp", 0, 0);

	//CLogMaker::GetInstance()->Rename((char *)"Log_2_");

	return 0;
}

// 로직 함수
void Update() {

	switch (g_GameState) {

	case TILTE:
		//Update_Title();
		break;
	case GAME:
		Update_Game();
		break;
	}
}

// Title로직 함수
void Update_Title() {}

// Game로직 함수
void Update_Game() {

	// Key Process
	if (g_IsGameActivate) {
		
		KeyProcess();
	}
	
	//Action
	Action();
	
	// 프레임 스킵여부
	if (FrameSkip()) {

		//Draw
		Draw();

		// Flip()
		ScreenDib::GetInstance()->Flip(g_hWnd);
	}
}

BOOL FrameSkip() {

	int result = true;
	
	static DWORD oldTime = timeGetTime();
	static DWORD remainTime = NULL;
	static DWORD dwCntSec = timeGetTime();
	static int iCountLogic = 0;
	static int iCountRender = 0;

	DWORD Time = timeGetTime() - oldTime;

	// FRAME ++ 
	iCountLogic++;

	// 넘어간 시간이 20을 넘었다면
	if (remainTime >= dfSLEEP_TIME) {

		remainTime = remainTime - dfSLEEP_TIME;

		result = false;
	}

	//시간이 20 이하
	else if (Time < dfSLEEP_TIME) {

		Sleep(dfSLEEP_TIME - Time);

		// RENDER ++
		iCountRender++;
		result = true;
	}

	// 시간이 20 이상
	else if (Time >= dfSLEEP_TIME) {

		remainTime += Time - dfSLEEP_TIME;

		// RENDER ++
		iCountRender++;
		result = true;
	}

	// 1Sec마다 FPS 표기
	if (timeGetTime() - dwCntSec >= dfONESEC) {

		int newCountLogic = iCountLogic * dfONESEC / ( timeGetTime() - dwCntSec );
		int newCountRender = iCountRender * dfONESEC / (timeGetTime() - dwCntSec);
		//PrintFrame(iCountLogic, iCountRender);
		PrintFrame(newCountLogic, newCountRender);
		iCountLogic = iCountRender = 0;
		dwCntSec = timeGetTime();
	}

	oldTime = timeGetTime();
	return result;
}

void PrintFrame(int dwCounterLogic, int dwCounterRender) {

	char str[70] = { ' ', };

	sprintf_s(str, "2D_GAME_PROJECT / FRAME : %02d / RENDER : %02d \0", dwCounterLogic, dwCounterRender);
	
	SetWindowTextA(g_hWnd, str);
}

// 키 입력 함수
void KeyProcess()
{
	//int OldAction = g_pPlayerObject->GetAction();

	/*if (OldAction == dfACTION_ATTACK1 || OldAction == dfACTION_ATTACK2 || OldAction == dfACTION_ATTACK3)
		return;
*/
	/*switch (OldAction) {
	case 0:
		CLogMaker::GetInstance()->WriteLog((char *)"OldAction Stand");
		break;
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
		CLogMaker::GetInstance()->WriteLog((char *)"OldAction Move");
		break;
	case 9:
		CLogMaker::GetInstance()->WriteLog((char *)"OldAction Attack1");
		break;
	case 10:
		CLogMaker::GetInstance()->WriteLog((char *)"OldAction Attack2");
		break;
	case 11:
		CLogMaker::GetInstance()->WriteLog((char *)"OldAction Attack3");
		break;
	}*/

	if (g_pPlayerObject == NULL)
		return;

	int Action = static_cast<int>(dfACTION_STAND);

	if (GetKeyState(VK_LEFT) & 0x8000)
		Action = static_cast<int>(dfACTION_MOVE_LL);

	if (GetKeyState(VK_UP) & 0x8000)
		Action = static_cast<int>(dfACTION_MOVE_UU);
	
	if (GetKeyState(VK_RIGHT) & 0x8000)
		Action = static_cast<int>(dfACTION_MOVE_RR);

	if (GetKeyState(VK_DOWN) & 0x8000)
		Action = static_cast<int>(dfACTION_MOVE_DD);

	if ((GetKeyState(VK_LEFT) & 0x8000)&& (GetKeyState(VK_UP) & 0x8000))
		Action = static_cast<int>(dfACTION_MOVE_LU);

	if ((GetKeyState(VK_UP) & 0x8000) && (GetKeyState(VK_RIGHT) & 0x8000))
		Action = static_cast<int>(dfACTION_MOVE_RU);

	if ((GetKeyState(VK_RIGHT) & 0x8000) && (GetKeyState(VK_DOWN) & 0x8000))
		Action = static_cast<int>(dfACTION_MOVE_RD);

	if ((GetKeyState(VK_DOWN) & 0x8000) && (GetKeyState(VK_LEFT) & 0x8000))
		Action = static_cast<int>(dfACTION_MOVE_LD);

	if (GetKeyState(0x5A) & 0x8000) // Z
		Action = static_cast<int>(dfACTION_ATTACK1);

	if (GetKeyState(0x58) & 0x8000) // X
		Action = static_cast<int>(dfACTION_ATTACK2);

	if (GetKeyState(0x43) & 0x8000) // C
		Action = static_cast<int>(dfACTION_ATTACK3);

	/*switch (Action) {
	case 0:
		CLogMaker::GetInstance()->WriteLog((char *)"Action Stand");
		break;
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
		CLogMaker::GetInstance()->WriteLog((char *)"Action Move");
		break;
	case 9:
		CLogMaker::GetInstance()->WriteLog((char *)"Action Attack1");
		break;
	case 10:
		CLogMaker::GetInstance()->WriteLog((char *)"Action Attack2");
		break;
	case 11:
		CLogMaker::GetInstance()->WriteLog((char *)"Action Attack3");
		break;
	}*/

	g_pPlayerObject->ActionInput(Action);
}

bool Compare(CBaseObject *pObject1, CBaseObject *pObject2) {

	return pObject1->GetCurY() < pObject2->GetCurY();
}

void Action() {

	list<CBaseObject *>::iterator iter = g_ObjectList.begin();

	// Run 호출
	for (; iter != g_ObjectList.end(); ++iter) {

		(*iter)->Run();
	}
	
	// Y축 정렬
	g_ObjectList.sort(Compare);

	// effect는 뒤로

}

void Draw() {

	BYTE *byDest = ScreenDib::GetInstance()->GetDibBuffer();
	int iDestWidth = ScreenDib::GetInstance()->GetWidth();
	int iDestHeight = ScreenDib::GetInstance()->getHeight();
	int iDestPitch = ScreenDib::GetInstance()->GetPitch();
	
	// map 출력
	CSpriteDib::GetInstance()->DrawImage(static_cast<int>(e_SPRITE::eMAP), 0, 0, byDest, iDestWidth, iDestHeight, iDestPitch);

	list<CBaseObject *>::iterator iter = g_ObjectList.begin();
	
	// Render 호출
	for (; iter != g_ObjectList.end(); ++iter) {
		
		//int iterID = (*iter)->GetObjectID();
		(*iter)->Render(byDest, iDestWidth, iDestHeight, iDestPitch);
	}
}

// Window 사이즈 변경
void changeWindowSize() {

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

//void Run1() {
//
//	BYTE *Dest = ScreenDib::GetInstance()->GetDibBuffer();
//	int iDestWidth = ScreenDib::GetInstance()->GetWidth();
//	int iDestHeight = ScreenDib::GetInstance()->getHeight();
//	int iDestPitch = ScreenDib::GetInstance()->GetPitch();
//
//	BYTE byGrayColor = 0;
//	for (int i = 0; i < 480; i++) {
//
//		memset(Dest, byGrayColor, 640 * 4);
//		Dest += iDestPitch;
//		byGrayColor++;
//	}
//
//	Dest = ScreenDib::GetInstance()->GetDibBuffer();
//	Dest += (iDestWidth / 2) * 4;
//	int byRedColor = 0x0000ff00;
//	for (int i = 0; i < 480; i++) {
//
//		*Dest = 0x00;
//		*(Dest + 1) = 0x00;
//		*(Dest + 2) = 0xff;
//
//		Dest += iDestPitch;
//	}
//	ScreenDib::GetInstance()->Flip(g_hWnd);
//}
//
//void Run2() {
//
//	// read Bitmap
//	FILE *fp = NULL;
//	BITMAPFILEHEADER bmfh;
//	DWORD dwFileSize, dwDibSize;
//	BYTE * pDib = NULL;
//
//	fopen_s(&fp, "Sprite_Data\\_Map.bmp", "rb");
//	fseek(fp, 0, SEEK_END);
//	dwFileSize = (DWORD)ftell(fp);
//	dwDibSize = dwFileSize - sizeof(BITMAPFILEHEADER);
//	pDib = new BYTE[dwDibSize];
//
//	fseek(fp, 0, SEEK_SET);
//	fread(&bmfh, sizeof(BITMAPFILEHEADER), 1, fp);
//	fread(pDib, dwDibSize, 1, fp);
//
//	fclose(fp);
//
//	// print Bitmap
//	BITMAPINFOHEADER * lpbmih;
//	BYTE* lpvBits;
//	int w, h, c;
//
//	lpbmih = (BITMAPINFOHEADER*)pDib;  //pDib는 BITMAPINFOHEADER의 시작주소를 가르키고 있음
//	w = lpbmih->biWidth;  //가로크기 저장
//	h = lpbmih->biHeight;  //세로크기 저장
//	c = lpbmih->biBitCount;  //비트수 저장
//
//	//비트맵 정보 시작 위치를 계산한다.
//
//	if (c == 24)  //색상테이블이 존재하지 않음
//		lpvBits = (BYTE*)pDib + sizeof(BITMAPINFOHEADER);  //픽셀 더이터의 시작주소를 저장
//
//
//	else  //색상테이블이 존재
//		lpvBits = (BYTE*)pDib + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*(1 << c);
//
//	SetDIBitsToDevice(g_mainHDC, 0, 0, w, h, 0, 0, 0, h, lpvBits, (BITMAPINFO*)pDib, DIB_RGB_COLORS);
//	// 비트맵을 화면에 출력하는 함수 
//	//디바이스 컨텍스트의 핸들 , 마우스를 클릭한 x 좌표, y좌표 , ......
//
//	//동적 할당한 메모리 해제
//	delete[] pDib;
//}
//
//void Run3() {
//
//	FILE *fp = NULL;
//	fopen_s(&fp, "test.txt", "wb");
//	fprintf(fp, "test");
//	fclose(fp);
//}
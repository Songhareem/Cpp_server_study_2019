// Thread_Debug.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "define.h"

////////////////////////////////////////////////////////
// ������ ��
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
// ���ӿ�û ���. 
//
// IOThread ���� �ֱ������� ����, 
// AcceptThread ���� �� ����Ʈ�� ��(SessionID ��)�� �̾Ƽ�  ���ο� Session �� �����.
// �̹� �����ϴ� SessionID �� ���ð�� ���� �ؾ� ��
////////////////////////////////////////////////////////
CRITICAL_SECTION	g_Accept_cs;
list<DWORD>			g_AcceptPacketList;

#define				LockAccept()	EnterCriticalSection(&g_Accept_cs)
#define 			UnlockAccept()	LeaveCriticalSection(&g_Accept_cs)

////////////////////////////////////////////////////////
// �׼� ��û ���.
//
// IOThread ���� �ֱ������� ����(SessionID),
// UpdateThread ������ �� ���� �̾Ƽ� �ش� �÷��̾��� Content + 1 �� �Ѵ�.
// �������� �ʴ� SessionID �� ������ �����ؾ� ��.
////////////////////////////////////////////////////////
CRITICAL_SECTION	g_Action_cs;
list<DWORD>			g_ActionPacketList;

#define				LockAction()	EnterCriticalSection(&g_Action_cs)
#define				UnlockAction()	LeaveCriticalSection(&g_Action_cs)

////////////////////////////////////////////////////////
// �������� ��û ���
//
// IOThread ���� �ֱ������� ����, 
// AcceptThread ���� �� ����Ʈ�� ���� �̾Ƽ� (SessionID ��) �ش� Session �� ���� ��Ų��.
////////////////////////////////////////////////////////
CRITICAL_SECTION	g_Disconnect_cs;
list<DWORD>			g_DisconnectPacketList;

#define				LockDisconnect()	EnterCriticalSection(&g_Disconnect_cs)
#define				UnlockDisconnect()	LeaveCriticalSection(&g_Disconnect_cs)

////////////////////////////////////////////////////////
// Session ���.
//
// ������ �Ϸ�� (Accept ó�� �Ϸ�)  st_SESSION �� ���� �����Ͽ�, SessionList �� �����͸� �ִ´�.
// �׸��� ������ ������ �� �ش� ������ ���� �Ѵ�.
////////////////////////////////////////////////////////
CRITICAL_SECTION		g_Session_cs;
list<st_SESSION *>		g_SessionList;

#define	LockSession()	EnterCriticalSection(&g_Session_cs)
#define UnlockSession()	LeaveCriticalSection(&g_Session_cs)

////////////////////////////////////////////////////////
// Player ���.
//
// Session �� ���� ��, ���� �ɶ� (Accept ó�� �Ϸ��)  st_PLAYER ��ü�� �Բ� �����Ǿ� ���⿡ ��� �ȴ�.
////////////////////////////////////////////////////////

st_PLAYER g_PlayerList[dfMAX_PLAYER];

// EventHandle
HANDLE	g_hExitThreadEvent;
HANDLE	g_hAcceptThreadEvent;
HANDLE	g_hUpdateThreadEvent;

// Debug sz
WCHAR *g_szDebug;

void NewSession(DWORD dwSessionID)
{
	st_SESSION *pSession = new st_SESSION;
	pSession->SessionID = dwSessionID;

	LockSession();
	g_SessionList.push_back(pSession);
	UnlockSession();

	// �÷��̾� �߰� �ʱ�ȭ
	bool isFull = true;
	for (int index = 0; index < dfMAX_PLAYER; index++) {

		if (g_PlayerList[index].OnState == ON_STATE_ACCEPT) {
			
			g_PlayerList[index].SessionID = dwSessionID;
			g_PlayerList[index].Content[0] = 0;
			g_PlayerList[index].Content[1] = 0;
			g_PlayerList[index].Content[2] = 0;
			g_PlayerList[index].OnState = ON_STATE_ACCEPT2UPDATE;
		
			isFull = false;
			break;
		}
	}

	if (isFull)
		wprintf(L"Player Full\n");
}

void DeleteSession(DWORD dwSessionID)
{
	LockSession();
	list<st_SESSION *>::iterator SessionIter = g_SessionList.begin();
	list<st_SESSION *>::iterator SessionIterEnd = g_SessionList.end();
	for (; SessionIter != SessionIterEnd; ++SessionIter)
	{
		if (dwSessionID == (*SessionIter)->SessionID)
		{
			delete *SessionIter;
			g_SessionList.erase(SessionIter);
			break;
		}
	}
	UnlockSession();

	// Player Delete FlagON
	for (int index = 0; index < dfMAX_PLAYER; index++) {

		if (dwSessionID == g_PlayerList[index].SessionID) {

			g_PlayerList[index].DisconnectFlag = true;
			
			break;
		}
	}
}

// �ش� ���� ID�� �ִٸ� true, ���ٸ� false
bool FindSessionList(DWORD dwSessionID)
{
	LockSession();
	list<st_SESSION *>::iterator SessionIter = g_SessionList.begin();
	list<st_SESSION *>::iterator SessionIterEnd = g_SessionList.end();
	for (; SessionIter != SessionIterEnd; ++SessionIter)
	{
		if (dwSessionID == (*SessionIter)->SessionID)
		{
			UnlockSession();
			return true;
		}
	}

	UnlockSession();

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// Accept Thread
/////////////////////////////////////////////////////////////////////////////////////
unsigned int WINAPI AcceptThread(LPVOID lpParam)
{
	HANDLE hEvent[2] = {g_hExitThreadEvent, g_hAcceptThreadEvent};
	DWORD dwError;
	DWORD dwSessionID;
	bool bLoop = true;
	bool bFindIt = false;

	wprintf(L"Accept Thread Start\n");
	srand(GetTickCount() + 1);

	while ( bLoop )
	{
		dwError = WaitForMultipleObjects(2, hEvent, FALSE, INFINITE);
		if ( dwError == WAIT_FAILED )
 		{
			wprintf(L"Accept Thread Event Error\n");
			wprintf(L"Accept Thread Event Error\n");
			wprintf(L"Accept Thread Event Error\n");
			wprintf(L"Accept Thread Event Error\n");
			break;
		}

		if ( dwError == WAIT_OBJECT_0 )
			break;

		//----------------------------------------------------------
		//----------------------------------------------------------
		// ���� ����ó�� 
		//----------------------------------------------------------
		//----------------------------------------------------------

		//----------------------------------------------------------
		// ���ӿ�û ó��
		//----------------------------------------------------------
		
		while ( !g_AcceptPacketList.empty() )
		{
			dwSessionID = *g_AcceptPacketList.begin();
			LockAccept();
			g_AcceptPacketList.pop_front();
			UnlockAccept();

			//----------------------------------------------------------
			// SessionList �� �̹� �����ϴ� SessionID ���� Ȯ��, ���� ��츸 ���.
			// ���ڸ��� OnState���� Accept���� ���
			//----------------------------------------------------------
			if ( !FindSessionList(dwSessionID) )
			{
				// New Session
				NewSession(dwSessionID);	
				wprintf(L"AcceptThread - New Session[%d]\n", dwSessionID);
			}
		}

		//----------------------------------------------------------
		// �������� ó��
		//----------------------------------------------------------
		while ( !g_DisconnectPacketList.empty())
		{
			dwSessionID = *g_DisconnectPacketList.begin();
			LockDisconnect();
			g_DisconnectPacketList.pop_front();
			UnlockDisconnect();

			//----------------------------------------------------------
			// SessionList �� �����ϴ� SessionID ���� Ȯ��.  �ִ� ��츸 ����
			//----------------------------------------------------------
			if ( FindSessionList(dwSessionID) )
			{
				DeleteSession(dwSessionID);
				wprintf(L"AcceptThread - Delete Session[%d]\n", dwSessionID);
			}
		}
	}

	wprintf(L"Accept Thread Exit\n");
	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// IO Thread
/////////////////////////////////////////////////////////////////////////////////////
unsigned int WINAPI IOThread(LPVOID lpParam)
{
	DWORD dwError;
	bool bLoop = true;
	int iRand = 0;
	DWORD dwSessionID = 0;

	srand(GetTickCount() + 2);
	wprintf(L"IO Thread Start\n");

	while ( bLoop )
	{
		dwError = WaitForSingleObject(g_hExitThreadEvent, 10);
		if ( dwError != WAIT_TIMEOUT )
			break;

		//----------------------------------------------------------
		// ���� ����ó�� 
		//----------------------------------------------------------
		iRand = rand() % 3;
		dwSessionID = rand() % 5000;
		//dwSessionID++;
		switch ( iRand )
		{
		case 0:			// Accept �߰�
			wsprintf(g_szDebug, L"# IOThread AcceptPacket Insert [%d] \n", dwSessionID);
			LockAccept();
			g_AcceptPacketList.push_back(dwSessionID);
			UnlockAccept();
			SetEvent(g_hAcceptThreadEvent);
			break;

		case 1:			// Disconnect �߰�
			wsprintf(g_szDebug, L"# IOThread DisconnetPacket Insert [%d] \n", dwSessionID);
			LockDisconnect();
			g_DisconnectPacketList.push_back(dwSessionID);
			UnlockDisconnect();
			SetEvent(g_hAcceptThreadEvent);
			break;

		case 2:			// Action �߰�
			wsprintf(g_szDebug, L"# IOThread ActionPacket Insert [%d] \n", dwSessionID);
			LockAction();
			g_ActionPacketList.push_back(dwSessionID);
			UnlockAction();
			SetEvent(g_hUpdateThreadEvent);
			break;
		}
		OutputDebugString(g_szDebug);
	}


	wprintf(L"IO Thread Exit\n");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// Update Thread
/////////////////////////////////////////////////////////////////////////////////////
unsigned int WINAPI UpdateThread(LPVOID lpParam)
{
	HANDLE hEvent[2] = {g_hExitThreadEvent, g_hUpdateThreadEvent};
	DWORD dwError;
	DWORD dwSessionID;
	bool bLoop = true;

	srand(GetTickCount() + 3);

	wprintf(L"Update Thread Start\n");

	while (bLoop)
	{
		dwError = WaitForMultipleObjects(2, hEvent, FALSE, INFINITE);
		if (dwError == WAIT_FAILED)
		{
			wprintf(L"Update Thread Event Error\n");
			wprintf(L"Update Thread Event Error\n");
			wprintf(L"Update Thread Event Error\n");
			wprintf(L"Update Thread Event Error\n");
			break;
		}

		if (dwError == WAIT_OBJECT_0)
			break;

		//----------------------------------------------------------
		// ���� ����ó�� 
		//----------------------------------------------------------
		for (int i = 0; i < dfMAX_PLAYER; i++) {

			// ��ü ��ä�� ��� ���¿��� Update Thread�� ���º�ȯ
			if (g_PlayerList[i].OnState == ON_STATE_ACCEPT2UPDATE)
				g_PlayerList[i].OnState = ON_STATE_UPDATE;

			// Disconnect ó��
			if (g_PlayerList[i].DisconnectFlag) {

				g_PlayerList[i].OnState = ON_STATE_ACCEPT;
				g_PlayerList[i].DisconnectFlag = false;
			}
		}

		//----------------------------------------------------------
		// �÷��̾� �׼� ó��
		//----------------------------------------------------------
		while (!g_ActionPacketList.empty())
		{
			dwSessionID = *g_ActionPacketList.begin();
			LockAction();
			g_ActionPacketList.pop_front();
			UnlockAction(); // �߰�
			
			//----------------------------------------------------------
			// PlayerList �� �̹� �����ϴ� SessionID ���� Ȯ��. �ִ� ��츸 �ش� �÷��̾� ã�Ƽ� + 1
			//----------------------------------------------------------
			for (int index = 0; index < dfMAX_PLAYER; index++) {

				if(dwSessionID == g_PlayerList[index].SessionID && g_PlayerList[index].OnState == ON_STATE_UPDATE) {

					g_PlayerList[index].Content[0]++;
					g_PlayerList[index].Content[1]++;
					g_PlayerList[index].Content[2]++;

					wprintf(L"UpdateThread - Session[%d] Content[%d/%d/%d] \n",
						dwSessionID, g_PlayerList[index].Content[0],
						g_PlayerList[index].Content[1], g_PlayerList[index].Content[2]);

					break;
				}
			}
		}
	}
	wprintf(L"Update Thread Exit\n");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// Initial
/////////////////////////////////////////////////////////////////////////////////////
void Initial()
{
	//g_szDebug = new WCHAR[40];
	g_szDebug = new WCHAR[50];

	//------------------------------------------------
	// ������ �����带 ���� �̺�Ʈ
	//------------------------------------------------
	g_hAcceptThreadEvent	= CreateEvent(NULL, FALSE, FALSE, NULL);
	g_hUpdateThreadEvent	= CreateEvent(NULL, FALSE, FALSE, NULL);

	//------------------------------------------------
	// ��� �����带 ���� ��ų �̺�Ʈ
	//------------------------------------------------
	//g_hExitThreadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	g_hExitThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	InitializeCriticalSection(&g_Accept_cs);
	InitializeCriticalSection(&g_Action_cs);
	InitializeCriticalSection(&g_Disconnect_cs);
	//InitializeCriticalSection(&g_Player_cs);
	InitializeCriticalSection(&g_Session_cs);

}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// Release
/////////////////////////////////////////////////////////////////////////////////////
void Release()
{
	g_AcceptPacketList.clear();
	g_ActionPacketList.clear();
	g_DisconnectPacketList.clear();

	list<st_SESSION *>::iterator SessionIter = g_SessionList.begin();
	list<st_SESSION *>::iterator SessionIterEnd = g_SessionList.end();
	while ( SessionIter != SessionIterEnd)
	{
		delete *SessionIter;
		SessionIter = g_SessionList.erase(SessionIter);
		//g_SessionList.erase(SessionIter);
	}

	delete[] g_szDebug;

	DeleteCriticalSection(&g_Accept_cs);
	DeleteCriticalSection(&g_Action_cs);
	DeleteCriticalSection(&g_Disconnect_cs);
	DeleteCriticalSection(&g_Session_cs);
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// Main
/////////////////////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hIOThread;			// ���ӿ�û, �����û, �׼ǿ�û �߻�.  (IO �ùķ��̼�)
	HANDLE hAcceptThread;		// ���ӿ�û, ���⿡ ���� ó��	
	HANDLE hUpdateThread;		// �׼ǿ�û ó��.

	DWORD dwThreadID;

	Initial();

	//------------------------------------------------
	// ������ ����.
	//------------------------------------------------
	hAcceptThread	= (HANDLE)_beginthreadex(NULL, 0, AcceptThread, (LPVOID)0, 0, (unsigned int *)&dwThreadID);
	hIOThread		= (HANDLE)_beginthreadex(NULL, 0, IOThread,	(LPVOID)0, 0, (unsigned int *)&dwThreadID);
	hUpdateThread	= (HANDLE)_beginthreadex(NULL, 0, UpdateThread, (LPVOID)0, 0, (unsigned int *)&dwThreadID);


	WCHAR ControlKey;

	//------------------------------------------------
	// ���� ��Ʈ��...
	//------------------------------------------------
	while ( 1 )
	{	
		ControlKey = _getwch();
		if ( ControlKey == L'q' || ControlKey == L'Q' )
		{
			//------------------------------------------------
			// ����ó��
			//------------------------------------------------
			SetEvent(g_hExitThreadEvent);
			break;
		}
	}


	//------------------------------------------------
	// ������ ���� ���
	//------------------------------------------------
	HANDLE hThread[3] = {hAcceptThread, hIOThread, hUpdateThread};
	WaitForMultipleObjects(dfTHREAD_NUM, hThread, TRUE, INFINITE);

	Release();

	//------------------------------------------------
	// ������ �ڵ�  ������ �������� Ȯ��.
	//------------------------------------------------
	DWORD ExitCode;

	wprintf(L"\n\n--- THREAD CHECK LOG -----------------------------\n\n");

	GetExitCodeThread(hAcceptThread, &ExitCode);
	if ( ExitCode != 0 )
		wprintf(L"error - Accept Thread not exit\n");

	GetExitCodeThread(hIOThread, &ExitCode);
	if ( ExitCode != 0 )
		wprintf(L"error - IO Thread not exit\n");

	GetExitCodeThread(hUpdateThread, &ExitCode);
	if ( ExitCode != 0 )
		wprintf(L"error - Update Thread not exit\n");

	CloseHandle(hAcceptThread);
	CloseHandle(hIOThread);
	CloseHandle(hUpdateThread);
	CloseHandle(g_hAcceptThreadEvent);
	CloseHandle(g_hUpdateThreadEvent);
	CloseHandle(g_hExitThreadEvent);

	system("pause");

	return 0;
}



#pragma comment(lib,"winmm")

#include "CNetworkLib.h"
//#include "CGameServer.h"
#include <locale.h>

int main() {

	timeBeginPeriod(1);
	
	setlocale(LC_ALL, "");

	CNetworkLib CNet;

	while (1) {

		if (GetAsyncKeyState(VK_SPACE) & 0x8001)
			break;

		Sleep(200);
	}

	timeEndPeriod(1);
	
	return 0;
}

//class CNetwork {
//public:
//	��Ŀ ������ �Լ�() {
//		����Լ� ȣ��()
//	}
//
//	���Ʈ ������ �Լ�() {
//		����Լ� ȣ��()
//		// OnAccept()
//	}
//
//	CNetwork() {
//		/*
//			��Ŀ ������ ����
//			���Ʈ ������ ����
//		*/
//	}
//
//	~CNetwork() {
//
//	}
//
//private:
//	HANDLE hThread[2];
//	map<SessionID, Session *> sessionMap;
//};
//
//class CGameServer {
//public:
//	virtual void OnRecv(...) {
//		// �Ҳ� �ϱ�
//	}
//
//	CNetwork() {
//		/*
//			��Ŀ ������ ����
//			���Ʈ ������ ����
//		*/
//	}
//
//	~CNetwork() {
//
//	}
//
//private:
//	HANDLE hThread[2];	// public
//	map<SessionID, Session *> sessionMap;
//};
//
//CGameServer cGame;
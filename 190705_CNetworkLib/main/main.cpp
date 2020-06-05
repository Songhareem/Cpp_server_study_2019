
#include "CNetworkLib.h"

#pragma comment(lib,"winmm")

int main() {

	timeBeginPeriod(1);
	
	int iRet = 0;

	CNetworkLib Net;
	
	DWORD	NetMaxThread = Net.GetNetMaxThread();
	HANDLE *pThreadHandle = Net.GetThreadHandle();

	while (1) {

		if (GetKeyState(VK_SPACE) & 0x8001)
			break;

		iRet = WaitForMultipleObjects(NetMaxThread, pThreadHandle, true, 200);
		if (WAIT_TIMEOUT != iRet)
			break;
	}

	timeEndPeriod(1);
	
	return 0;
}

//class CNetwork {
//public:
//	워커 스레드 함수() {
//		멤버함수 호출()
//	}
//
//	어셉트 스레드 함수() {
//		멤버함수 호출()
//		// OnAccept()
//	}
//
//	CNetwork() {
//		/*
//			워커 스레드 생성
//			어셉트 스레드 생성
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
//		// 할꺼 하기
//	}
//
//	CNetwork() {
//		/*
//			워커 스레드 생성
//			어셉트 스레드 생성
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
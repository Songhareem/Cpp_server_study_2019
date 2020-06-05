
#include "pch.h"
#include <iostream>
#include "Server.h"

#pragma comment(lib,"winmm")

int main()
{
	timeBeginPeriod(1);

	setlocale(LC_ALL, " ");

	// 네티워크 초기화 및 listen
	NetStartUp();

	while (!g_bShutdown) {

		// 네트워크 송수신 처리
		NetIOProc();

		// 게임 로직 업데이트 게임 프레임 업데이트 약 20ms
		if (FrameSkip())
			Update();

		// 서버 컨트롤
		ServerControl();

		// 서버 모니터링
		Monitoring();
	}

	// 서버 마무리 및 종료 작업
	// DB에 저장 및 마무리 할 일 확인 후 종료

	// 네트워크 종료
	NetCleanUp();

	timeEndPeriod(1);
	return 0;
}

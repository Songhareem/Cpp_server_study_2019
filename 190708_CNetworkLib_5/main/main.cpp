
#pragma comment(lib,"winmm")

#include "CNetworkLib.h"
#include "CGameServer.h"
#include <locale.h>

int main() {

	timeBeginPeriod(1);
	
	setlocale(LC_ALL, "");

	CGameServer Game;

	while (1) {

		if (GetAsyncKeyState(VK_SPACE) & 0x8001)
			break;

		Sleep(200);
	}

	timeEndPeriod(1);
	
	return 0;
}

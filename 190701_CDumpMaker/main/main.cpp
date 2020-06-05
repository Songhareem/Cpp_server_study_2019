
#include <iostream>
#include "SelfDump.h"
#include "CDumpMaker.h"

CDumpMaker Dump;
long CDumpMaker::_DumpCount = 0;

int main() {

	while(1) {
		
		// A ¹öÆ°
		if (GetAsyncKeyState(0x41) & 0x8001) {

			int a = 0;

			a = a / 0;
		}

		Sleep(200);
	}
	
	return 0;
}
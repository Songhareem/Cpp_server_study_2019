
#include <Windows.h>
#include <iostream>
#include <process.h>
#include "CLockFreeStack.h"

#define dfMAX_THREAD	4
#define	dfMAX_DATA		1000
#define dfMAX_All_DATA	(dfMAX_THREAD * dfMAX_DATA)

#pragma comment(lib,"winmm")

typedef struct st_TEST {

	LONG64 data = 0x00000000ffffffff;
	LONG64 count = 0;

} TEST;

CLockFreeStack<TEST*> LFStack;

unsigned int __stdcall Thread(void *arg) {

	TEST *pData[dfMAX_DATA];

	printf("BeginThread\n");

	for(;;) {
		for (int i = 0; i < dfMAX_DATA; i++) {

			LFStack.Pop(&pData[i]);

			//printf("[%d] POP\n",pData[i]);
		}

		for (int i = 0; i < dfMAX_DATA; i++) {

			if (pData[i]->data != 0x00000000ffffffff || pData[i]->count != 0) {
				int a = 0;
			}

			InterlockedIncrement64(&pData[i]->data);
			InterlockedIncrement64(&pData[i]->count);
		}

		for (int i = 0; i < dfMAX_DATA; i++) {

			if (pData[i]->data != 0x0000000100000000 || pData[i]->count != 1) {
				int a = 0;
			}
			InterlockedDecrement64(&pData[i]->data);
			InterlockedDecrement64(&pData[i]->count);
		}

		for (int i = 0; i < dfMAX_DATA; i++) {

			//printf("[%d] PUSH\n", pData[i]);

			LFStack.Push(pData[i]);
		}
	
		//printf("StackSize : %d KeyCount : %lld\n", LFStack.GetStackSize(), LFStack.GetKeyCount());
	}
	return 0;
}

int main() {

	timeBeginPeriod(1);

	unsigned int uiID;
	HANDLE hThread[dfMAX_THREAD];

	for (int i = 0; i < dfMAX_All_DATA; i++) {

		TEST *pData = new TEST;

		LFStack.Push(pData);
	}

	for (int i = 0; i < dfMAX_THREAD; i++) {

		hThread[i] = (HANDLE)_beginthreadex(NULL, 0, Thread, NULL, 1, &uiID);
	}

	for (;;) {

		if (GetAsyncKeyState(VK_SHIFT) & 0001) {

			printf("%d\n", LFStack.GetStackSize());
		}
		printf("%d\n", LFStack.GetStackSize());
		Sleep(1000);
	}

	timeEndPeriod(1);


	return 0;
}

#include <Windows.h>
#include <iostream>
#include <process.h>
#include "CLockFreeQ.h"

#define dfMAX_THREAD	4
#define	dfMAX_DATA		5
#define dfMAX_ALL_DATA	(dfMAX_THREAD * dfMAX_DATA)

#pragma comment(lib,"winmm")

typedef struct st_TEST {

	LONG64 data = 0x00000000ffffffff;
	LONG64 count = 0;

} TEST;

CLockFreeQ<TEST*> LFQue;

unsigned int __stdcall Thread(void *arg) {

	TEST *pData[dfMAX_DATA];

	printf("BeginThread\n");

	for (;;) {
		for (int i = 0; i < dfMAX_DATA; i++) {

			LFQue.Deque(&pData[i]);

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

			LFQue.Enque(pData[i]);
		}

		//printf("StackSize : %d KeyCount : %lld\n", LFStack.GetStackSize(), LFStack.GetKeyCount());
	}
	return 0;
}

void TestQ() {

	CLockFreeQ<int> intLFQue;

	int data[dfMAX_ALL_DATA];
	
	printf("[Enque] ");
	for (int i = 0; i < dfMAX_ALL_DATA; i++) {

		data[i] = i+1;

		printf("%d ", data[i]);
		intLFQue.Enque(data[i]);
	}
	printf("\n");

	ZeroMemory(data, sizeof(int) * dfMAX_ALL_DATA);

	printf("[Deque] ");
	for (int i = 0; i < dfMAX_ALL_DATA; i++) {

		intLFQue.Deque(&data[i]);

		printf("%d ", data[i]);
	}
	printf("\n");

	Sleep(5000);
}

int main() {

	timeBeginPeriod(1);

	unsigned int uiID;
	HANDLE hThread[dfMAX_THREAD];

	//TestQ();

	for (int i = 0; i < dfMAX_ALL_DATA; i++) {

		TEST *pData = new TEST;

		LFQue.Enque(pData);
	}

	for (int i = 0; i < dfMAX_THREAD; i++) {

		hThread[i] = (HANDLE)_beginthreadex(NULL, 0, Thread, NULL, 1, &uiID);
	}

	for (;;) {

		if (GetAsyncKeyState(VK_SHIFT) & 0001) {

			printf("%d==\n", LFQue.GetQueSize());
		}
		printf("%d\n", LFQue.GetQueSize());
		Sleep(250);
	}

	timeEndPeriod(1);

	return 0;
}
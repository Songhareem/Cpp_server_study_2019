
#include "pch.h"
#include <iostream>
#include <windows.h>
#include <process.h>

#define dfMAX_HAND	3

#pragma comment(lib,"winmm")

int g_iCnt = 0;
int g_Thread_1_Cnt = 0;
int g_Thread_2_Cnt = 0;
int g_Thread_3_Cnt = 0;

unsigned int __stdcall Thread_1(LPVOID lpThread_1);
unsigned int __stdcall Thread_2(LPVOID lpThread_2);
unsigned int __stdcall Thread_3(LPVOID lpThread_3);

int main()
{
	timeBeginPeriod(1);

	DWORD someData = 42;
	unsigned int InitFlag = CREATE_SUSPENDED;
	HANDLE hand[dfMAX_HAND];
	DWORD dwID[dfMAX_HAND];
	DWORD dwExitCode[dfMAX_HAND];

	hand[0] = (HANDLE)_beginthreadex(NULL, 0, Thread_1, (void *)someData, InitFlag, (unsigned int*)&dwID[0]);
	hand[1] = (HANDLE)_beginthreadex(NULL, 0, Thread_2, (void *)someData, InitFlag, (unsigned int*)&dwID[1]);
	hand[2] = (HANDLE)_beginthreadex(NULL, 0, Thread_3, (void *)someData, InitFlag, (unsigned int*)&dwID[2]);

	printf("save point\n");

	// GetThreadid / GetExitCodeThread / TerminateThread

	/*printf("Thread_1 ID : %d\n", GetThreadId(hand[0]));
	printf("Thread_2 ID : %d\n", GetThreadId(hand[1]));
	printf("Thread_3 ID : %d\n", GetThreadId(hand[2]));

	GetExitCodeThread(hand[0], &dwExitCode[0]);
	GetExitCodeThread(hand[1], &dwExitCode[1]);
	GetExitCodeThread(hand[2], &dwExitCode[2]);

	printf("state Thread_1 : %d\n", dwExitCode[0]);
	printf("state Thread_2 : %d\n", dwExitCode[1]);
	printf("state Thread_3 : %d\n", dwExitCode[2]);

	DWORD dwTmp = STILL_ACTIVE;
	TerminateThread(hand[0], dwTmp);
	TerminateThread(hand[1], dwTmp);
	TerminateThread(hand[2], dwTmp);

	Sleep(1000);

	GetExitCodeThread(hand[0], &dwExitCode[0]);
	GetExitCodeThread(hand[1], &dwExitCode[1]);
	GetExitCodeThread(hand[2], &dwExitCode[2]);

	printf("state Thread_1 : %d\n", dwExitCode[0]);
	printf("state Thread_2 : %d\n", dwExitCode[1]);
	printf("state Thread_3 : %d\n", dwExitCode[2]);
*/

	// Priority

	printf("1 : %d\n", GetThreadPriority(hand[0]));
	printf("2 : %d\n", GetThreadPriority(hand[1]));
	printf("3 : %d\n", GetThreadPriority(hand[2]));

	for(int i=0; i<dfMAX_HAND; i++)
		ResumeThread(hand[i]);

	while (1) {

		if (g_iCnt > 1000)
			break;

		//printf("g_iCnt : %d\n", g_iCnt);
	}

	printf("Thread_1 Cnt : %d\n", g_Thread_1_Cnt);
	printf("Thread_2 Cnt : %d\n", g_Thread_2_Cnt);
	printf("Thread_3 Cnt : %d\n", g_Thread_3_Cnt);

	// Priority
	printf("1 : %d\n", GetThreadPriority(hand[0]));
	printf("2 : %d\n", GetThreadPriority(hand[1]));
	printf("3 : %d\n", GetThreadPriority(hand[2]));

	timeEndPeriod(1);

	return 0;
}

unsigned int __stdcall Thread_1(LPVOID lpThread_1) {

	while (1) {

		g_iCnt++;

		g_Thread_1_Cnt++;

		printf("Thread_1 | g_iCnt : %d\n", g_iCnt);

		Sleep(0);
	}

	return 1;
}

unsigned int __stdcall Thread_2(LPVOID lpThread_1) {

	while (1) {

		g_iCnt++;

		g_Thread_2_Cnt++;

		printf("Thread_2 | g_iCnt : %d\n", g_iCnt);

		Sleep(0);
	}

	return 1;
}

unsigned int __stdcall Thread_3(LPVOID lpThread_1) {

	while (1) {

		g_iCnt++;

		g_Thread_3_Cnt++;

		printf("Thread_3 | g_iCnt : %d\n", g_iCnt);

		Sleep(0);
	}

	return 1;
}

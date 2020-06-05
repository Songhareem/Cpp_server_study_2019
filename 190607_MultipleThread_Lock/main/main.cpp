
// MAXIMUM_WAIT_OBJECTS = 64개
// Sleep(0) / Sleep(1) 차이 ref : https://informationtechnology.tistory.com/7


#include "pch.h"
#include <iostream>
#include <process.h>
#include <Windows.h>

#pragma comment(lib,"winmm")

#define dfMAX_THREAD	5

int		g_iData = 0;			// 가상의 데이터 처리
int		g_iConnect = 0;			// 가상의 접속자
bool	g_bShutdown = false;	// 종료 Flag

CRITICAL_SECTION	g_cs;		// Critical Section
SRWLOCK				g_SrwLock;	// SrwLock

unsigned int __stdcall AcceptThread(LPVOID lparam);
unsigned int __stdcall DisconnectThread(LPVOID lparam);
unsigned int __stdcall UpdateThread(LPVOID lparam);

int main()
{
	timeBeginPeriod(1);

	// CRITICAL_SECTION, SRWLock 초기화
	InitializeCriticalSection(&g_cs);
	//InitializeSRWLock(&g_SrwLock);

	// 스레드 생성 및 시작
	HANDLE hThreadHand[dfMAX_THREAD];
	DWORD dwThreadID[dfMAX_THREAD];
	DWORD dwThreadParam[dfMAX_THREAD] = { 0, };

	hThreadHand[0] = (HANDLE)_beginthreadex(NULL, 0, AcceptThread, (void *)&dwThreadParam[0], 0, (unsigned int *)&dwThreadID[0]);
	hThreadHand[1] = (HANDLE)_beginthreadex(NULL, 0, DisconnectThread, (void *)&dwThreadParam[1], 0, (unsigned int *)&dwThreadID[1]);

	for (int i = 2; i < dfMAX_THREAD; i++)
		hThreadHand[i] = (HANDLE)_beginthreadex(NULL, 0, UpdateThread, (void *)&dwThreadParam[i], 0, (unsigned int *)&dwThreadID[i]);

	// 1초마다 print, 20초 후 break
	DWORD dwStartTime = timeGetTime();
	DWORD dwEndTime = dwStartTime + 20000;
	printf("StartTime : %d, EndTime : %d\n", dwStartTime, dwEndTime);
	for (;;) {

		DWORD dwCurrTime = timeGetTime();

		// 20초 후 break
		if (dwCurrTime >= dwEndTime) {
			
			printf("CurrTime : %d\n", dwCurrTime);
			break;
		}
		// 1초마다 print
		if (dwCurrTime - dwStartTime >= 1000) {
			printf("CurrTime : %d\n", dwCurrTime);
			
			/*EnterCriticalSection(&g_cs);
			printf("Connect : %d\n", g_iConnect);
			LeaveCriticalSection(&g_cs);*/

			AcquireSRWLockExclusive(&g_SrwLock);
			printf("Connect : %d\n", g_iConnect);
			ReleaseSRWLockExclusive(&g_SrwLock);

			dwStartTime = dwCurrTime;
		}

		// Sleep(1) 비교
		WaitForSingleObject(GetCurrentThread(), 1);
	}

	// g_bShutdown false -> true
	g_bShutdown = true;
	
	// MultipleObject로 종료확인 후 종료
	DWORD retval = WaitForMultipleObjects(dfMAX_THREAD, hThreadHand, true, INFINITE);
	
	printf("g_iData : %d\n", g_iData);

	// 핸들 없애기
	for (int i = 0; i < dfMAX_THREAD; i++)
		CloseHandle(hThreadHand[i]);

	DeleteCriticalSection(&g_cs);
	timeEndPeriod(1);

	return 0;
}

unsigned int __stdcall AcceptThread(LPVOID lparam) {

	while (!g_bShutdown) {

		_InterlockedIncrement((long *)&g_iConnect);
	
		DWORD dwTimeout = rand() % 901 + 100;
		//DWORD dwTimeout = 100;
		WaitForSingleObject(GetCurrentThread(), dwTimeout);
	}

	printf("AcceptThread Exit\n");

	return 0;
}

unsigned int __stdcall DisconnectThread(LPVOID lparam) {

	while (!g_bShutdown) {

		_InterlockedDecrement((long *)&g_iConnect);
	
		DWORD dwTimeout = rand() % 901 + 100;
		//DWORD dwTimeout = 100;
		WaitForSingleObject(GetCurrentThread(), dwTimeout);
	}

	printf("DisconnectThread Exit\n");

	return 0;
}

unsigned int __stdcall UpdateThread(LPVOID lparam) {
	
	DWORD dwTickTime = timeGetTime();
	while (!g_bShutdown) {

		DWORD dwCurrTime = timeGetTime();
		if (dwCurrTime < dwTickTime + 10)
			continue;

		dwTickTime = dwCurrTime;

		int iData = _InterlockedIncrement((long *)&g_iData);
		if(iData % 1000 == 0)
			printf("g_iData : %d\n", g_iData);

		/*EnterCriticalSection(&g_cs);
		g_iData++;
		if (g_iData % 1000 == 0)
			printf("g_iData : %d\n", g_iData);
		LeaveCriticalSection(&g_cs);*/

		/*AcquireSRWLockExclusive(&g_SrwLock);
		g_iData++;
		if (g_iData % 1000 == 0)
			printf("g_iData : %d\n", g_iData);
		ReleaseSRWLockExclusive(&g_SrwLock);*/

		//DWORD dwTimeout = 10;
		//WaitForSingleObject(GetCurrentThread(), dwTimeout);
		
		WaitForSingleObject(GetCurrentThread(), 1);
	}

	printf("UpdateThread Exit\n");

	return 0;
}
// EventPractice.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

// 정수 -> 스트링 / 스트링 -> 정수
// std::string s = std::to_string(42);
// std::string s = std::to_stoi(42);

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <process.h>
#include <Windows.h>
#include <time.h>
#include <list>

#pragma comment(lib,"winmm")

#define dfMAX_THREAD	6

using namespace std;

list<DWORD> g_dwList;
HANDLE g_hSave;
bool g_bServerAlive = true;
CRITICAL_SECTION g_cs;
SRWLOCK g_srwlock;

bool GetKey(void);
void Log();
unsigned int __stdcall PrintThread(void *Param);
unsigned int __stdcall DeleteThread(void *Param);
unsigned int __stdcall WorkerThread(void *Param);
unsigned int __stdcall SaveThread(void *Param);

int main()
{
	timeBeginPeriod(1);
	
	setlocale(LC_ALL, " ");
	InitializeCriticalSection(&g_cs);
	
	HANDLE hThread[dfMAX_THREAD];
	DWORD dwThreadID[dfMAX_THREAD];

	// 보안, true 수동 / false 자동, true 시그널 / false 논시그널, 
	// 이벤트 객체 이름(있다면, 타 스레드에서 호출 가능 / 없다면 CreateEvent한 스레드에서만 호출 가능)
	g_hSave = CreateEvent(NULL, true, false, NULL);

	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, PrintThread, NULL, 0, (unsigned int*)&dwThreadID[0]);
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, DeleteThread, NULL, 0, (unsigned int*)&dwThreadID[1]);
	hThread[2] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, NULL, 0, (unsigned int*)&dwThreadID[2]);
	hThread[3] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, NULL, 0, (unsigned int*)&dwThreadID[3]);
	hThread[4] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, NULL, 0, (unsigned int*)&dwThreadID[4]);
	hThread[5] = (HANDLE)_beginthreadex(NULL, 0, SaveThread, NULL, 0, (unsigned int*)&dwThreadID[5]);
	
	for (;;) {

		// Key입력
		if (!GetKey())
			break;

		WaitForSingleObject(GetCurrentThread(), 1);
	}

	WaitForMultipleObjects(dfMAX_THREAD, hThread, true, INFINITE);
	printf("Confirm\n");

	DeleteCriticalSection(&g_cs);

	CloseHandle(g_hSave);
	for (int i = 0; i < dfMAX_THREAD; ++i)
		CloseHandle(hThread[i]);
	
	timeEndPeriod(1);

	return 0;
}

bool GetKey(void) {

	// Log
	if (GetAsyncKeyState(VK_SPACE) & 0x0001) {

		// SaveThread 살리기
		printf("Save Data\n");
		SetEvent(g_hSave);
	}

	// 프로그램 종료 요청
	if (GetAsyncKeyState(VK_ESCAPE) & 0x0001) {

		printf("ESC Program\n");
		g_bServerAlive = false;
		return false;
	}

	return true;
}

void Log() {
	wstring wstr;
	wofstream wFile("Log.txt");

	/*static int iCnt = 0;
	iCnt++;
	if (iCnt > 2) {

		wFile.clear();
		iCnt = 0;
	}*/

	EnterCriticalSection(&g_cs);
	list<DWORD>::iterator iterEnd = g_dwList.end();

	for (list<DWORD>::iterator iter = g_dwList.begin(); iter != iterEnd; ++iter) {

		wstr += to_wstring((*iter));
		wstr += L" ";
	}

	LeaveCriticalSection(&g_cs);

	wFile << wstr;
	wFile.close();
}

unsigned int __stdcall PrintThread(void *Param) {

	while (g_bServerAlive) {

		EnterCriticalSection(&g_cs);
		list<DWORD>::iterator iter;
		list<DWORD>::iterator iterEnd = g_dwList.end();
		for (iter = g_dwList.begin(); iter != iterEnd; ++iter) {

			printf("%d ", (*iter));
		}
		LeaveCriticalSection(&g_cs);

		printf("\n");

		WaitForSingleObject(GetCurrentThread(), 1000);
	}

	return 0;
}

unsigned int __stdcall DeleteThread(void *Param) {

	while (g_bServerAlive) {

		/*EnterCriticalSection(&g_cs);

		if(!g_dwList.empty()) 
			g_dwList.pop_back();
			
		LeaveCriticalSection(&g_cs);
*/
		WaitForSingleObject(GetCurrentThread(), 333);
	}

	return 0;
}

unsigned int __stdcall WorkerThread(void *Param) {

	while (g_bServerAlive) {

		static DWORD dwTime = timeGetTime();
		srand((unsigned int)dwTime);
		DWORD dwData = rand() % 100 + 1;

		EnterCriticalSection(&g_cs);

		g_dwList.push_back(dwData);

		LeaveCriticalSection(&g_cs);

		WaitForSingleObject(GetCurrentThread(), 1000);
	}

	return 0;
}

unsigned int __stdcall SaveThread(void *Param) {

	while (g_bServerAlive) {

		// 핸들이 들어올때까지 무한 대기
		DWORD dwRet = WaitForSingleObject(g_hSave, 1);
		if(dwRet == WAIT_OBJECT_0) {
			
			// Log 남기기
			printf("Save In\n");
			Log();
		}

		ResetEvent(g_hSave);
	}

	return 0;
}

// ReactorPractice.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <iostream>
#include <process.h>
#include <Windows.h>
#include <string.h>
#include <list>
#include "CRingBuffer.h"

#pragma comment(lib,"winmm")

using namespace std;

#define dfMAX_THREAD		3

// 메시지 타입
#define dfTYPE_ADD_STR		0
#define dfTYPE_DEL_STR		1
#define dfTYPE_PRINT_LIST	2
#define dfTYPE_QUIT			3	

// 메시지 구조
struct st_MSG_HEAD
{
	short shType;
	WCHAR String[50];
};

// 전역변수
list<wstring>g_List;
CRingBuffer g_MsgQ(50000);
SRWLOCK g_srwlock;
HANDLE g_hWork;
bool g_bServerAlive = true;
bool g_bKillThread = false;
unsigned int __stdcall WorkerThread(void *param);

int main()
{
	timeBeginPeriod(1);

	setlocale(LC_ALL, " ");

	WCHAR wstr[] = L"ILOVEYOU";

	HANDLE	hThreadHand[dfMAX_THREAD];
	DWORD	dwThreadID[dfMAX_THREAD] = { 0, 1, 2};

	g_hWork = CreateEvent(NULL, true, false, NULL);

	for (int i = 0; i < dfMAX_THREAD; ++i)
		hThreadHand[i] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, NULL, 0, (unsigned int *)&dwThreadID[i]);

	st_MSG_HEAD MsgHead;

	for (;;) {

		// 1. 메시지 생성
		int iType = rand() % 3;
		int iSize = rand() % 8 + 1;
		MsgHead.shType = iType;
		wcsncpy_s(MsgHead.String, 50, wstr, iSize);

		// 2. 스페이스 키가 눌렸다면 1번 무시하고 종료 메시지 넣기
		if (GetAsyncKeyState(VK_ESCAPE) & 0x0001) {

			MsgHead.shType = dfTYPE_QUIT;
		}

		// 3. 위에서 만든 메시지 큐에 넣고 워커스레드 깨우기
		int EnqSize = g_MsgQ.GetEnqSize();
		int MsgSize = sizeof(st_MSG_HEAD);
		int iInputSize = 0;
		if (EnqSize < MsgSize)
			iInputSize = EnqSize;
		else
			iInputSize = MsgSize;
		g_MsgQ.Lock();
		g_MsgQ.Enqueue((char *)&MsgHead, iInputSize);
		g_MsgQ.Unlock();

		SetEvent(g_hWork);

		DWORD dwRet = WaitForMultipleObjects(dfMAX_THREAD, hThreadHand, true, 0);
		if (dwRet != WAIT_TIMEOUT) {
			break;
		}
	}

	for (int i = 0; i < dfMAX_THREAD; ++i)
		CloseHandle(hThreadHand[i]);

	timeEndPeriod(1);

	return 0;
}

unsigned int __stdcall WorkerThread(void *param) {

	DWORD ThreadID = GetThreadId(GetCurrentThread());
	printf("Run WorkerThreadID : %d\n",ThreadID);

	while (!g_bKillThread) {

		DWORD dwRet = WaitForSingleObject(g_hWork, 1);
		if (dwRet != WAIT_TIMEOUT) {

			//bool bKillThread = false;

			st_MSG_HEAD MsgHead;
			memset(&MsgHead, 0, sizeof(MsgHead));
			
			// 1. 메시지 큐에서 메시지 뽑음
			g_MsgQ.Lock();
			
			// 큐에 메시지가 남았다면,
			bool isEmpty = g_MsgQ.isEmpty();
			if(!isEmpty)
				g_MsgQ.Dequeue((char *)&MsgHead, sizeof(MsgHead));
			
			// 큐에 메시지가 없다면,
			else {
				g_MsgQ.Unlock();
				ResetEvent(g_hWork);
				continue;
			}

			switch (MsgHead.shType) {

			case dfTYPE_ADD_STR:
			{
				g_List.push_back(MsgHead.String);
			}
			break;
			case dfTYPE_DEL_STR:
			{
				if (!g_List.empty())
					g_List.pop_front();
			}
			break;
			case dfTYPE_PRINT_LIST:
			{
				list<wstring>::iterator iter;
				list<wstring>::iterator iterEnd = g_List.end();
				for (iter = g_List.begin(); iter != iterEnd; ++iter) {

					//wprintf(L"[%s]", *iter);
					wcout << L"[" <<iter->c_str() << L"]";
				}
				wprintf(L"\n");
			}
			break;
			case dfTYPE_QUIT:
			{
				//bKillThread = true;
				g_bKillThread = true;
			}
			break;
			}

			// UnLock
			g_MsgQ.Unlock();

			// 스레드 break;
			//if (bKillThread)
			//	break;
		}
	}

	printf("Dead WorkerThreadID : %d\n", ThreadID);

	return 0;
}
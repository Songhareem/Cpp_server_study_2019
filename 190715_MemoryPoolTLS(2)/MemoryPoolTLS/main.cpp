#include <iostream>
#include <tchar.h>
#include <process.h>
#include "MemoryPoolTLS.h"
#include "CMemoryPool.h"
#include <list>

using namespace std;


class Test
{
public:
	Test()
	{
		buf = new char[5];
	}

	~Test()
	{
		delete buf;
	}

	char* buf;
};

list<Test*> testList;
CMemoryPoolTLS<Test> pMemoryPoolTLS;
SRWLOCK srw;

unsigned __stdcall Thread1(LPVOID param)
{
	Test* p = nullptr;
	bool ssFlag = false;

	while (1)
	{
		p = pMemoryPoolTLS.Alloc();
		//p = new Test;
		AcquireSRWLockExclusive(&srw);
		testList.push_front(p);
		ReleaseSRWLockExclusive(&srw);
		
		if (GetAsyncKeyState(VK_F4) & 0x0001)
		{
			printf("Alloc Thread Stop / Use : %d / Alloc : %d / Free : %d\n",pMemoryPoolTLS.GetUsedChunkCount(),pMemoryPoolTLS.GetAllocChunkCount(),pMemoryPoolTLS.GetFreeChunkCount());
			ssFlag = true;
		}

		while (ssFlag)
		{
			if (GetAsyncKeyState(VK_F4) & 0x0001)
			{
				printf("Alloc Thread Restart\n");
				ssFlag = false;
			}
			Sleep(10);
		}

		//Sleep(100);
	}

	return 0;
}

unsigned __stdcall Thread2(LPVOID param)
{
	Test* p = nullptr;
	bool ssFlag = false;

	while (1)
	{
		if (testList.empty()) continue;
		AcquireSRWLockExclusive(&srw);
		p = testList.front();
		testList.pop_front();
		ReleaseSRWLockExclusive(&srw);
		pMemoryPoolTLS.Free(p);
		
		
		if (GetAsyncKeyState(VK_F6) & 0x0001)
		{
			printf("Free Thread Stop / Use : %d / Alloc : %d / Free : %d\n", pMemoryPoolTLS.GetUsedChunkCount(), pMemoryPoolTLS.GetAllocChunkCount(), pMemoryPoolTLS.GetFreeChunkCount());
			ssFlag = true;
		}

		while (ssFlag)
		{
			if (GetAsyncKeyState(VK_F6) & 0x0001)
			{
				printf("Free Thread Restart\n");
				ssFlag = false;
			}
			Sleep(10);
		}

		//Sleep(100);
	}

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hThread1, hThread2;
	DWORD dwThreadID;

	InitializeSRWLock(&srw);

	hThread1 = (HANDLE)_beginthreadex(NULL, 0, Thread1, 0, 0, (unsigned int*)& dwThreadID);
	hThread2 = (HANDLE)_beginthreadex(NULL, 0, Thread2, 0, 0, (unsigned int*)& dwThreadID);


	for (;;)
	{
		//printf("Chunk : %d\n",pMemoryPoolTLS->GetUsedChunkCount());
		if(GetAsyncKeyState(VK_F1)&0x0001)
			printf("Test List Size : %d / Chunk Use Size : %d\n", testList.size(),pMemoryPoolTLS.GetUsedChunkCount());
		Sleep(1000);
	}

	return 0;
}
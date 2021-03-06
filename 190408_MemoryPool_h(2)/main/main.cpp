//
// 연달아서 포인터 사용해서 메모리 할당후 -> 주소값 이동 -> 메모리 할당시 연결이 안되는지..?
//

#include "pch.h"
#include <iostream>
#include "CMemoryPool.h"
#include <Windows.h>
#include <list>

#pragma comment (lib,"winmm.lib")

using namespace std;

void mallocNewTest();
void placementNewTest();
void freeFuncPreTest();
void placementNewfuncTest();
void gapTimeTest();
void ListTest();

// for gapTimeTest
#define dfMAX_MEM	10000

int main()
{
	placementNewfuncTest();
	
	//ListTest();
	return 0;
}

void mallocNewTest() {

	//short *p = (short *)new char[sizeof(short) + sizeof(int)];
	short *p = (short *)malloc(sizeof(short) + sizeof(int));

	short *pch = p;
	char *pOrigin = (char *)p;

	for (int i = 0; i < 3; i++)
		*(pch + i) = i + 1;

	int breakPoint_1 = 0;

	//delete[] pOrigin;
	free(p);

	int breakPoint_2 = 0 ;
}

void placementNewTest() {

	class ForTest {

	public:
		ForTest() {
			a = 10;
			printf("create!\n");
		}
		~ForTest() {

			printf("destroy!\n");
		}

		short a;
	};

	/*
	char *p = (char *)malloc(sizeof(short) + sizeof(ForTest));
	char *pMove = p;
	pMove += sizeof(short);
	*/

	short *p = (short *)malloc(sizeof(short) + sizeof(ForTest));
	short *pMove = (short *)p;
	pMove += 1;

	ForTest *pTest = (ForTest *)pMove;

	new(pTest) ForTest();

	pTest -> ~ForTest();

	free(p);
}

void freeFuncPreTest() {

	short *p = (short *)malloc(sizeof(short) + sizeof(int));
	short *pNext = p;
	pNext += 1;
	int *pData = (int *)pNext;

	*pData = 0xf0f0f0f0;

	short *pRollback = (short *)pData;
	pRollback -= 1;
	*pRollback = 0x5a5a;
	free(pRollback);
	int breakPoint = 0;
}

void placementNewfuncTest() {

	class ForTest {

	public:
		ForTest() {
			
			printf("create!\n");
		}
		~ForTest() {

			printf("destroy!\n");
		}

		short a;
	};

	// 일반 변수 테스트 
	CMemoryPool<ForTest> MemPool(5, true);

	ForTest *pi[10];

	// 메모리 할당
	for (int i = 0; i < 7; i++) {
		pi[i] = MemPool.Alloc();
		(pi[i])->a = i + 1;
	}

	// 메모리 회수
	for (int i = 0; i < 7; i++) {
		MemPool.Free(pi[i]);
	}

	printf("\n\n");

	for (int i = 0; i < 5; i++) {

		printf("%d\n", (pi[i])->a);
	}

	printf("\n\n");

	printf("AllocCount : %d\n", MemPool.GetAllocCount());
	printf("UseCount : %d\n", MemPool.GetUseCount());
	printf("FreeCount : %d\n", MemPool.GetFreeCount());

	printf("\n\n");

	//
	/*for (int i = 0; i < 7; i++) {
		pi[i] = MemPool.Alloc();
	}*/

	for (int i = 0; i < 5; i++) {
		MemPool.Free(pi[i]);
	}

	printf("\n\n");

	printf("AllocCount : %d\n", MemPool.GetAllocCount());
	printf("UseCount : %d\n", MemPool.GetUseCount());
	printf("FreeCount : %d\n", MemPool.GetFreeCount());

	printf("\n\n");

	for (int i = 0; i < 7; i++) {
		MemPool.Free(pi[i]);
	}

	MemPool.~CMemoryPool();
}

void gapTimeTest() {

	class ForTest {

	public:
		ForTest() {
			a = 10;
			printf("create!\n");
		}
		~ForTest() {

			printf("destroy!\n");
		}

		short a;
	};

	timeBeginPeriod(1);

	DWORD time1 = timeGetTime();

	int *pi[dfMAX_MEM];
	for (int i = 0; i < dfMAX_MEM; i++)
		pi[i] = new int;

	for (int i = 0; i < dfMAX_MEM; i++)
		delete pi[i];

	DWORD time2 = timeGetTime();

	printf("new_delete TimeGap : %d\n\n", time2 - time1);

	CMemoryPool<int> MemPool(0, false);

	time1 = timeGetTime();

	int *pi2[dfMAX_MEM];
	for (int i = 0; i < dfMAX_MEM; i++)
		pi2[i] = MemPool.Alloc();

	for (int i = 0; i < dfMAX_MEM; i++)
		MemPool.Free(pi2[i]);

	time2 = timeGetTime();

	printf("Alloc_Free TimeGap : %d\n", time2 - time1);

	timeEndPeriod(1);
}

void ListTest() {

	class ForTest {

	public:
		ForTest() {

			printf("create!\n");
		}
		~ForTest() {

			printf("destroy!\n");
		}

		short a;
	};

	list<ForTest *>testList;

	ForTest *pTest = new ForTest();

	testList.pop_front();
	testList.push_back(pTest);
}
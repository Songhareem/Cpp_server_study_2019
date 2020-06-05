
#include "pch.h"
#include <iostream>
#include <process.h>
#include <Windows.h>
#include "CRingBuffer.h"

CRingBuffer Buff(50);

char src[81] = "1234567890 abcde fghij !@#$%^&*() i love you HelloWorld 1234567890 !@#$%^&*() S2";

unsigned int __stdcall Thread_1(void *arg) {

	for (;;) {

		static int StartPoint = 0;

		int iSize = rand() % 40 + 1;
		int EndPoint = StartPoint + iSize;

		if (EndPoint > 80)
			EndPoint = 80;

		int DiffSize = EndPoint - StartPoint;
		int retval = Buff.Enqueue(src, DiffSize);

		if(retval != 0 )
			StartPoint = EndPoint;

		if (StartPoint == 80)
			StartPoint = 0;
		
		//Sleep(500);
	}
	
	return 0;
}

unsigned int __stdcall Thread_2(void *arg) {

	char buf[81] = { -1, };

	for (;;) {

		int iSize = rand() % 40 + 1;

		int retval = Buff.Dequeue(buf, iSize);

		for (int i = 0; i < retval; i++) {
			if (buf[i] == -1)
				break;
			
			printf("%c", buf[i]);
		}

		//Sleep(500);
	}

	return 0;
}

int main()
{
	HANDLE hand1;
	HANDLE hand2;
	DWORD dwID1;
	DWORD dwID2;
	unsigned int InitFlag = 0;
	DWORD SomeData = 42;
	
	int Size = strlen(src);

	hand1 = (HANDLE)_beginthreadex(NULL, 0, Thread_1, (void *)SomeData, InitFlag, (unsigned int *)&dwID1);
	hand2 = (HANDLE)_beginthreadex(NULL, 0, Thread_2, (void *)SomeData, InitFlag, (unsigned int *)&dwID2);

	for (;;) {

	}

	/*char src[] = "123456789";
	char dest[10];
	char dest2[10];

	for (int i = 0; i < 9; i++) {

		*(&dest[0] + i) = src[i];
	}

	memcpy(dest2, src, 9);
*/
	return 0;
}


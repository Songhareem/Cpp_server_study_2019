// main.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <iostream>
#include "CRingBuffer.h"
#include <time.h>
#include "CLogMaker.h"
#include <Windows.h>

#define MAXSIZE 81

CRingBuffer RecvQ(10);
CLogMaker CLogMaker::Log;

void Test_1();
void Test_2();

int main()
{
	
	//char buf[10] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '10' };

	//int iSize = RecvQ.GetFreeSize();

	////memcpy(RecvQ.GetRearBufferPtr(), buf, 5);
	////RecvQ.MoveRear(5);
	////RecvQ.MoveFront(5);
	//
	//if(iSize <= RecvQ.GetNotBrokenPutSize()) {
	//	memcpy(RecvQ.GetRearBufferPtr(), buf, iSize);
	//	RecvQ.MoveRear(iSize);
	//}
	//else {
	//	memcpy(RecvQ.GetRearBufferPtr(), buf, RecvQ.GetNotBrokenPutSize());
	//	RecvQ.MoveRear(RecvQ.GetNotBrokenPutSize());
	//	memcpy(RecvQ.GetRearBufferPtr(), buf, iSize - RecvQ.GetNotBrokenGetSize());
	//	RecvQ.MoveRear(iSize - RecvQ.GetNotBrokenGetSize());
	//}

	//iSize = RecvQ.GetUseSize();
	//
	//if (iSize <= RecvQ.GetNotBrokenGetSize())
	//	RecvQ.MoveFront(iSize);

	//else {
	//	RecvQ.MoveFront(RecvQ.GetNotBrokenGetSize());
	//	RecvQ.MoveFront(iSize - RecvQ.GetNotBrokenGetSize());
	//}

	Test_1();

	return 0;
}

void Test_1() {

	srand(time(NULL));

	int UseSize = 0;
	int randSize;
	char testContext[MAXSIZE] = "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12S2";
	//printf("%s", testContext);
	//printf("%d", sizeof(testContext));
	char PeekTmp[MAXSIZE] = { 0, };
	char DestTmp[MAXSIZE] = { 0, };

	for (;;) {
		randSize = rand() % (MAXSIZE - UseSize) + 1;

		RecvQ.Enqueue(&testContext[UseSize], randSize);
		RecvQ.Peek(PeekTmp, randSize);
		randSize = RecvQ.Dequeue(DestTmp, randSize);

		if (memcmp(PeekTmp, DestTmp, randSize) == 0) {
			printf("%s", DestTmp);
			memset(PeekTmp, 0, MAXSIZE);
			memset(DestTmp, 0, MAXSIZE);
		}
		/*else
			CLogMaker::GetInstance()->writeLog((char *)"Peek != Dest");
*/
		UseSize += randSize;
		if (UseSize == MAXSIZE)
			UseSize = 0;

		Sleep(10);
	}
}

void Test_2() {

	char buf[10];
	char buf2[10];

	for (int i = 0; i < 10; i++) {
		buf[i] = i + 1;
		printf("%d\n", buf[i]);
	}

	int FullSize = 4;
	RecvQ.Enqueue(&buf[0], FullSize);
	RecvQ.Dequeue(&buf2[0], FullSize);

	//printf("\nsave\n");

	/*memset(buf2, 0, sizeof(buf2));

	int NotBrokenSize1 = RecvQ.GetNotBrokenPutSize();
	printf("NotBrokenSize1 : %d\n", NotBrokenSize1);

	char *pRear = RecvQ.GetRearBufferPtr();
	memcpy(pRear, &buf[FullSize], NotBrokenSize1);
	RecvQ.MoveRear(NotBrokenSize1);

	int NotBrokenSize2 = RecvQ.GetNotBrokenPutSize();
	printf("NotBrokenSize2 : %d\n", NotBrokenSize2);

	pRear = RecvQ.GetRearBufferPtr();
	memcpy(pRear, &buf[FullSize + NotBrokenSize1], NotBrokenSize2);
	RecvQ.MoveRear(NotBrokenSize2);


	RecvQ.Peek(&buf2[0], NotBrokenSize1);
	RecvQ.MoveFront(NotBrokenSize1);

	RecvQ.Peek(&buf2[NotBrokenSize1], NotBrokenSize2);
	RecvQ.MoveFront(NotBrokenSize2);*/

	RecvQ.Enqueue(&buf[FullSize], FullSize);
	RecvQ.Dequeue(&buf2[0], FullSize);
	//RecvQ.Peek(&buf2[0], FullSize);
	//RecvQ.MoveFront(FullSize);

	printf("\nsave\n");

	for (int i = 0; i < 4; i++) {
		printf("%d\n", buf2[i]);
	}
}
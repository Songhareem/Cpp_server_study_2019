
#include "pch.h"
#include <iostream>
#include "CPacket.h"

void EnDequeTest();
void operatorTest();
void SizeOverTest();

int main()
{
	//operatorTest();
	SizeOverTest();

	return 0;
}

void EnDequeTest() {

	CPacket Pack1(10);
	char buf[20];
	char buf2[20] = { 0, };

	for (int i = 0; i < 20; i++) {
		buf[i] = i + 1;
	}

	printf("Before: _Write : %d\n", Pack1.GetWritePos());
	printf("Before: _Read : %d\n", Pack1.GetReadPos());
	printf("Before: _Use : %d\n", Pack1.GetPacketSize());
	printf("Before: _Free : %d\n", Pack1.GetFreeSize());

	Pack1.Enqueue(buf, 10);
	//Pack1.PrintPacket();
	Pack1.Dequeue(buf2, 10);

	printf("After: _Write : %d\n", Pack1.GetWritePos());
	printf("After: _Read : %d\n", Pack1.GetReadPos());
	printf("After: _Use : %d\n", Pack1.GetPacketSize());
	printf("After: _Free : %d\n", Pack1.GetFreeSize());

	for (int i = 0; i < 20; i++) {
		printf("buf2[%d] : %d\n", i, buf2[i]);
	}
}

void operatorTest(){


	CPacket Pack1(sizeof(bool) + sizeof(char) + sizeof(short)
		+ sizeof(int) + sizeof(long) + sizeof(__int64) + sizeof(float) + sizeof(double));

	bool blSrc = true;
	char chSrc = 'a';
	short shSrc = 10;
	int iSrc = 20;
	long lgSrc = 30;
	__int64 i64Src = 40;
	float fSrc = 0.1f;
	double dSrc = 2.2f;

	bool blDest = false;
	char chDest = 0;
	short shDest = 0;
	int iDest = 0;
	long lgDest = 0;
	__int64 i64Dest = 0;
	float fDest = 0;
	double dDest = 0;

	Pack1 << blSrc << chSrc << shSrc << iSrc << lgSrc << i64Src << fSrc << dSrc;
	
	// << test
	printf("Pack1\n");
	Pack1.PrintPacket();
	printf("\n");

	// = test
	CPacket Pack2 = Pack1;
	printf("Pack2\n");
	Pack1.PrintPacket();
	printf("\n");

	Pack1 >> blDest >> chDest >> shDest >> iDest >> lgDest >> i64Dest >> fDest >> dDest;

	// >> test
	printf("save\n");

	printf("%i \n", blDest);
	printf("%c \n",chDest);
	printf("%i \n", shDest);
	printf("%i \n", iDest);
	printf("%i \n", lgDest);
	printf("%lli \n", i64Dest);
	printf("%f \n", fDest);
	printf("%lf \n", dDest);

}

void SizeOverTest() {

	CPacket Pack(sizeof(int));

	memset(Pack.GetBufferPtr(), 0, sizeof(Pack.GetBufferSize()));

	bool Data = true;
	short Data2 = 10;
	char Data3 = 'a';
	char Data4 = 'b';

	Pack << Data << Data2 << Data3 << Data4;

	bool getData = false;
	short getData2 = 0;
	char getData3 = 'p';
	char getData4 = 'p';

	Pack >> getData >> getData2 >> getData3 >> getData4;

	printf("%d\n", getData);
	printf("%d\n", getData2);
	printf("%c\n", getData3);
	printf("%c\n", getData4);

}

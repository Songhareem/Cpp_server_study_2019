//#include "pch.h"
#include "CPacket.h"
#include <iostream> // test용

CPacket::CPacket()
{
	Initial();
}

CPacket::CPacket(int iBufferSize) {

	Initial(iBufferSize);
}

CPacket:: CPacket(const CPacket &clSrcPacket) {

	//CPacket SrcPack = clSrcPacket;
	//int Size = SrcPack.GetBufferSize();

	//// 배열 생성
	//_BufSize = Size;
	//_Buffer = new char[_BufSize];

	//// Data 복사
	//char *pDest = GetBufferPtr();
	//char *pSrc = SrcPack.GetBufferPtr();
	//int SrcWriteSize = SrcPack.GetWritePos();
	//
	//for (int i = 0; i < SrcWriteSize; i++) {

	//	*(pDest + i) = *(pSrc + i);
	//}

	//// Read Write위치 복사
	//_Write = SrcPack.GetWritePos();
	//_Read = SrcPack.GetReadPos();
}


CPacket::~CPacket()
{
	Release();
}
//--------------------
// Protected
//--------------------

// 버퍼 초기화
void CPacket::Initial(int iBufferSize) {

	_Buffer = new char[iBufferSize];
	_BufSize = iBufferSize;
	_Write = _Read = 0;
}

// 버퍼 해제
void CPacket::Release(void) {

	delete _Buffer;
	//printf("소멸자 호출됨\n");
}

//---------------------
// Public 
//---------------------
void CPacket::Clear(void) {

	_Write = _Read = 0;
}

int CPacket::GetBufferSize(void) {

	return _BufSize;
}

int	CPacket::GetPacketSize(void) {

	return _Write - _Read;
}

int	CPacket::GetFreeSize(void) {

	return _BufSize - _Write;
}

int CPacket::GetReadPos(void) {

	return _Read;
}

int CPacket::GetWritePos(void) {

	return _Write;
}

char *CPacket::GetBufferPtr(void) {

	return _Buffer;
}

char *CPacket::GetReadPtr(void) {

	return (_Buffer + _Read);
}

char *CPacket::GetWritePtr(void) {

	return (_Buffer + _Write);
}

// 걸러내는 작업이 이 안에서 필요한건지 생각해볼것
int	CPacket::MoveWritePos(int iSize) {

	if (iSize <= 0)
		return 0;

	int MoveSize;

	if (GetFreeSize() < iSize) {
		_Write += GetFreeSize();
		MoveSize = GetFreeSize();
	}

	else {
		_Write += iSize;
		MoveSize = iSize;
	}
	
	return MoveSize;
}

// 걸러내는 작업이 이 안에서 필요한건지 생각해볼것
int	CPacket::MoveReadPos(int iSize) {

	if (iSize <= 0)
		return 0;

	int MoveSize;

	if (GetPacketSize() < iSize) {
		_Read += GetPacketSize();
		MoveSize = GetPacketSize();
	}

	else {
		_Read += iSize;
		MoveSize = iSize;
	}

	return MoveSize;
}

int	CPacket::Enqueue(char *chpDest, int iSize) {

	if (iSize <= 0 || GetFreeSize() == 0)
		return 0;

	int Size;

	if (GetFreeSize() < iSize)
		Size = GetFreeSize();

	else
		Size = iSize;

	char *pWrite = GetWritePtr();
	
	for (int i = 0; i < Size; i++) {

		*(pWrite+i) = *(chpDest+i);
	}

	_Write += Size;
	return Size;
}

int	CPacket::Dequeue(char *chpDest, int iSize) {

	if (iSize <= 0 || GetPacketSize() == 0)
		return 0;

	int Size;

	if (GetPacketSize() < iSize)
		Size = GetPacketSize();

	else
		Size = iSize;

	char *pRead = GetReadPtr();

	for (int i = 0; i < Size; i++) {

		*(chpDest+i) = *(pRead+i);
	}

	_Read += Size;
	return Size;	
}

void CPacket::PrintPacket(bool isHex, int iSize) {

	int PrintSize = _BufSize;

	if (iSize != NULL)
		PrintSize = iSize;

	if(isHex == true) {
		for (int i = 0; i < PrintSize; i++) {

			printf("[%d] : 0x%02x\n", i,*(_Buffer + i));
		}
	}

	else {

		for (int i = 0; i < PrintSize; i++) {

			printf("[%d] : %d\n", i, *(_Buffer + i));
		}
	}
}

//------------------------------
// 연산자 오버로딩
//------------------------------
// =
CPacket &CPacket::operator = (const CPacket &clSrcPacket) {

	CPacket SrcPack = clSrcPacket;
	int Size = SrcPack.GetBufferSize();

	// 배열 생성
	_BufSize = Size;
	_Buffer = new char[_BufSize];

	// Data 복사
	char *pDest = GetBufferPtr();
	char *pSrc = SrcPack.GetBufferPtr();
	int SrcWriteSize = SrcPack.GetWritePos();

	for (int i = 0; i < SrcWriteSize; i++) {

		*(pDest + i) = *(pSrc + i);
	}

	// Read Write위치 복사
	_Write = SrcPack.GetWritePos();
	_Read = SrcPack.GetReadPos();

	return *this;
}

// 예외처리 return this 대신 Resizing 해야함
//<<
CPacket &CPacket::operator << (bool	blVal) {

	if (GetFreeSize() < sizeof(bool))
		return *this;

	bool *pbool = (bool *)GetWritePtr();
	*pbool = blVal;
	MoveWritePos(sizeof(bool));

	//Enqueue((char *)&blVal, sizeof(bool));

	return *this;
}

CPacket &CPacket::operator << (char	chVal) {

	if (GetFreeSize() < sizeof(char))
		return *this;
	
	char *pchar = (char *)GetWritePtr();
	*pchar = chVal;
	MoveWritePos(sizeof(char));

	//Enqueue(&chVal, sizeof(char));
	
	return *this;
}

CPacket &CPacket::operator << (short shVal) {

	if (GetFreeSize() < sizeof(short))
		return *this;
	
	short *pshort = (short *)GetWritePtr();
	*pshort = shVal;
	MoveWritePos(sizeof(short));
	
	//Enqueue((char *)&shVal, sizeof(short));
	
	return *this;
}

CPacket &CPacket::operator << (int	iVal) {

	if (GetFreeSize() < sizeof(int))
		return *this;

	int *pint = (int *)GetWritePtr();
	*pint = iVal;
	MoveWritePos(sizeof(int));

	//Enqueue((char *)&iVal, sizeof(int));
	
	return *this;
}

CPacket &CPacket::operator << (long	lgVal) {

	if (GetFreeSize() < sizeof(long))
		return *this;

	long *plong = (long *)GetWritePtr();
	*plong = lgVal;
	MoveWritePos(sizeof(long));
	
	//Enqueue((char *)&lgVal, sizeof(long));

	return *this;
}

CPacket &CPacket::operator << (float fVal) {

	if (GetFreeSize() < sizeof(float))
		return *this;

	float *pfloat = (float *)GetWritePtr();
	*pfloat = fVal;
	MoveWritePos(sizeof(float));

	//Enqueue((char *)&fVal, sizeof(float));

	return *this;
}

CPacket &CPacket::operator << (double dVal) {
	
	if (GetFreeSize() < sizeof(double))
		return *this;

	double *pdouble = (double *)GetWritePtr();
	*pdouble = dVal;
	MoveWritePos(sizeof(double));

	//Enqueue((char *)&dVal, sizeof(double));

	return *this;
}

//CPacket &CPacket::operator << (__int8 i8Val) {
//
//	Enqueue((char *)&i8Val, sizeof(__int8));
//
//	return *this;
//}
//
//CPacket &CPacket::operator << (__int16 i16Val) {
//
//	Enqueue((char *)&i16Val, sizeof(__int16));
//
//	return *this;
//}
//
//CPacket &CPacket::operator << (__int32 i32Val) {
//	
//	Enqueue((char *)&i32Val, sizeof(__int32));
//
//	return *this;
//}

CPacket &CPacket::operator << (__int64 i64Val) {

	if (GetFreeSize() < sizeof(__int64))
		return *this;

	__int64 *pint64 = (__int64 *)GetWritePtr();
	*pint64 = i64Val;
	MoveWritePos(sizeof(__int64));

	//Enqueue((char *)&i64Val, sizeof(__int64));

	return *this;
}

CPacket &CPacket::operator << (unsigned char uchVal) {

	if (GetFreeSize() < sizeof(unsigned char))
		return *this;

	unsigned char *puchar = (unsigned char *)GetWritePtr();
	*puchar = uchVal;
	MoveWritePos(sizeof(unsigned char));

	//Enqueue((char *)&uchVal, sizeof(unsigned char));

	return *this;
}

CPacket &CPacket::operator << (unsigned short ushVal) {
	
	if (GetFreeSize() < sizeof(unsigned short))
		return *this;

	unsigned short *pushort = (unsigned short *)GetWritePtr();
	*pushort = ushVal;
	MoveWritePos(sizeof(unsigned short));

	//Enqueue((char *)&ushVal, sizeof(unsigned short));

	return *this;
}

CPacket &CPacket::operator << (unsigned int uiVal) {

	if (GetFreeSize() < sizeof(unsigned int))
		return *this;

	unsigned int *puint = (unsigned int *)GetWritePtr();
	*puint = uiVal;
	MoveWritePos(sizeof(unsigned int));
	
	//Enqueue((char *)&uiVal, sizeof(unsigned int));

	return *this;
}

CPacket &CPacket::operator << (unsigned long ulgVal) {

	if (GetFreeSize() < sizeof(unsigned long))
		return *this;

	unsigned long *pulong = (unsigned long *)GetWritePtr();
	*pulong = ulgVal;
	MoveWritePos(sizeof(unsigned long));
	
	//Enqueue((char *)&ulgVal, sizeof(unsigned long));

	return *this;
}

//CPacket &CPacket::operator << (unsigned __int64 ui64Val) {
//
//	if (GetFreeSize() < sizeof(unsigned __int64))
//		return *this;
//
//	unsigned __int64 *puint64 = (unsigned __int64 *)GetWritePtr();
//	*puint64 = ui64Val;
//	MoveWritePos(sizeof(unsigned __int64));
//
//	//Enqueue((char *)&i64Val, sizeof(__int64));
//
//	return *this;
//}

// 예외처리 해야함 throw 던지기 (예외객체 만들기)
//>>
CPacket &CPacket::operator >> (bool &blVal) {

	if (GetPacketSize() < sizeof(bool))
		return *this;

	bool *pbool = (bool *)GetReadPtr();
	blVal = *pbool;
	MoveReadPos(sizeof(bool));

	//Dequeue((char *)&blVal, sizeof(bool));

	return *this;
}

CPacket &CPacket::operator >> (char	&chVal) {

	if (GetPacketSize() < sizeof(char))
		return *this;

	char *pchar = (char *)GetReadPtr();
	chVal = *pchar;
	MoveReadPos(sizeof(char));

	//Dequeue(&chVal, sizeof(char));

	return *this;
}

CPacket &CPacket::operator >> (short &shVal) {

	if (GetPacketSize() < sizeof(short))
		return *this;

	short *pshort = (short *)GetReadPtr();
	shVal = *pshort;
	MoveReadPos(sizeof(short));

	//Dequeue((char *)&shVal, sizeof(short));

	return *this;
}

CPacket &CPacket::operator >> (int &iVal) {

	if (GetPacketSize() < sizeof(int))
		return *this;

	int *pint = (int *)GetReadPtr();
	iVal = *pint;
	MoveReadPos(sizeof(int));

	//Dequeue((char *)&iVal, sizeof(int));

	return *this;
}

CPacket &CPacket::operator >> (long &lgVal) {

	if (GetPacketSize() < sizeof(long))
		return *this;

	long *plong = (long *)GetReadPtr();
	lgVal = *plong;
	MoveReadPos(sizeof(long));
	
	//Dequeue((char *)&lgVal, sizeof(long));

	return *this;
}

CPacket &CPacket::operator >> (float &fVal) {

	if (GetPacketSize() < sizeof(float))
		return *this;

	float *pfloat = (float *)GetReadPtr();
	fVal = *pfloat;
	MoveReadPos(sizeof(float));
	
	//Dequeue((char *)&fVal, sizeof(float));

	return *this;
}

CPacket &CPacket::operator >> (double &dVal) {

	if (GetPacketSize() < sizeof(double))
		return *this;

	double *pdouble = (double *)GetReadPtr();
	dVal = *pdouble;
	MoveReadPos(sizeof(double));
	
	//Dequeue((char *)&dVal, sizeof(double));

	return *this;
}

//CPacket &CPacket::operator >> (__int8 &i8Val) {
//
//	Dequeue((char *)&i8Val, sizeof(__int8));
//
//	return *this;
//}
//
//CPacket &CPacket::operator >> (__int16 &i16Val) {
//
//	Dequeue((char *)&i16Val, sizeof(__int16));
//
//	return *this;
//}
//
//CPacket &CPacket::operator >> (__int32 &i32Val) {
//
//	Dequeue((char *)&i32Val, sizeof(__int32));
//
//	return *this;
//}

CPacket &CPacket::operator >> (__int64 &i64Val) {

	if (GetPacketSize() < sizeof(__int64))
		return *this;

	__int64 *pint64 = (__int64 *)GetReadPtr();
	i64Val = *pint64;
	MoveReadPos(sizeof(__int64));
	
	//Dequeue((char *)&i64Val, sizeof(__int64));

	return *this;
}

CPacket &CPacket::operator >> (unsigned char &uchVal) {

	if (GetPacketSize() < sizeof(unsigned char))
		return *this;

	unsigned char *puchar = (unsigned char *)GetReadPtr();
	uchVal = *puchar;
	MoveReadPos(sizeof(unsigned char));
	
	//Enqueue((char *)&uchVal, sizeof(unsigned char));

	return *this;
}

CPacket &CPacket::operator >> (unsigned short &ushVal) {

	if (GetPacketSize() < sizeof(unsigned short))
		return *this;

	unsigned short *pushort = (unsigned short *)GetReadPtr();
	ushVal = *pushort;
	MoveReadPos(sizeof(unsigned short));
	
	//Enqueue((char *)&ushVal, sizeof(unsigned short));

	return *this;
}

CPacket &CPacket::operator >> (unsigned int &uiVal) {

	if (GetPacketSize() < sizeof(unsigned int))
		return *this;

	unsigned int *puint = (unsigned int *)GetReadPtr();
	uiVal = *puint;
	MoveReadPos(sizeof(unsigned int));
	
	//Enqueue((char *)&uiVal, sizeof(unsigned int));

	return *this;
}

CPacket &CPacket::operator >> (unsigned long &ulgVal) {

	if (GetPacketSize() < sizeof(unsigned long))
		return *this;

	unsigned long *pulong = (unsigned long *)GetReadPtr();
	ulgVal = *pulong;
	MoveReadPos(sizeof(unsigned long));
	
	//Enqueue((char *)&ulgVal, sizeof(unsigned long));

	return *this;
}

//CPacket &CPacket::operator >> (unsigned __int64 &ui64Val) {
//
//	if (GetPacketSize() < sizeof(unsigned __int64))
//		return *this;
//
//	unsigned __int64 *puint64 = (unsigned __int64 *)GetReadPtr();
//	ui64Val = *puint64;
//	MoveReadPos(sizeof(unsigned __int64));
//
//	//Dequeue((char *)&i64Val, sizeof(__int64));
//
//	return *this;
//}
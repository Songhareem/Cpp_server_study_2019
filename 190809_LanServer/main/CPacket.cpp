#include "CPacket.h"
#include <iostream> // test용

CMemoryPoolTLS<CPacket> CPacket::_packetPoolTls;

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

void CPacket::InitHeaderOption(bool bNetOption)
{
	_bHeaderOpt = bNetOption;
	
	if (bNetOption == dfEXTERNAL_NET)
		_iHead = 0;

	else
		_iHead = 3;
}

bool CPacket::SetHeader(char * pHeader)
{
	if (pHeader == NULL) {
		
		return false;
	}

	char *pHead = _pBuffer + _iHead;

	for (int i = 0; i < eHEADER_MAX_SIZE - _iHead; i++)
		*(pHead + i) = *(pHeader + i);

	return true;
}

//--------------------
// Protected
//--------------------

// 버퍼 초기화
void CPacket::Initial(int iBufferSize) {

	_pBuffer = new char[iBufferSize];
	_iBufSize = iBufferSize;
	_iWrite = _iRead = en_PACKET::eHEADER_MAX_SIZE;

	InitHeaderOption();
}

// 버퍼 해제
void CPacket::Release(void) {

	delete _pBuffer;
	printf("소멸자 호출됨\n");
}

//---------------------
// Public 
//---------------------
void CPacket::Clear(void) {

	_iWrite = _iRead = 5;
}

int CPacket::GetBufferSize(void) {

	return _iBufSize;
}

int	CPacket::GetPayloadSize(void) {

	return _iWrite - _iRead;
}

int CPacket::GetPacketSize(void)
{
	return _iWrite - _iHead;
}

int	CPacket::GetFreeSize(void) {

	return _iBufSize - _iWrite;
}

int CPacket::GetReadPos(void) {

	return _iRead;
}

int CPacket::GetWritePos(void) {

	return _iWrite;
}

char *CPacket::GetBufferPtr(void) {

	return _pBuffer;
}

char * CPacket::GetHeadPtr(void)
{
	char *pHead = _pBuffer + _iHead;

	return pHead;
}

char *CPacket::GetReadPtr(void) {

	return (_pBuffer + _iRead);
}

char *CPacket::GetWritePtr(void) {

	return (_pBuffer + _iWrite);
}

// 걸러내는 작업이 이 안에서 필요한건지 생각해볼것
int	CPacket::MoveWritePos(int iSize) {

	if (iSize <= 0)
		return 0;

	int MoveSize;

	if (GetFreeSize() < iSize) {
		_iWrite += GetFreeSize();
		MoveSize = GetFreeSize();
	}

	else {
		_iWrite += iSize;
		MoveSize = iSize;
	}
	
	return MoveSize;
}

// 걸러내는 작업이 이 안에서 필요한건지 생각해볼것
int	CPacket::MoveReadPos(int iSize) {

	if (iSize <= 0)
		return 0;

	int MoveSize;

	if (GetPayloadSize() < iSize) {
		_iRead += GetPayloadSize();
		MoveSize = GetPayloadSize();
	}

	else {
		_iRead += iSize;
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

	_iWrite += Size;
	return Size;
}

int	CPacket::Dequeue(char *chpDest, int iSize) {

	if (iSize <= 0 || GetPayloadSize() == 0)
		return 0;

	int Size;

	if (GetPayloadSize() < iSize)
		Size = GetPayloadSize();

	else
		Size = iSize;

	char *pRead = GetReadPtr();

	for (int i = 0; i < Size; i++) {

		*(chpDest+i) = *(pRead+i);
	}

	_iRead += Size;
	return Size;	
}

long CPacket::RefIncrement()
{
	return InterlockedIncrement(&_lRefCnt);
}

long CPacket::RefDecrement()
{
	return InterlockedDecrement(&_lRefCnt);
}

void CPacket::PrintPacket() {

	for (int i = 0; i < _iBufSize; i++) {

		printf("%d\n", *(_pBuffer + i));
	}
}

CPacket * CPacket::Alloc()
{
	return _packetPoolTls.Alloc();
}

bool CPacket::Free(CPacket * pPack)
{
	if(pPack->RefDecrement() == 0) {
		_packetPoolTls.Free(pPack);
		return true;
	}
	return false;
}

//------------------------------
// 연산자 오버로딩
//------------------------------
// =
CPacket &CPacket::operator = (const CPacket &clSrcPacket) {

	CPacket SrcPack = clSrcPacket;
	int Size = SrcPack.GetBufferSize();

	// 배열 생성
	_iBufSize = Size;
	_pBuffer = new char[_iBufSize];

	// Data 복사
	char *pDest = GetBufferPtr();
	char *pSrc = SrcPack.GetBufferPtr();
	int SrcWriteSize = SrcPack.GetWritePos();

	for (int i = 0; i < SrcWriteSize; i++) {

		*(pDest + i) = *(pSrc + i);
	}

	// Read Write위치 복사
	_iWrite = SrcPack.GetWritePos();
	_iRead = SrcPack.GetReadPos();

	return *this;
}

// 예외처리 해야함!
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

//>>
CPacket &CPacket::operator >> (bool &blVal) {

	if (GetPayloadSize() < sizeof(bool))
		return *this;

	bool *pbool = (bool *)GetReadPtr();
	blVal = *pbool;
	MoveReadPos(sizeof(bool));

	//Dequeue((char *)&blVal, sizeof(bool));

	return *this;
}

CPacket &CPacket::operator >> (char	&chVal) {

	if (GetPayloadSize() < sizeof(char))
		return *this;

	char *pchar = (char *)GetReadPtr();
	chVal = *pchar;
	MoveReadPos(sizeof(char));

	//Dequeue(&chVal, sizeof(char));

	return *this;
}

CPacket &CPacket::operator >> (short &shVal) {

	if (GetPayloadSize() < sizeof(short))
		return *this;

	short *pshort = (short *)GetReadPtr();
	shVal = *pshort;
	MoveReadPos(sizeof(short));

	//Dequeue((char *)&shVal, sizeof(short));

	return *this;
}

CPacket &CPacket::operator >> (int &iVal) {

	if (GetPayloadSize() < sizeof(int))
		return *this;

	int *pint = (int *)GetReadPtr();
	iVal = *pint;
	MoveReadPos(sizeof(int));

	//Dequeue((char *)&iVal, sizeof(int));

	return *this;
}

CPacket &CPacket::operator >> (long &lgVal) {

	if (GetPayloadSize() < sizeof(long))
		return *this;

	long *plong = (long *)GetReadPtr();
	lgVal = *plong;
	MoveReadPos(sizeof(long));
	
	//Dequeue((char *)&lgVal, sizeof(long));

	return *this;
}

CPacket &CPacket::operator >> (float &fVal) {

	if (GetPayloadSize() < sizeof(float))
		return *this;

	float *pfloat = (float *)GetReadPtr();
	fVal = *pfloat;
	MoveReadPos(sizeof(float));
	
	//Dequeue((char *)&fVal, sizeof(float));

	return *this;
}

CPacket &CPacket::operator >> (double &dVal) {

	if (GetPayloadSize() < sizeof(double))
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

	if (GetPayloadSize() < sizeof(__int64))
		return *this;

	__int64 *pint64 = (__int64 *)GetReadPtr();
	i64Val = *pint64;
	MoveReadPos(sizeof(__int64));
	
	//Dequeue((char *)&i64Val, sizeof(__int64));

	return *this;
}

CPacket &CPacket::operator >> (unsigned char &uchVal) {

	if (GetPayloadSize() < sizeof(unsigned char))
		return *this;

	unsigned char *puchar = (unsigned char *)GetReadPtr();
	uchVal = *puchar;
	MoveReadPos(sizeof(unsigned char));
	
	//Enqueue((char *)&uchVal, sizeof(unsigned char));

	return *this;
}

CPacket &CPacket::operator >> (unsigned short &ushVal) {

	if (GetPayloadSize() < sizeof(unsigned short))
		return *this;

	unsigned short *pushort = (unsigned short *)GetReadPtr();
	ushVal = *pushort;
	MoveReadPos(sizeof(unsigned short));
	
	//Enqueue((char *)&ushVal, sizeof(unsigned short));

	return *this;
}

CPacket &CPacket::operator >> (unsigned int &uiVal) {

	if (GetPayloadSize() < sizeof(unsigned int))
		return *this;

	unsigned int *puint = (unsigned int *)GetReadPtr();
	uiVal = *puint;
	MoveReadPos(sizeof(unsigned int));
	
	//Enqueue((char *)&uiVal, sizeof(unsigned int));

	return *this;
}

CPacket &CPacket::operator >> (unsigned long &ulgVal) {

	if (GetPayloadSize() < sizeof(unsigned long))
		return *this;

	unsigned long *pulong = (unsigned long *)GetReadPtr();
	ulgVal = *pulong;
	MoveReadPos(sizeof(unsigned long));
	
	//Enqueue((char *)&ulgVal, sizeof(unsigned long));

	return *this;
}

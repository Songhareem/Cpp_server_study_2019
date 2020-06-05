
//#include "pch.h"
#include "CRingBuffer.h"
#include <iostream>

CRingBuffer::CRingBuffer(void) {

	_pBuffer = NULL;

	Initial(10000);
}

CRingBuffer::CRingBuffer(int iBufferSize) {

	Initial(iBufferSize);
}

CRingBuffer::~CRingBuffer(void) {

	delete _pBuffer;
}

void CRingBuffer::Initial(int iBufferSize) {

	if (iBufferSize <= 0)
		return;

	if (_pBuffer != NULL)
		delete _pBuffer;

	_BufferSize = iBufferSize + 1;
	_pBuffer = new char[_BufferSize];
	_Front = _Rear = 0;

	InitializeSRWLock(&_SRWLock);
}

/////////////////////////////////////////////////////////////////////////
// ������ ������ ���. ( ���� byte ���� )
//
// Parameters: ����.
// Return: (int)����� �뷮 + 1
/////////////////////////////////////////////////////////////////////////
int	CRingBuffer::GetBufferSize(void) {

	if (_pBuffer != NULL)
		return _BufferSize;

	return 0;
}

/////////////////////////////////////////////////////////////////////////
// ���� ������� �뷮 ���.
//
// Parameters: ����.
// Return: (int)������� �뷮.
/////////////////////////////////////////////////////////////////////////
int	CRingBuffer::GetUseSize(void) {

	int iRear = _Rear;
	int iFront = _Front;

	if (iRear < iFront)
		return _BufferSize - (iFront - iRear);

	return iRear - iFront;
}

/////////////////////////////////////////////////////////////////////////
// ���� ���ۿ� ���� �뷮 ���. 
//
// Parameters: ����.
// Return: (int)�����뷮.
/////////////////////////////////////////////////////////////////////////
int	CRingBuffer::GetFreeSize(void) {

	int iRealSize = _BufferSize - 1;
	int iUseSize = GetUseSize();
	return iRealSize - iUseSize;
}

/////////////////////////////////////////////////////////////////////////
// ���� �����ͷ� �ܺο��� �ѹ濡 �а�, �� �� �ִ� ����.
// (������ ���� ����)
//
// ���� ť�� ������ ������ ���ܿ� �ִ� �����ʹ� �� -> ó������ ���ư���
// 2���� �����͸� ��ų� ���� �� ����. �� �κп��� �������� ���� ���̸� �ǹ�
//
// Parameters: ����.
// Return: (int)��밡�� �뷮.
////////////////////////////////////////////////////////////////////////
int	CRingBuffer::GetDeqSize(void) {

	int Size = _BufferSize - _Front;
	int UseSize = GetUseSize();

	if (UseSize > Size) {

		return Size;
	}

	return UseSize;

	//if (_Rear > _Front)
	//	return _Rear - _Front;
	//else //if (_Rear < _Front)
	//	return _BufferSize - _Front;
}

int	CRingBuffer::GetEnqSize(void) {

	int MaxSize = _BufferSize;
	int iRet = 0;
	int iFront = _Front;
	int iRear = _Rear;

	if (iFront == 0)
		MaxSize -= 1;

	if (iRear >= iFront)
		iRet = MaxSize - iRear;

	else //if (_Rear < _Front)
		iRet = iFront - iRear - 1;

	if (iRet < 0) // -1 ������ ��찡 ����
		return 0;

	return iRet;
}

/////////////////////////////////////////////////////////////////////////
// WritePos �� ����Ÿ ����.
//
// Parameters: (char *)����Ÿ ������. (int)ũ��. 
// Return: 1(false), 0(succeed)
/////////////////////////////////////////////////////////////////////////

//bool CRingBuffer::Enqueue(char *chpData, int iSize) {
//	int Check = _Rear + 1;
//	if (Check > _BufferSize - 1)
//	Check -= _BufferSize;
//
//	if (Check == _Front)
//	return false;
//
//	int FreeSize = GetFreeSize();
//	if (FreeSize < iSize)
//		return false;
//
//	int NotBrokenPutSize = GetNotBrokenPutSize();
//	if (NotBrokenPutSize >= iSize)
//	memcpy(&_pBuffer[_Rear], chpData, iSize);
//	else {
//		memcpy(&_pBuffer[_Rear], chpData, NotBrokenPutSize);
//		memcpy(&_pBuffer[0], &chpData[NotBrokenPutSize], iSize - NotBrokenPutSize);
//	}
//
//	MoveRear(iSize);
//	return true;
//}

int CRingBuffer::Enqueue(char *chpData, int iSize) {

	if (iSize <= 0)
		return 0;

	char *pRear = GetRearBufferPtr();
	char *pBuff = GetBufferPtr();

	int iFreeSize = 0;
	int iEnqSize = 0;

	GetFreeEnqSize(&iFreeSize, &iEnqSize);

	if (iFreeSize <= 0)
		return 0;

	int iLen = iSize;
	if (iFreeSize < iSize)
		iLen = iFreeSize;

	if (iEnqSize >= iLen) {
		memcpy(pRear, chpData, iLen);
	}
	else {
		memcpy(pRear, chpData, iEnqSize);
		memcpy(pBuff, &chpData[iEnqSize], iLen - iEnqSize);
	}

	MoveRear(iLen);
	return iLen;
}

/////////////////////////////////////////////////////////////////////////
// ReadPos ���� ����Ÿ ������. ReadPos �̵�.
//
// Parameters: (char *)����Ÿ ������. (int)ũ��.
// Return: 1(false), 0(succeed)
/////////////////////////////////////////////////////////////////////////

//bool CRingBuffer::Dequeue(char *chpDest, int iSize) {
//	int Check = _Front;
//
//	if (Check == _Rear)
//	return false;
//
//	int UseSize = GetUseSize();
//	if (UseSize < iSize)
//		return false;
//
//	int NotBrokenGetSize = GetNotBrokenGetSize();
//	if (NotBrokenGetSize >= iSize)
//	memcpy(chpDest, &_pBuffer[_Front], iSize);
//	else {
//		memcpy(chpDest, &_pBuffer[_Front], NotBrokenGetSize);
//		memcpy(&chpDest[NotBrokenGetSize], &_pBuffer[0], iSize - NotBrokenGetSize);
//	}
//	MoveFront(iSize);
//	return true;
//}
int CRingBuffer::Dequeue(char *chpDest, int iSize) {

	if (iSize <= 0)
		return 0;

	char *pFront = GetFrontBufferPtr();
	char *pBuff = GetBufferPtr();

	int iLen = iSize;
	int iUseSize = 0;
	int iDeqSize = 0;

	GetUseDeqSize(&iUseSize, &iDeqSize);

	if (iUseSize < iSize)
		iLen = iUseSize;

	if (iDeqSize >= iLen) {
		memcpy(chpDest, pFront, iLen);
	}
	else {
		memcpy(chpDest, pFront, iDeqSize);
		memcpy(&chpDest[iDeqSize], pBuff, iLen - iDeqSize);
	}

	MoveFront(iLen);
	return iLen;
}

/////////////////////////////////////////////////////////////////////////
// ReadPos ���� ����Ÿ �о��. ReadPos ����.
//
// Parameters: (char *)����Ÿ ������. (int)ũ��.
// Return: 1(false), 0(succeed)
/////////////////////////////////////////////////////////////////////////

//bool CRingBuffer::Peek(char *chpDest, int iSize) {
//
//	int Check = _Front;
//
//	if (Check == _Rear)
//		return false;
//
//	int UseSize = GetUseSize();
//	if (UseSize < iSize)
//		return false;
//
//	int NotBrokenGetSize = GetNotBrokenGetSize();
//	if (NotBrokenGetSize >= iSize)
//		memcpy(chpDest, &_pBuffer[_Front], iSize);
//	else {
//		memcpy(chpDest, &_pBuffer[_Front], NotBrokenGetSize);
//		memcpy(&chpDest[NotBrokenGetSize], &_pBuffer[0], iSize - NotBrokenGetSize);
//	}
//	return true;
//}
int CRingBuffer::Peek(char *chpDest, int iSize) {

	if (iSize <= 0)
		return 0;

	int iLen = iSize;
	int UseSize = GetUseSize();
	if (UseSize < iSize)
		iLen = UseSize;

	char *pFront = GetFrontBufferPtr();
	char *pBuff = GetBufferPtr();

	int NotBrokenGetSize = GetDeqSize();
	if (NotBrokenGetSize >= iLen) {
		memcpy(chpDest, pFront, iLen);
	}
	else {
		memcpy(chpDest, pFront, NotBrokenGetSize);
		memcpy(&chpDest[NotBrokenGetSize], pBuff, iLen - NotBrokenGetSize);
	}

	return iLen;
}

/////////////////////////////////////////////////////////////////////////
// ���ϴ� ���̸�ŭ �б���ġ ���� ���� / ���� ��ġ �̵�
//
// Parameters: ����.
// Return: ����.
/////////////////////////////////////////////////////////////////////////
void CRingBuffer::MoveRear(int iSize) {

	/*if (iSize <= 0)
		return;

	int FreeSize = GetFreeSize();

	if (FreeSize < iSize)
		iSize = FreeSize;*/

	int iRear = _Rear + iSize;

	//if (_Rear >= _BufferSize)
	//	_Rear -= _BufferSize;

	_Rear = iRear % _BufferSize;
}

void CRingBuffer::MoveFront(int iSize) {

	/*if (iSize <= 0)
		return;

	int UseSize = GetUseSize();

	if (UseSize < iSize)
		iSize = UseSize;*/

	int iFront = _Front + iSize;

	//if (_Front >= _BufferSize)
	//	_Front -= _BufferSize;

	_Front = iFront % _BufferSize;
}

/////////////////////////////////////////////////////////////////////////
// ������ ��� ����Ÿ ����.
//
// Parameters: ����.
// Return: ����.
/////////////////////////////////////////////////////////////////////////
void CRingBuffer::ClearBuffer(void) {

	_Front = _Rear = 0;
}

/////////////////////////////////////////////////////////////////////////
// ������ Front ������ ����.
//
// Parameters: ����.
// Return: (char *) ���� ������.
/////////////////////////////////////////////////////////////////////////
char *CRingBuffer::GetBufferPtr(void) {

	return _pBuffer;
}

/////////////////////////////////////////////////////////////////////////
// ������ Front ������ ����.
//
// Parameters: ����.
// Return: (char *) ���� ������.
/////////////////////////////////////////////////////////////////////////
char *CRingBuffer::GetFrontBufferPtr(void) {

	return (_pBuffer + _Front);
}

/////////////////////////////////////////////////////////////////////////
// ������ RearPos ������ ����.
//
// Parameters: ����.
// Return: (char *) ���� ������.
/////////////////////////////////////////////////////////////////////////
char *CRingBuffer::GetRearBufferPtr(void) {

	return (_pBuffer + _Rear);
}

void CRingBuffer::GetUseDeqSize(int * pUseSize, int * pDeqSize)
{
	int iFront = _Front;
	int iRear = _Rear;
	int iUseSize = 0;
	int iDeqSize = 0;

	int Size = _BufferSize - iFront;
	
	if (iRear < iFront)
		iUseSize = _BufferSize - (iFront - iRear);

	else
		iUseSize = iRear - iFront;

	if (iUseSize > Size) {

		iDeqSize = Size;
	}

	else
		iDeqSize = iUseSize;

	*pUseSize = iUseSize;
	*pDeqSize = iDeqSize;
}

void CRingBuffer::GetFreeEnqSize(int * pFreeSize, int * pEnqSize)
{
	int iRear = _Rear;
	int iFront = _Front;
	int iFreeSize = 0;
	int iEnqSize = 0;

	// UseSize ���ϱ�
	int iUseSize = GetUseSize();
	if (iRear < iFront)
		iUseSize = _BufferSize - (iFront - iRear);
	else
		iUseSize = iRear - iFront;

	// FreeSize ���ϱ�
	int iRealSize = _BufferSize - 1;
	iFreeSize = iRealSize - iUseSize;

	// EnqSize ���ϱ� 
	int MaxSize = _BufferSize;
	
	if (iFront == 0)
		MaxSize -= 1;

	if (iRear >= iFront)
		iEnqSize = MaxSize - iRear;

	else //if (_Rear < _Front)
		iEnqSize = iFront - iRear - 1;

	if (iEnqSize <= 0) {
		int a = 0;
	}

	*pFreeSize = iFreeSize;
	*pEnqSize = iEnqSize;
}

void CRingBuffer::Lock(void)
{
	AcquireSRWLockExclusive(&_SRWLock);
}

void CRingBuffer::Unlock(void)
{
	ReleaseSRWLockExclusive(&_SRWLock);
}

bool CRingBuffer::isEmpty()
{
	return _Front == _Rear;
}

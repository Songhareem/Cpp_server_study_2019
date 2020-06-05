
#include "pch.h"
#include "CRingBuffer.h"
#include <iostream>

CRingBuffer::CRingBuffer(void) {
	
	Initial(10000);
}

CRingBuffer::CRingBuffer(int iBufferSize) {

	Initial(iBufferSize);
}

CRingBuffer::~CRingBuffer(void) {

	delete[] _pBuffer;
}

void CRingBuffer::Initial(int iBufferSize) {

	_BufferSize = iBufferSize + 1;
	_pBuffer = new char[_BufferSize];
	_Front = _Rear = 0;
}

void CRingBuffer::Resize(int size) {

	delete[] _pBuffer;
	_BufferSize = size + 1;
	_pBuffer = new char[size + 1];
	_Front = _Rear = 0;
}

/////////////////////////////////////////////////////////////////////////
// ������ ������ ���. ( ���� byte ���� )
//
// Parameters: ����.
// Return: (int)����� �뷮 + 1
/////////////////////////////////////////////////////////////////////////
int	CRingBuffer::GetBufferSize(void) {

	return _BufferSize;
}

/////////////////////////////////////////////////////////////////////////
// ���� ������� �뷮 ���.
//
// Parameters: ����.
// Return: (int)������� �뷮.
/////////////////////////////////////////////////////////////////////////
int	CRingBuffer::GetUseSize(void) {

	if (_Rear >= _Front)
		return _Rear - _Front;
	else //if (_Rear < _Front)
		return _BufferSize - (_Front - _Rear);
}

/////////////////////////////////////////////////////////////////////////
// ���� ���ۿ� ���� �뷮 ���. 
//
// Parameters: ����.
// Return: (int)�����뷮.
/////////////////////////////////////////////////////////////////////////
int	CRingBuffer::GetFreeSize(void) {

	int realSize = _BufferSize - 1;
	
	return realSize - GetUseSize();
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
int	CRingBuffer::GetNotBrokenGetSize(void) {

	if (_Rear >= _Front)
		return _Rear - _Front;
	else //if (_Rear < _Front)
		return _BufferSize - _Front;
}

int	CRingBuffer::GetNotBrokenPutSize(void) {

	if (_Rear >= _Front)
		return _BufferSize - _Rear;
	else //if (_Rear < _Front)
		return _Front - _Rear - 1;
}

/////////////////////////////////////////////////////////////////////////
// WritePos �� ����Ÿ ����.
//
// Parameters: (char *)����Ÿ ������. (int)ũ��. 
// Return: ���� ������
/////////////////////////////////////////////////////////////////////////
int CRingBuffer::Enqueue(char *chpData, int iSize) {

	/* return bool
	int Check = _Rear + 1;
	if (Check > _BufferSize - 1)
		Check -= _BufferSize;
	
	if (Check == _Front)
		return false;

	int FreeSize = GetFreeSize();
	if (FreeSize < iSize)
		return false;

	int NotBrokenPutSize = GetNotBrokenPutSize();
	if (NotBrokenPutSize >= iSize)
		memcpy(&_pBuffer[_Rear], chpData, iSize);
	else {
		memcpy(&_pBuffer[_Rear], chpData, NotBrokenPutSize);
		memcpy(&_pBuffer[0], &chpData[NotBrokenPutSize], iSize - NotBrokenPutSize);
	}
	MoveRear(iSize);
	return true;
	*/

	int len = iSize;
	int free = GetFreeSize();
	int part = GetNotBrokenPutSize();
	char* pData = chpData;
	char* ptr = GetRearBufferPtr();
	int rearPos = 0;

	if (iSize == 0) { return 0; }
	if (free <= 0) { return 0; }
	if (free < iSize) { len = free; }

	if (len > part)
	{
		memcpy_s(ptr, part, pData, part);
		pData += part;
		ptr = _pBuffer;
		part = len - part;
		memcpy_s(ptr, part, pData, part);
	}
	else
	{
		memcpy_s(ptr, len, chpData, len);
	}

	_Rear += len;
	_Rear %= _BufferSize;

	return len;
}

/////////////////////////////////////////////////////////////////////////
// ReadPos ���� ����Ÿ ������. ReadPos �̵�.
//
// Parameters: (char *)����Ÿ ������. (int)ũ��.
// Return: �� ������
/////////////////////////////////////////////////////////////////////////
int CRingBuffer::Dequeue(char *chpDest, int iSize) {
	
	/* return bool
	int Check = _Front;

	if (Check == _Rear)
		return false;

	int UseSize = GetUseSize();
	if (UseSize < iSize)
		return false;

	int NotBrokenGetSize = GetNotBrokenGetSize();
	if (NotBrokenGetSize >= iSize)
		memcpy(chpDest, &_pBuffer[_Front], iSize);
	else {
		memcpy(chpDest, &_pBuffer[_Front], NotBrokenGetSize);
		memcpy(&chpDest[NotBrokenGetSize], &_pBuffer[0], iSize - NotBrokenGetSize);
	}
	MoveFront(iSize);
	
	return true;
	*/

	int len = iSize;
	int use = GetUseSize();
	int part = GetNotBrokenGetSize();
	char* pDest = chpDest;
	char* ptr = GetFrontBufferPtr();

	if (iSize == 0) { return 0; }
	if (use < iSize) { len = use; }

	if (len > part)
	{
		memcpy_s(pDest, part, ptr, part);
		pDest += part;
		ptr = _pBuffer;
		part = len - part;
		memcpy_s(pDest, part, ptr, part);
	}
	else
	{
		memcpy_s(pDest, len, ptr, len);
	}

	_Front += len;
	_Front %= _BufferSize;

	return len;
}

/////////////////////////////////////////////////////////////////////////
// ReadPos ���� ����Ÿ �о��. ReadPos ����.
//
// Parameters: (char *)����Ÿ ������. (int)ũ��.
// Return: �̾Ƴ� ������
/////////////////////////////////////////////////////////////////////////
int CRingBuffer::Peek(char *chpDest, int iSize) {

	/* return bool 
	int Check = _Front;

	if (Check == _Rear)
		return false;

	int UseSize = GetUseSize();
	if (UseSize < iSize)
		return false;

	int NotBrokenGetSize = GetNotBrokenGetSize();
	if (NotBrokenGetSize >= iSize)
		memcpy(chpDest, &_pBuffer[_Front], iSize);
	else {
		memcpy(chpDest, &_pBuffer[_Front], NotBrokenGetSize);
		memcpy(&chpDest[NotBrokenGetSize], &_pBuffer[0], iSize - NotBrokenGetSize);
	}
	return true;
	*/

	int len = iSize;
	int use = GetUseSize();
	int part = GetNotBrokenGetSize();
	char* pDest = chpDest;
	char* ptr = GetFrontBufferPtr();

	if (iSize == 0) { return 0; }
	if (use < iSize) { len = use; }

	if (len > part)
	{
		memcpy_s(pDest, part, ptr, part);
		pDest += part;
		ptr = _pBuffer;
		part = len - part;
		memcpy_s(pDest, part, ptr, part);
	}
	else
	{
		memcpy_s(pDest, len, ptr, len);
	}

	return len;
}

/////////////////////////////////////////////////////////////////////////
// ���ϴ� ���̸�ŭ �б���ġ ���� ���� / ���� ��ġ �̵�
//
// Parameters: ����.
// Return: ����.
/////////////////////////////////////////////////////////////////////////
void CRingBuffer::MoveRear(int iSize) {

	_Rear += iSize;

	if (_Rear > _BufferSize)
		_Rear -= _BufferSize;
}

void CRingBuffer::MoveFront(int iSize) {

	_Front += iSize;

	if (_Front > _BufferSize)
		_Front -= _BufferSize;
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
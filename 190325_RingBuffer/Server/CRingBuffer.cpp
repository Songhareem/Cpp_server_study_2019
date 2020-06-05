
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
// Return: 1(false), 0(succeed)
/////////////////////////////////////////////////////////////////////////
bool CRingBuffer::Enqueue(char *chpData, int iSize) {

	int Check = _Rear + 1;
	if (Check > _BufferSize - 1)
		Check -= _BufferSize;
	
	if (Check == _Front)
		return 1;

	int FreeSize = GetFreeSize();
	if (FreeSize < iSize)
		return 1;

	int NotBrokenPutSize = GetNotBrokenPutSize();
	if (NotBrokenPutSize >= iSize)
		memcpy(&_pBuffer[_Rear], chpData, iSize);
	else {
		memcpy(&_pBuffer[_Rear], chpData, NotBrokenPutSize);
		memcpy(&_pBuffer[0], &chpData[NotBrokenPutSize], iSize - NotBrokenPutSize);
	}
	MoveRear(iSize);
	return 0;
}

/////////////////////////////////////////////////////////////////////////
// ReadPos ���� ����Ÿ ������. ReadPos �̵�.
//
// Parameters: (char *)����Ÿ ������. (int)ũ��.
// Return: 1(false), 0(succeed)
/////////////////////////////////////////////////////////////////////////
bool CRingBuffer::Dequeue(char *chpDest, int iSize) {
	
	int Check = _Front;

	if (Check == _Rear)
		return 1;

	int UseSize = GetUseSize();
	if (UseSize < iSize)
		return 1;

	int NotBrokenGetSize = GetNotBrokenGetSize();
	if (NotBrokenGetSize >= iSize)
		memcpy(chpDest, &_pBuffer[_Front], iSize);
	else {
		memcpy(chpDest, &_pBuffer[_Front], NotBrokenGetSize);
		memcpy(&chpDest[NotBrokenGetSize], &_pBuffer[0], iSize - NotBrokenGetSize);
	}
	MoveFront(iSize);
	return 0;
}

/////////////////////////////////////////////////////////////////////////
// ReadPos ���� ����Ÿ �о��. ReadPos ����.
//
// Parameters: (char *)����Ÿ ������. (int)ũ��.
// Return: 1(false), 0(succeed)
/////////////////////////////////////////////////////////////////////////
bool CRingBuffer::Peek(char *chpDest, int iSize) {

	int Check = _Front;

	if (Check == _Rear)
		return 1;

	int UseSize = GetUseSize();
	if (UseSize < iSize)
		return 1;

	int NotBrokenGetSize = GetNotBrokenGetSize();
	if (NotBrokenGetSize >= iSize)
		memcpy(chpDest, &_pBuffer[_Front], iSize);
	else {
		memcpy(chpDest, &_pBuffer[_Front], NotBrokenGetSize);
		memcpy(&chpDest[NotBrokenGetSize], &_pBuffer[0], iSize - NotBrokenGetSize);
	}
	return 0;
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
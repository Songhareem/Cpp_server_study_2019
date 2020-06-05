
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
// 링버퍼 사이즈 얻기. ( 여백 byte 포함 )
//
// Parameters: 없음.
// Return: (int)선언시 용량 + 1
/////////////////////////////////////////////////////////////////////////
int	CRingBuffer::GetBufferSize(void) {

	return _BufferSize;
}

/////////////////////////////////////////////////////////////////////////
// 현재 사용중인 용량 얻기.
//
// Parameters: 없음.
// Return: (int)사용중인 용량.
/////////////////////////////////////////////////////////////////////////
int	CRingBuffer::GetUseSize(void) {

	if (_Rear >= _Front)
		return _Rear - _Front;
	else //if (_Rear < _Front)
		return _BufferSize - (_Front - _Rear);
}

/////////////////////////////////////////////////////////////////////////
// 현재 버퍼에 남은 용량 얻기. 
//
// Parameters: 없음.
// Return: (int)남은용량.
/////////////////////////////////////////////////////////////////////////
int	CRingBuffer::GetFreeSize(void) {

	int realSize = _BufferSize - 1;
	
	return realSize - GetUseSize();
}

/////////////////////////////////////////////////////////////////////////
// 버퍼 포인터로 외부에서 한방에 읽고, 쓸 수 있는 길이.
// (끊기지 않은 길이)
//
// 원형 큐의 구조상 버퍼의 끝단에 있는 데이터는 끝 -> 처음으로 돌아가서
// 2번에 데이터를 얻거나 넣을 수 있음. 이 부분에서 끊어지지 않은 길이를 의미
//
// Parameters: 없음.
// Return: (int)사용가능 용량.
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
// WritePos 에 데이타 넣음.
//
// Parameters: (char *)데이타 포인터. (int)크기. 
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
// ReadPos 에서 데이타 가져옴. ReadPos 이동.
//
// Parameters: (char *)데이타 포인터. (int)크기.
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
// ReadPos 에서 데이타 읽어옴. ReadPos 고정.
//
// Parameters: (char *)데이타 포인터. (int)크기.
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
// 원하는 길이만큼 읽기위치 에서 삭제 / 쓰기 위치 이동
//
// Parameters: 없음.
// Return: 없음.
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
// 버퍼의 모든 데이타 삭제.
//
// Parameters: 없음.
// Return: 없음.
/////////////////////////////////////////////////////////////////////////
void CRingBuffer::ClearBuffer(void) {

	_Front = _Rear = 0;
}

/////////////////////////////////////////////////////////////////////////
// 버퍼의 Front 포인터 얻음.
//
// Parameters: 없음.
// Return: (char *) 버퍼 포인터.
/////////////////////////////////////////////////////////////////////////
char *CRingBuffer::GetFrontBufferPtr(void) {

	return (_pBuffer + _Front);
}

/////////////////////////////////////////////////////////////////////////
// 버퍼의 RearPos 포인터 얻음.
//
// Parameters: 없음.
// Return: (char *) 버퍼 포인터.
/////////////////////////////////////////////////////////////////////////
char *CRingBuffer::GetRearBufferPtr(void) {

	return (_pBuffer + _Rear);
}
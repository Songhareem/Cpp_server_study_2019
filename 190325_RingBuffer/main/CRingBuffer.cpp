//
//#include "pch.h"
//#include "CRingBuffer.h"
//#include <iostream>
//
//CRingBuffer::CRingBuffer(void) {
//	
//	Initial(10000);
//}
//
//CRingBuffer::CRingBuffer(int iBufferSize) {
//
//	Initial(iBufferSize);
//}
//
//CRingBuffer::~CRingBuffer(void) {
//
//	delete[] _pBuffer;
//}
//
//void CRingBuffer::Initial(int iBufferSize) {
//
//	_BufferSize = iBufferSize + 1;
//	_pBuffer = new char[_BufferSize];
//	_Front = _Rear = 0;
//}
//
//void CRingBuffer::Resize(int size) {
//
//	delete[] _pBuffer;
//	_BufferSize = size + 1;
//	_pBuffer = new char[size + 1];
//	_Front = _Rear = 0;
//}
//
///////////////////////////////////////////////////////////////////////////
//// ������ ������ ���. ( ���� byte ���� )
////
//// Parameters: ����.
//// Return: (int)����� �뷮 + 1
///////////////////////////////////////////////////////////////////////////
//int	CRingBuffer::GetBufferSize(void) {
//
//	return _BufferSize;
//}
//
///////////////////////////////////////////////////////////////////////////
//// ���� ������� �뷮 ���.
////
//// Parameters: ����.
//// Return: (int)������� �뷮.
///////////////////////////////////////////////////////////////////////////
//int	CRingBuffer::GetUseSize(void) {
//
//	if (_Rear >= _Front)
//		return _Rear - _Front;
//	else //if (_Rear < _Front)
//		return _BufferSize - (_Front - _Rear);
//}
//
///////////////////////////////////////////////////////////////////////////
//// ���� ���ۿ� ���� �뷮 ���. 
////
//// Parameters: ����.
//// Return: (int)�����뷮.
///////////////////////////////////////////////////////////////////////////
//int	CRingBuffer::GetFreeSize(void) {
//
//	int realSize = _BufferSize - 1;
//	
//	return realSize - GetUseSize();
//}
//
///////////////////////////////////////////////////////////////////////////
//// ���� �����ͷ� �ܺο��� �ѹ濡 �а�, �� �� �ִ� ����.
//// (������ ���� ����)
////
//// ���� ť�� ������ ������ ���ܿ� �ִ� �����ʹ� �� -> ó������ ���ư���
//// 2���� �����͸� ��ų� ���� �� ����. �� �κп��� �������� ���� ���̸� �ǹ�
////
//// Parameters: ����.
//// Return: (int)��밡�� �뷮.
//////////////////////////////////////////////////////////////////////////
//int	CRingBuffer::GetNotBrokenGetSize(void) {
//
//	if (_Rear >= _Front)
//		return _Rear - _Front;
//	else //if (_Rear < _Front)
//		return _BufferSize - _Front;
//}
//
//int	CRingBuffer::GetNotBrokenPutSize(void) {
//
//	int MaxSize = _BufferSize;
//	
//	if (_Front == 0)
//		MaxSize -= 1;
//
//	if (_Rear >= _Front)
//		return MaxSize - _Rear;
//
//	else //if (_Rear < _Front)
//		return _Front - _Rear - 1;
//}
//
///////////////////////////////////////////////////////////////////////////
//// WritePos �� ����Ÿ ����.
////
//// Parameters: (char *)����Ÿ ������. (int)ũ��. 
//// Return: 1(false), 0(succeed)
///////////////////////////////////////////////////////////////////////////
//
////bool CRingBuffer::Enqueue(char *chpData, int iSize) {
////	int Check = _Rear + 1;
////	if (Check > _BufferSize - 1)
////	Check -= _BufferSize;
////
////	if (Check == _Front)
////	return false;
////
////	int FreeSize = GetFreeSize();
////	if (FreeSize < iSize)
////		return false;
////
////	int NotBrokenPutSize = GetNotBrokenPutSize();
////	if (NotBrokenPutSize >= iSize)
////	memcpy(&_pBuffer[_Rear], chpData, iSize);
////	else {
////		memcpy(&_pBuffer[_Rear], chpData, NotBrokenPutSize);
////		memcpy(&_pBuffer[0], &chpData[NotBrokenPutSize], iSize - NotBrokenPutSize);
////	}
////
////	MoveRear(iSize);
////	return true;
////}
//int CRingBuffer::Enqueue(char *chpData, int iSize) {
//
//	int Check = _Rear + 1;
//	if (Check > _BufferSize - 1)
//		Check -= _BufferSize;
//	
//	if (Check == _Front)
//		return 0;
//
//	int FreeSize = GetFreeSize();
//	if (FreeSize < iSize)
//		iSize = FreeSize;
//
//	int NotBrokenPutSize = GetNotBrokenPutSize();
//	if (NotBrokenPutSize >= iSize)
//		memcpy(&_pBuffer[_Rear], chpData, iSize);
//	else {
//		memcpy(&_pBuffer[_Rear], chpData, NotBrokenPutSize);
//		memcpy(&_pBuffer[0], &chpData[NotBrokenPutSize], iSize - NotBrokenPutSize);
//	}
//	
//	MoveRear(iSize);
//	return iSize;
//}
//
///////////////////////////////////////////////////////////////////////////
//// ReadPos ���� ����Ÿ ������. ReadPos �̵�.
////
//// Parameters: (char *)����Ÿ ������. (int)ũ��.
//// Return: 1(false), 0(succeed)
///////////////////////////////////////////////////////////////////////////
//
////bool CRingBuffer::Dequeue(char *chpDest, int iSize) {
////	int Check = _Front;
////
////	if (Check == _Rear)
////	return false;
////
////	int UseSize = GetUseSize();
////	if (UseSize < iSize)
////		return false;
////
////	int NotBrokenGetSize = GetNotBrokenGetSize();
////	if (NotBrokenGetSize >= iSize)
////	memcpy(chpDest, &_pBuffer[_Front], iSize);
////	else {
////		memcpy(chpDest, &_pBuffer[_Front], NotBrokenGetSize);
////		memcpy(&chpDest[NotBrokenGetSize], &_pBuffer[0], iSize - NotBrokenGetSize);
////	}
////	MoveFront(iSize);
////	return true;
////}
//int CRingBuffer::Dequeue(char *chpDest, int iSize) {
//	
//	int Check = _Front;
//
//	if (Check == _Rear)
//		return 0;
//
//	int UseSize = GetUseSize();
//	if (UseSize < iSize)
//		iSize = UseSize;
//
//	int NotBrokenGetSize = GetNotBrokenGetSize();
//	if (NotBrokenGetSize >= iSize)
//		memcpy(chpDest, &_pBuffer[_Front], iSize);
//	else {
//		memcpy(chpDest, &_pBuffer[_Front], NotBrokenGetSize);
//		memcpy(&chpDest[NotBrokenGetSize], &_pBuffer[0], iSize - NotBrokenGetSize);
//	}
//	MoveFront(iSize);
//	return iSize;
//}
//
///////////////////////////////////////////////////////////////////////////
//// ReadPos ���� ����Ÿ �о��. ReadPos ����.
////
//// Parameters: (char *)����Ÿ ������. (int)ũ��.
//// Return: 1(false), 0(succeed)
///////////////////////////////////////////////////////////////////////////
//
////bool CRingBuffer::Peek(char *chpDest, int iSize) {
////
////	int Check = _Front;
////
////	if (Check == _Rear)
////		return false;
////
////	int UseSize = GetUseSize();
////	if (UseSize < iSize)
////		return false;
////
////	int NotBrokenGetSize = GetNotBrokenGetSize();
////	if (NotBrokenGetSize >= iSize)
////		memcpy(chpDest, &_pBuffer[_Front], iSize);
////	else {
////		memcpy(chpDest, &_pBuffer[_Front], NotBrokenGetSize);
////		memcpy(&chpDest[NotBrokenGetSize], &_pBuffer[0], iSize - NotBrokenGetSize);
////	}
////	return true;
////}
//int CRingBuffer::Peek(char *chpDest, int iSize) {
//
//	int Check = _Front;
//
//	if (Check == _Rear)
//		return 0;
//
//	int UseSize = GetUseSize();
//	if (UseSize < iSize)
//		iSize = UseSize;
//
//	int NotBrokenGetSize = GetNotBrokenGetSize();
//	if (NotBrokenGetSize >= iSize)
//		memcpy(chpDest, &_pBuffer[_Front], iSize);
//	else {
//		memcpy(chpDest, &_pBuffer[_Front], NotBrokenGetSize);
//		memcpy(&chpDest[NotBrokenGetSize], &_pBuffer[0], iSize - NotBrokenGetSize);
//	}
//	return iSize;
//}
//
///////////////////////////////////////////////////////////////////////////
//// ���ϴ� ���̸�ŭ �б���ġ ���� ���� / ���� ��ġ �̵�
////
//// Parameters: ����.
//// Return: ����.
///////////////////////////////////////////////////////////////////////////
//void CRingBuffer::MoveRear(int iSize) {
//
//	if (iSize <= 0)
//		return;
//
//	int FreeSize = GetFreeSize();
//
//	if (FreeSize < iSize)
//		iSize = FreeSize;
//
//	_Rear += iSize;
//
//	if (_Rear >= _BufferSize)
//		_Rear -= _BufferSize;
//}
//
//void CRingBuffer::MoveFront(int iSize) {
//
//	if (iSize <= 0)
//		return;
//
//	int UseSize = GetUseSize();
//
//	if (UseSize < iSize)
//		iSize = UseSize;
//
//	_Front += iSize;
//
//	if (_Front >= _BufferSize)
//		_Front -= _BufferSize;
//}
//
///////////////////////////////////////////////////////////////////////////
//// ������ ��� ����Ÿ ����.
////
//// Parameters: ����.
//// Return: ����.
///////////////////////////////////////////////////////////////////////////
//void CRingBuffer::ClearBuffer(void) {
//
//	_Front = _Rear = 0;
//}
//
///////////////////////////////////////////////////////////////////////////
//// ������ Front ������ ����.
////
//// Parameters: ����.
//// Return: (char *) ���� ������.
///////////////////////////////////////////////////////////////////////////
//char *CRingBuffer::GetFrontBufferPtr(void) {
//
//	return (_pBuffer + _Front);
//}
//
///////////////////////////////////////////////////////////////////////////
//// ������ RearPos ������ ����.
////
//// Parameters: ����.
//// Return: (char *) ���� ������.
///////////////////////////////////////////////////////////////////////////
//char *CRingBuffer::GetRearBufferPtr(void) {
//
//	return (_pBuffer + _Rear);
//}


#include "pch.h"
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

	delete[] _pBuffer;
}

void CRingBuffer::Initial(int iBufferSize) {

	if (iBufferSize <= 0)
		return;

	if (_pBuffer != NULL)
		delete _pBuffer;

	_BufferSize = iBufferSize + 1;
	_pBuffer = new char[_BufferSize];
	_Front = _Rear = 0;
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

	if (_Rear < _Front)
		return _BufferSize - (_Front - _Rear);

	return _Rear - _Front;
}

/////////////////////////////////////////////////////////////////////////
// ���� ���ۿ� ���� �뷮 ���. 
//
// Parameters: ����.
// Return: (int)�����뷮.
/////////////////////////////////////////////////////////////////////////
int	CRingBuffer::GetFreeSize(void) {

	int RealSize = _BufferSize - 1;
	int UseSize = GetUseSize();
	return RealSize - UseSize;
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

int	CRingBuffer::GetNotBrokenPutSize(void) {

	int MaxSize = _BufferSize;

	if (_Front == 0)
		MaxSize -= 1;

	if (_Rear >= _Front)
		return MaxSize - _Rear;

	else //if (_Rear < _Front)
		return _Front - _Rear - 1;
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

	/*int Check = _Rear + 1;
	if (Check > _BufferSize - 1)
		Check -= _BufferSize;

	if (Check >= _Front)
		return 0;
	*/

	int FreeSize = GetFreeSize();
	if (FreeSize <= 0)
		return 0;
	if (FreeSize < iSize)
		iSize = FreeSize;

	char *pRear = GetRearBufferPtr();
	char *pBuff = GetBufferPtr();

	int NotBrokenPutSize = GetNotBrokenPutSize();
	if (NotBrokenPutSize >= iSize) {
		memcpy(&_pBuffer[_Rear], chpData, iSize);
		/*for (int i = 0; i < iSize; i++) {
			*(pRear + i) = *(chpData + i);
		}*/
	}
	else {
		memcpy(pRear, chpData, NotBrokenPutSize);
		memcpy(pBuff, &chpData[NotBrokenPutSize], iSize - NotBrokenPutSize);
		/*for (int i = 0; i < NotBrokenPutSize; i++) {
			*(pRear + i) = *(chpData + i);
		}
		for (int i = 0; i < iSize - NotBrokenPutSize; i++) {

			*(pBuff + i) = *(chpData + NotBrokenPutSize + i);
		}*/
	}

	MoveRear(iSize);
	return iSize;
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

	/*int Check = _Front;

	if (Check == _Rear)
		return 0;
*/
	int UseSize = GetUseSize();
	if (UseSize < iSize)
		iSize = UseSize;

	char *pFront = GetFrontBufferPtr();
	char *pBuff = GetBufferPtr();

	int NotBrokenGetSize = GetNotBrokenGetSize();
	if (NotBrokenGetSize >= iSize) {
		memcpy(chpDest, &_pBuffer[_Front], iSize);
		/*for (int i = 0; i < iSize; i++) {

			*(chpDest + i) = *(pFront + i);
		}*/
	}
	else {
		memcpy(chpDest, pFront, NotBrokenGetSize);
		memcpy(&chpDest[NotBrokenGetSize], pBuff, iSize - NotBrokenGetSize);
		/*for (int i = 0; i < NotBrokenGetSize; i++) {
			*(chpDest + i) = *(pFront + i);
		}
		for (int i = 0; i < iSize - NotBrokenGetSize; i++) {

			*(chpDest + NotBrokenGetSize + i) = *(pBuff + i);
		}*/
	}
	MoveFront(iSize);
	return iSize;
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

	int Check = _Front;

	if (Check == _Rear)
		return 0;

	int UseSize = GetUseSize();
	if (UseSize < iSize)
		iSize = UseSize;

	char *pFront = GetFrontBufferPtr();
	char *pBuff = GetBufferPtr();

	int NotBrokenGetSize = GetNotBrokenGetSize();
	if (NotBrokenGetSize >= iSize) {
		memcpy(chpDest, &_pBuffer[_Front], iSize);
		/*for (int i = 0; i < iSize; i++) {

			*(chpDest + i) = *(pFront + i);
		}*/
	}
	else {
		memcpy(chpDest, pFront, NotBrokenGetSize);
		memcpy(&chpDest[NotBrokenGetSize], pBuff, iSize - NotBrokenGetSize);
		/*for (int i = 0; i < NotBrokenGetSize; i++) {
			*(chpDest + i) = *(pFront + i);
		}
		for (int i = 0; i < iSize - NotBrokenGetSize; i++) {

			*(chpDest + NotBrokenGetSize + i) = *(pBuff + i);
		}*/
	}

	/*int NotBrokenGetSize = GetNotBrokenGetSize();
	if (NotBrokenGetSize >= iSize)
		memcpy(chpDest, &_pBuffer[_Front], iSize);
	else {
		memcpy(chpDest, &_pBuffer[_Front], NotBrokenGetSize);
		memcpy(&chpDest[NotBrokenGetSize], &_pBuffer[0], iSize - NotBrokenGetSize);
	}*/

	return iSize;
}

/////////////////////////////////////////////////////////////////////////
// ���ϴ� ���̸�ŭ �б���ġ ���� ���� / ���� ��ġ �̵�
//
// Parameters: ����.
// Return: ����.
/////////////////////////////////////////////////////////////////////////
void CRingBuffer::MoveRear(int iSize) {

	if (iSize <= 0)
		return;

	int FreeSize = GetFreeSize();

	if (FreeSize < iSize)
		iSize = FreeSize;

	_Rear += iSize;

	if (_Rear >= _BufferSize)
		_Rear -= _BufferSize;
}

void CRingBuffer::MoveFront(int iSize) {

	if (iSize <= 0)
		return;

	int UseSize = GetUseSize();

	if (UseSize < iSize)
		iSize = UseSize;

	_Front += iSize;

	if (_Front >= _BufferSize)
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
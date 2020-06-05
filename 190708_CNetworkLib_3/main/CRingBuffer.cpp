
#pragma once

#include "CRingBuffer.h"


CRingBuffer::CRingBuffer()
{
	Initial(DEFALT_SIZE);
}

CRingBuffer::CRingBuffer(int iBufferSize)
{
	Initial(iBufferSize);
}

CRingBuffer::~CRingBuffer()
{
	if (buffer != nullptr)
	{
		delete buffer;
	}
}

void CRingBuffer::Initial(int iBufferSize)
{
	buffer = new char[iBufferSize];
	memset(buffer, 0, iBufferSize);
	size = iBufferSize;
	front = rear = 0;
	InitializeCriticalSection(&cs);
}

bool CRingBuffer::isEmpty()
{
	return (front == rear);
}

bool CRingBuffer::Resize(int resize)
{
	char* rebuf = new char[resize];
	if (rebuf == NULL) { return false; }
	for (unsigned int i = 0; i < resize; i++)
	{
		if (i < size)
		{
			rebuf[i] = buffer[i];
		}
		else
		{
			rebuf[i] = '\0';
		}
	}
	size = resize;

	char* target = buffer;
	delete target;
	buffer = rebuf;

	return true;
}

int CRingBuffer::GetBufferSize(void)
{
	return size;
}

int CRingBuffer::GetUseSize(void)
{
	if (front > rear)
	{
		return (size - (front - rear));

	}
	return (rear - front);
}

int CRingBuffer::GetFreeSize(void)
{
	int re = 0;
	re = (size - 1) - GetUseSize();
	return re;
}

int CRingBuffer::GetDeqSize(void) // 디큐사이즈
{
	int re = size - front;
	int use = GetUseSize();

	if (use > re)
	{
		return re;
	}
	else
	{
		return use;
	}
}

int CRingBuffer::GetEnqSize(void) // 인큐사이즈
{
	int re = size - rear;
	int free = GetFreeSize();

	if (free > re)
	{
		return re;
	}
	else
	{
		return free;
	}
}

int CRingBuffer::Enqueue(char* chpData, int iSize)
{
	int len = iSize;
	int free = GetFreeSize();
	if (free <= 0) { return 0; }
	//if (free < iSize)
	//{
	//	Resize(size+iSize + 1);
	//	free = GetFreeSize();

	//}
	int part = GetEnqSize();
	char* pData = chpData;
	char* ptr = GetRearBufferPtr();

	if (iSize == 0) { return 0; }
	if (free < iSize) { len = free; }

	if (len > part)
	{
		memcpy_s(ptr, part, pData, part);
		ptr = buffer;
		pData += part;
		part = len - part;
		memcpy_s(ptr, part, pData, part);
	}
	else
	{
		memcpy_s(ptr, len, pData, len);
	}

	MoveRear(len);

	return len;
}

int CRingBuffer::Dequeue(char* chpDest, int iSize)
{
	int len = iSize;
	int use = GetUseSize();
	int part = GetDeqSize();
	char* pDest = chpDest;
	char* ptr = GetFrontBufferPtr();

	if (iSize <= 0) { return 0; }
	if (use < iSize) { len = use; }

	if (len > part)
	{
		memcpy_s(pDest, part, ptr, part);
		ptr = buffer;
		pDest += part;
		part = len - part;
		memcpy_s(pDest, part, ptr, part);
	}
	else
	{
		memcpy_s(pDest, len, ptr, len);
	}

	MoveFront(len);

	return len;
}

int CRingBuffer::Peek(char* chpDest, int iSize)
{
	int len = iSize;
	int use = GetUseSize();
	int part = GetDeqSize();
	char* pDest = chpDest;
	char* ptr = GetFrontBufferPtr();

	if (iSize == 0) { return 0; }
	if (use < iSize) { len = use; }

	if (len > part)
	{
		memcpy_s(pDest, part, ptr, part);
		ptr = buffer;
		pDest += part;
		part = len - part;
		memcpy_s(pDest, part, ptr, part);
	}
	else
	{
		memcpy_s(pDest, len, ptr, len);
	}

	return len;
}

void CRingBuffer::MoveRear(int iSize)
{
	int re = rear + iSize;

	rear = re % size;
}

int CRingBuffer::MoveFront(int iSize)
{
	int re = front + iSize;
	front = re % size;
	return front;
}

void CRingBuffer::ClearBuffer(void)
{
	front = rear = 0;
}

char* CRingBuffer::GetFrontBufferPtr(void)
{
	return &buffer[front];
}

char* CRingBuffer::GetRearBufferPtr(void)
{
	return &buffer[rear];
}

char * CRingBuffer::GetBufferPtr(void)
{
	return &buffer[0];
}

void CRingBuffer::Lock()
{
	EnterCriticalSection(&cs);
}

void CRingBuffer::Unlock()
{
	LeaveCriticalSection(&cs);
}

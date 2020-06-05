#pragma once
#include <iostream>
#include <Windows.h>
#include <process.h>

//#define DEFALT_SIZE 102400
#define DEFALT_SIZE 1001

class CRingBuffer
{
public:
	CRingBuffer();
	CRingBuffer(int iBufferSize);
	~CRingBuffer();

private:
	void Initial(int iBufferSize);
public:
	bool isEmpty();
	bool Resize(int size);
	int GetBufferSize(void);

	int GetUseSize(void);
	int GetFreeSize(void);

	int	GetDeqSize(void);
	int	GetEnqSize(void);

	int	Enqueue(char* chpData, int iSize);
	int	Dequeue(char* chpDest, int iSize);

	int	Peek(char* chpDest, int iSize);

	void MoveRear(int iSize);
	int	MoveFront(int iSize);

	void ClearBuffer(void);

	char* GetFrontBufferPtr(void);
	char* GetRearBufferPtr(void);
	char* GetBufferPtr(void);
	
	void Lock();
	void Unlock();

	void PrintBuffer()
	{
		printf("{ ");
		for (int i = 0; i < size; i++)
		{
			if (i == front) { printf("["); }
			if (i == rear) { printf("]"); }
			printf("%d", buffer[i]);
		}
		printf(" }");
		
	}

private:
	char* buffer = nullptr;
	unsigned int size = 0;
	unsigned int front = 0;
	unsigned int rear = 0;
	CRITICAL_SECTION cs;
};


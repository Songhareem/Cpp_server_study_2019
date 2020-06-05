#pragma once
#include <iostream>
#include <Windows.h>
#include <process.h>

#define DEFALT_SIZE 102400

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

	void Lock();
	void Unlock();

	void PrintBuffer()
	{
		//printf("-----------------------\n");
		//printf("\nGetUseSize : %d\nGetFreeSize : %d\nFront : %d\nRear : %d\n\n", GetUseSize(), GetFreeSize(), front, rear);
		//printf("{ ");
		if (front > rear)
		{
			for (unsigned int i = front; i < size; i++)
			{
				printf("%c ", buffer[i]);
			}
			for (unsigned int j = 0; j < rear; j++)
			{
				printf("%c ", buffer[j]);
			}
		}
		else
		{
			for (unsigned int i = front; i < rear; i++)
			{
				printf("%c ", buffer[i]);
			}

		}
		//printf(" }\n");
		//printf("-----------------------\n");
	}

private:
	char* buffer = nullptr;
	unsigned int size = 0;
	unsigned int front = 0;
	unsigned int rear = 0;
	CRITICAL_SECTION cs;
};


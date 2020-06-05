#pragma once
#include "MemoryPoolTLS.h"
#define dfMAX_BLOCK 10
#define dfCOUNT_INIT_VALUE 0

template<class DATA>
//typedef int DATA;
class CChunk
{
public:
	struct DATA_BLOCK
	{
		CChunk<DATA>* pChunk;
		DATA data;
	};

	CChunk()
	{
		allocCount = dfCOUNT_INIT_VALUE;
		freeCount = dfCOUNT_INIT_VALUE;
	}
	~CChunk()
	{

	}

	DATA* Alloc()
	{
		//printf("[0x%p]Chunk Alloc Count : %d\n",this, allocCount);

		DATA_BLOCK* pBlock = &blockList[allocCount];
		pBlock->pChunk = this;
		allocCount++;

		return &pBlock->data;
	}

	bool Free()
	{
		//printf("[0x%p]Chunk Free Count : %d\n", this, freeCount);
		long free = InterlockedIncrement(&freeCount); // TODO : 디버그 때매 따로 뺴놓음. 나중에 붙이셈
		if (free == dfMAX_BLOCK)
		{
			//pMemoryPoolTLS->pMemoryPool->Free(this); // 만약 Chunk가 멤버로 pMPTLS를 가진다면, 있어야 할 코드.
			return false;
		}
		return true;
	}
	
	long GetAllocCount()
	{
		return allocCount;
	}

public:
	DATA_BLOCK blockList[dfMAX_BLOCK];
	long allocCount = dfCOUNT_INIT_VALUE;
	long freeCount = dfCOUNT_INIT_VALUE;
};


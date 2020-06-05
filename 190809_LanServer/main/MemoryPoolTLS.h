#pragma once
#include <iostream>
#include <Windows.h>
#include "MemoryPool.h"
#include "Chunk.h"

template <class DATA>
class CMemoryPoolTLS
{
public:
	struct DATA_BLOCK
	{
		CChunk<DATA>* pChunk;
		DATA data;
	};

	CMemoryPoolTLS(bool bPlacementNew = true)
	{
		tlsIndex = TlsAlloc();
		if (TLS_OUT_OF_INDEXES == tlsIndex)
			exit(-1);
		pMemoryPool = new CMemoryPool<CChunk<DATA>>();
		
		InitializeSRWLock(&srwTLS);
	}
	~CMemoryPoolTLS()
	{
		TlsFree(tlsIndex);
		delete pMemoryPool;
	}

	DATA* Alloc()
	{
		AcquireSRWLockExclusive(&srwTLS);

		CChunk<DATA>* pChunk = (CChunk<DATA>*)TlsGetValue(tlsIndex);
		if (pChunk == nullptr) // 최초할당 or 재할당
		{
			pChunk = ChunkAlloc();
		}
		if (pChunk->GetAllocCount() == dfMAX_BLOCK)
		{
			pChunk = ChunkAlloc();
		}
		DATA* pData = pChunk->Alloc();
		

		ReleaseSRWLockExclusive(&srwTLS);

		return pData;
	}

	bool Free(DATA* pData)
	{
		AcquireSRWLockExclusive(&srwTLS);

		if (pData == nullptr)
		{
			printf("Error : Data pointer is Null\n");
			ReleaseSRWLockExclusive(&srwTLS);
			return false;
		}
		BYTE* ptr = (BYTE*)pData;
		DATA_BLOCK* pBlock = (DATA_BLOCK*)(ptr - sizeof(CChunk<DATA>*));
		
		if (!pBlock->pChunk->Free())
		{
			pMemoryPool->Free(pBlock->pChunk);
			InterlockedDecrement(&useSize);
			ReleaseSRWLockExclusive(&srwTLS);
			return false;
		}

		ReleaseSRWLockExclusive(&srwTLS);
		return true;
	}

	CChunk<DATA>* ChunkAlloc()
	{
		CChunk<DATA>* pChunk = (CChunk<DATA>*)pMemoryPool->Alloc();
		if (pChunk == nullptr) {
			printf("Error : Allocation failed - Chunk\n");
			pChunk = 0; // 메모리 부족. 크래쉬 코드.
		}
		
		if (!TlsSetValue(tlsIndex, pChunk))
		{
			printf("Error : TlsSetValue failed\n");
			return nullptr;
		}
		
		//pChunk->pMemoryPoolTLS = this;
		InterlockedIncrement(&useSize);
		return pChunk;
	}

	int GetUsedChunkCount()
	{
		//return pMemoryPool->GetAllocCount();
		return useSize;
	}

	int GetUsedChunkCount2()
	{
		return pMemoryPool->GetUseCount();
	}

	int GetAllocChunkCount()
	{
		return pMemoryPool->GetAllocCount();
	}

	int GetFreeChunkCount()
	{
		return pMemoryPool->GetFreeCount();
	}
private:
	CMemoryPool<CChunk<DATA>>* pMemoryPool;
	DWORD tlsIndex;
	long useSize = 0;

	SRWLOCK	srwTLS;
};
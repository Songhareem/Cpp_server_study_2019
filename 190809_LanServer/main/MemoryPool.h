#pragma once

#include <Windows.h>
#include <assert.h>
#include <new.h>

/*--------------------------------------------------------------------------*/
// ������ �޸� Ǯ
//
// �� ���� ���� ������ ȯ�濡 �����ϰ� �޸� ���� �Ҵ�, ���� ���ش�
/*--------------------------------------------------------------------------*/
template <class DATA>
//typedef __int64 DATA;
class CMemoryPool
{
private:

	/* ******************************************************************** */
	// �� �� �տ� ���� ��� ����ü.
	/* ******************************************************************** */
	struct st_BLOCK_NODE
	{
		st_BLOCK_NODE()
		{
			stpNextBlock = NULL;
		}

		st_BLOCK_NODE* stpNextBlock;
	};

	/* ******************************************************************** */
	// ������ �޸�Ǯ ž ���.
	/* ******************************************************************** */
	struct st_TOP_NODE
	{
		st_BLOCK_NODE* pTopNode;
		__int64			iUniqueNum;
	};


public:

	//////////////////////////////////////////////////////////////////////////
	// ������, �ı���.
	//
	// Parameters:	(int) �ִ� �� ����.
	//				(bool) �޸� Lock �÷��� - �߿��ϰ� �ӵ��� �ʿ�� �Ѵٸ� Lock.
	// Return:
	//////////////////////////////////////////////////////////////////////////
	CMemoryPool(bool bLockFlag = false)
	{
		//////////////////////////////////////////////////////////////////////
		// �ʱ�ȭ
		//////////////////////////////////////////////////////////////////////
		_lBlockCount = 0;
		_lAllocCount = 0;
		_bLockflag = bLockFlag;

		//////////////////////////////////////////////////////////////////////
		// ž ��� �Ҵ�
		//////////////////////////////////////////////////////////////////////
		_pTop = (st_TOP_NODE*)_aligned_malloc(sizeof(st_TOP_NODE), 16);
		_pTop->pTopNode = nullptr;
		_pTop->iUniqueNum = 0;

		_iUniqueNum = 0;
	}

	virtual					~CMemoryPool()
	{
		st_BLOCK_NODE* pNode;

		for (int iCnt = 0; iCnt < _lBlockCount; iCnt++)
		{
			pNode = _pTop->pTopNode;
			if (pNode == nullptr) continue;
			_pTop->pTopNode = _pTop->pTopNode->stpNextBlock;
			free(pNode);
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// �� �ϳ��� �Ҵ�޴´�.
	//
	// Parameters: ����.
	// Return: (DATA *) ����Ÿ �� ������.
	//////////////////////////////////////////////////////////////////////////
	DATA* Alloc(bool bPlacementNew = true)
	{
		st_BLOCK_NODE* pAllocNode = nullptr;
		st_TOP_NODE		stCloneTopNode;

		DATA* pData;

		long			lBlockCount = _lBlockCount;
		InterlockedIncrement((long*)& _lAllocCount);

		//////////////////////////////////////////////////////////////////////
		// �Ҵ� �ؾ� �� ���
		//////////////////////////////////////////////////////////////////////
		if (lBlockCount < _lAllocCount)
		{
			pAllocNode = (st_BLOCK_NODE*)malloc(sizeof(st_BLOCK_NODE) + sizeof(DATA));
			InterlockedIncrement((long*)& _lBlockCount);
		}

		else
		{
			__int64 iUniqueNum = InterlockedIncrement64((LONG64*)& _iUniqueNum);

			do
			{
				stCloneTopNode.iUniqueNum = _pTop->iUniqueNum;
				stCloneTopNode.pTopNode = _pTop->pTopNode;
			} while (!InterlockedCompareExchange128((LONG64*)_pTop, iUniqueNum, (LONG64)_pTop->pTopNode->stpNextBlock, (LONG64*)& stCloneTopNode));

			pAllocNode = stCloneTopNode.pTopNode;
		}

		pData = (DATA*)(pAllocNode + 1);

		if (bPlacementNew)
			new (pData)DATA;

		return pData;
	}

	//////////////////////////////////////////////////////////////////////////
	// ������̴� ���� �����Ѵ�.
	//
	// Parameters: (DATA *) �� ������.
	// Return: (BOOL) TRUE, FALSE.
	//////////////////////////////////////////////////////////////////////////
	bool					Free(DATA* pData)
	{
		st_BLOCK_NODE* pReturnNode = ((st_BLOCK_NODE*)pData) - 1;
		st_TOP_NODE		stCloneTopNode;

		__int64 iUniqueNum = InterlockedIncrement64((LONG64*)& _iUniqueNum);

		do
		{
			stCloneTopNode.iUniqueNum = _pTop->iUniqueNum;
			stCloneTopNode.pTopNode = _pTop->pTopNode;

			pReturnNode->stpNextBlock = _pTop->pTopNode;
		} while (!InterlockedCompareExchange128(
			(LONG64*)_pTop,
			iUniqueNum,
			(LONG64)pReturnNode,
			(LONG64*)& stCloneTopNode
		));

		InterlockedDecrement((long*)& _lAllocCount);

		return true;
	}


	//////////////////////////////////////////////////////////////////////////
	// ���� ������� �� ������ ��´�.
	//
	// Parameters: ����.
	// Return: (int) ������� �� ����.
	//////////////////////////////////////////////////////////////////////////
	int						GetAllocCount(void) { return (int)_lAllocCount; }


	//////////////////////////////////////////////////////////////////////////
	// �޸�Ǯ ��
	//
	// Parameters: ����.
	// Return: ����.
	//////////////////////////////////////////////////////////////////////////
	void					Lock()
	{
		AcquireSRWLockExclusive(&_srwMemoryPool);
	}


	//////////////////////////////////////////////////////////////////////////
	// �޸�Ǯ ���
	//
	// Parameters: ����.
	// Return: ����.
	//////////////////////////////////////////////////////////////////////////
	void					Unlock()
	{
		ReleaseSRWLockExclusive(&_srwMemoryPool);
	}

private:
	//////////////////////////////////////////////////////////////////////////
	// MemoryPool�� Top
	//////////////////////////////////////////////////////////////////////////
	st_TOP_NODE* _pTop;


	//////////////////////////////////////////////////////////////////////////
	// Top�� Unique Number
	//////////////////////////////////////////////////////////////////////////
	__int64					_iUniqueNum;


	//////////////////////////////////////////////////////////////////////////
	// Lockflag
	//////////////////////////////////////////////////////////////////////////
	bool					_bLockflag;


	//////////////////////////////////////////////////////////////////////////
	// ���� �Ҵ����� ��� ��
	//////////////////////////////////////////////////////////////////////////
	long					_lAllocCount;


	//////////////////////////////////////////////////////////////////////////
	// ��ü ��� ��
	//////////////////////////////////////////////////////////////////////////
	long					_lBlockCount;


	//////////////////////////////////////////////////////////////////////////
	// ����ȭ ��ü
	//////////////////////////////////////////////////////////////////////////
	SRWLOCK					_srwMemoryPool;
};
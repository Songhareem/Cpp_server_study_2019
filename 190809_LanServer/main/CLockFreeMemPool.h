//-----------------------------------------------------------------------------------
// ���� ���� ����
// 
// 0) �޸�Ǯ �������
// �ӵ� ��� ��ǥ(������ new - delete ���)
// �޸𸮰����� ����(�޸� ���� ����� �뵵) 
//
// 1) template class / typename ����?
// ref - https://www.ikpil.com/540 
//
// 2) class���� template ����, 
// ���� ����ÿ��� template �缱�� �ʿ����
// �ܺ� ����ÿ��� �Լ�(������, �Ҹ��� ����) ���� �缱���ʿ�
// ���� ����� h���Ͽ� �Լ� ���� �� �ڵ� �ۼ� + include h����
// �ܺ� ����� h���Ͽ� �Լ� ���� / CPP���Ͽ� �ڵ� �ۼ� + include h���� �� CPP����
//
// 3-1) placementNew FALSE ?
// push�� �޸� Ȯ�� �� ������ ȣ��, pop�� �޸� ���� �� �ı��� ȣ��
// Free�޾Ҵٰ� �ٽ� Alloc �ÿ��� �����ڸ� ��ȣ�� (Free�ÿ� ���� �޸𸮸� pop�ؾ��ϴ���?)
// �Ϲݺ��� �� ������ �ʿ���� Ŭ���� �޸� �Ҵ�� ���
//
// 3-2) placementNew TRUE ?
// push�� �޸� Ȯ��, pop�� �޸� ����
// Alloc�� ������ ȣ��, Free�� �ı��� ȣ��
// �����ڰ� �ʿ��� Ŭ���� �޸� �Ҵ�� ���
//
// 4) Ŭ���� ũ��
// ����ִٸ�(������ �� �Ҹ��� ���� + ���� X) 1byte
// ���� �ִٸ� sizeof(�� ����)
//
// 5) st_SEGMENT�� ���, 
// new st_SEGMENT�� ������, ���ο� �ִ� �����ڰ� ������ ȣ��
// �̰� new�� ������ �Ҹ��°�����, �����ڸ� ������ ȣ��Ǵ� ������?
// new �ѹ� ȣ�⿡ ������ ������ ȣ��Ǵ°�
// 
// 6) template Ŭ�����������ڸ�����?
// �۵� �� ����
// template template �ȿ����� �ƿ� �۵� ����
//
// 7) ���ȿ� �����Ͱ� �ƴ� ������ �ȿ� ��带 ���� ��������? (�ӵ� ��� ����)
// list�� ����, list �ܺο��� �ѹ� ����(new or �׳� ����)�ؼ� ����Ʈ�� ���ڷ� �ְ�,
// list ���ο����� new�� �޸� Ȯ�� �� ���ڷ� ���� ���� DATA�� ī��
// �ᱹ 2�� new�ϰ� �Ǵ� ��Ȳ�߻� (�ӵ� ������)
// DATA�ȿ� ��带 ���� ����ϰ� �Ǹ�, new �ѹ����� ���� �� ����(�ӵ� ���) 
// 
// 8) ::operator new(sizeof(type))
// new�� ������, Ÿ�� ����� ������ ����ϴ°� �ƴ϶� �츮�� �־������ 
// ������ ���Ƿ� ������ ȣ�� X
//-----------------------------------------------------------------------------------

#ifndef  __LOCKFREE_MEMORY_POOL__
#define  __LOCKFREE_MEMORY_POOL__

#include <stdio.h>
#include <new.h>
#include <windows.h>

#define dfINIT_NUM	0
#define dfMAGIC_NUM ((WORD)0x5a5a)

#define LOCK(SrwMemPool)	AcquireSRWLockExclusive(&SrwMemPool)
#define UNLOCK(SrwMemPool)	ReleaseSRWLockExclusive(&SrwMemPool)

//#define __DEBUG_MODE__

#ifdef __DEBUG_MODE__ 
#include <iostream>
#define DEBUGTEXT printf("_FreeNode add : %d | _FreeNode->Next : %d\n", _pFreeNode, _pFreeNode->stpNextNode);
#endif

#ifndef __DEBUG_MODE__
#define DEBUGTEXT
#endif

template <typename DATA>
//typedef int DATA;
class CLockFreeMemPool
{
private:

	/* **************************************************************** */
	// �� �� �տ� ���� ��� ����ü.
	/* **************************************************************** */
	typedef struct st_NODE
	{
		// ������
		st_NODE()
		{
			pNext = NULL;
			wMagicNum = dfMAGIC_NUM;
			//printf("st_NODE\n");
		}
		st_NODE *pNext;
		WORD	wMagicNum;
	} NODE;

	_declspec(align(16)) typedef struct st_NODE128
	{
		NODE	*pNode = NULL;
		LONG64	l64Key = 0;
	} NODE128;
	
	typedef struct st_SEGMENT
	{
		st_NODE node;
		DATA	data;
	} SEGMENT;

public:
	CLockFreeMemPool()
		: m_lAllocCount(0), m_lUseCount(0), m_bPlacementNew(true)
	{
		for (int i = 0; i < dfINIT_NUM; i++)
			Push();
	}

	//////////////////////////////////////////////////////////////////////////
	// ������, �ı���.
	//
	// Parameters:	(int) �ʱ� �� ����.
	//				(bool) Alloc �� ������ / Free �� �ı��� ȣ�� ����
	// Return:
	//////////////////////////////////////////////////////////////////////////
	CLockFreeMemPool(int iBlockNum, bool bPlacementNew = false) 
		: m_lAllocCount(0), m_lUseCount(0), m_bPlacementNew(bPlacementNew)
	{
		InitializeSRWLock(&SrwMemPool);

		for (int i = 0; i < iBlockNum; i++)
			Push();
	}

	virtual	~CLockFreeMemPool()
	{
		for (;;) {

			if (Pop() == false)
				break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// �� �ϳ��� �Ҵ�޴´�.  // LockFree Pop���� �̿�
	//
	// Parameters: ����.
	// Return: (DATA *) ����Ÿ �� ������.
	//////////////////////////////////////////////////////////////////////////
	DATA	*Alloc(void) 
	{
		//LOCK(SrwMemPool);
		// LF Pop ���
		NODE128 localTop;
		SEGMENT *pSeg;
		DATA *pAllocData;

		//������ ����ٸ� ���� push �� �Ŀ� �޸� �Ҵ�
		if (_top.pNode == NULL)
			Push();

		do {

			localTop = _top;

		} while (!InterlockedCompareExchange128((LONG64 *)&_top, InterlockedIncrement64(&_l64KeyCount), (LONG64)localTop.pNode->pNext, (LONG64 *)&localTop));

		// Alloc ���
		pSeg = (SEGMENT *)localTop.pNode;
		pAllocData = &pSeg->data;
		pSeg->node.wMagicNum = dfMAGIC_NUM;

		//printf("[Alloc] NodePtr : 0x%p, DataPtr : 0x%p\n", &pSeg->node, pAllocData);

		// placement �ɼ� ON, Alloc���� ������ ȣ��
		if (m_bPlacementNew == true) {

			//������ ȣ��
			new(pAllocData) DATA();					// placementNew
		}

		// UseCount++
		InterlockedIncrement(&m_lUseCount);
		//UNLOCK(SrwMemPool);
		return pAllocData;
	}

	//////////////////////////////////////////////////////////////////////////
	// ������̴� ���� �����Ѵ�.	// LockFree Push ����
	//
	// Parameters: (DATA *) �� ������.
	// Return: (BOOL) TRUE, FALSE.
	//////////////////////////////////////////////////////////////////////////
	bool	Free(DATA *pData) 
	{
		//LOCK(SrwMemPool);
		SEGMENT *pSeg = (SEGMENT *)((st_NODE *)pData - 1); // st_NODE size��ŭ ����
		DATA *pFreeData = pData;

		//printf("[Free] NodePtr : 0x%p DataPtr : 0x%p\n", &pSeg->node, pFreeData);

		// MagicNum üũ, �̻��ϸ� false
		if (pSeg->node.wMagicNum != dfMAGIC_NUM) {
			printf("MagicNum Error / NodePtr : 0x%p\n", &pSeg->node);
			//UNLOCK(SrwMemPool);
			return false;
		}

		if (m_bPlacementNew == true) {

			//�Ҹ��� ȣ��
			DATA *pNowData = pData;
			pNowData->~DATA();
		}

		// LFPush �õ�, Free ���
		
		NODE *pLocalTop = NULL;
		pSeg->node.wMagicNum = 0;
		
		do {

			pLocalTop = _top.pNode;
			pSeg->node.pNext = pLocalTop;
			
		} while (InterlockedCompareExchange64((LONG64 *)&_top.pNode, (LONG64)&pSeg->node, (LONG64)pLocalTop) != (LONG64)pLocalTop);

		// UseCount-- 
		InterlockedDecrement(&m_lUseCount);
		//UNLOCK(SrwMemPool);
		return true;
	}

	
	//////////////////////////////////////////////////////////////////////////
	// ���� Ȯ�� �� �� ������ ��´�. (�޸�Ǯ ������ ��ü ����)
	//
	// Parameters: ����.
	// Return: (int) �޸� Ǯ ���� ��ü ����
	//////////////////////////////////////////////////////////////////////////
	int		GetAllocCount(void) { return m_lAllocCount; }

	//////////////////////////////////////////////////////////////////////////
	// ���� ������� �� ������ ��´�.
	//
	// Parameters: ����.
	// Return: (int) ������� �� ����.
	//////////////////////////////////////////////////////////////////////////
	int		GetUseCount(void) { return m_lUseCount; }

	//////////////////////////////////////////////////////////////////////////
	// ���� �̻������ �� ������ ��´�.
	//
	// Parameters: ����.
	// Return: (int) �̻������ �� ����.
	//////////////////////////////////////////////////////////////////////////
	int		GetFreeCount(void) { return m_lAllocCount - m_lUseCount; }

	//////////////////////////////////////////////////////////////////////////
	// ���� �̻������ �� ������ ��´�.
	//
	// Parameters: ����.
	// Return: (int) �̻������ �� ����.
	//////////////////////////////////////////////////////////////////////////
	LONG64	GetKeyCount(void) { return _l64KeyCount; }

private:

	//////////////////////////////////////////////////////////////////////////
	// Segment ���� ����(PUSH)
	//
	// Parameters: ����.
	// Return: ����
	//////////////////////////////////////////////////////////////////////////
	void Push()
	{
		SEGMENT *pSeg = (SEGMENT *)malloc(sizeof(SEGMENT));
		pSeg->node.pNext = NULL;
		pSeg->node.wMagicNum = dfMAGIC_NUM;
	
		// placement �ɼ� OFF 
		// Segment ������, placementNew�� ������ ȣ��
		if (m_bPlacementNew == false) {

			DATA *pNewData = &pSeg->data;
			new(pNewData) DATA();					// placementNew
		}

		if(_top.pNode != NULL)
			pSeg->node.pNext = _top.pNode;

		_top.pNode = &pSeg->node;

		// �� �Ҵ�� �޸� ����
		InterlockedIncrement(&m_lAllocCount);

		//printf("_FreeNode add : %d | _FreeNode->Next : %d\n", _pFreeNode, _pFreeNode->stpNextNode);
		DEBUGTEXT
	}

	//////////////////////////////////////////////////////////////////////////
	// Segment ����(POP)
	//
	// Parameters: ����.
	// Return: false(���� ���н�-���� �������), true(���� ������)
	//////////////////////////////////////////////////////////////////////////
	bool Pop() 
	{

		// ������ ����ٸ� false
		if (_top.pNode == NULL)
			return false;

		st_NODE *pNowNode = _top.pNode;

		// placement �ɼ� OFF 
		// �޸�Ǯ �Ҹ��� ȣ���, placementNew�� �Ҵ�� ��ü�� �Ҹ��� ȣ��
		if (m_bPlacementNew == false) {

			st_NODE *pForReleaseNode = pNowNode;
			pForReleaseNode += 1;					// 1 = sizeof(st_NODE)
			DATA *pForDestroyData = (DATA *)pForReleaseNode;
			pForDestroyData->~DATA();				// �Ҹ��� ȣ��
		}

		//printf("_FreeNode add : %d | _FreeNode->Next : %d\n", _pFreeNode, _pFreeNode->stpNextNode);
		DEBUGTEXT

		// free
		_top.pNode = _top.pNode->pNext;
		free(pNowNode);

		return true;
	}

//private:

	// ���� ������� ��ȯ�� (�̻��) ������Ʈ ���� ����.
	
	long m_lAllocCount;
	long m_lUseCount;

	bool m_bPlacementNew;

	NODE128 _top;
	LONG64	_l64KeyCount;
	
	SRWLOCK SrwMemPool;
};
#endif
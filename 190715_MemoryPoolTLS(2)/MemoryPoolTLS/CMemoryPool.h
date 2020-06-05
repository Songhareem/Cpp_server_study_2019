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

#pragma once

#ifndef  __MEMORY_POOL__
#define  __MEMORY_POOL__
#include <new.h>
#include <windows.h>

#define dfINIT_NUM	200
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
class CMemoryPool
{
private:

	/* **************************************************************** */
	// �� �� �տ� ���� ��� ����ü.
	/* **************************************************************** */
	struct st_NODE
	{
		// ������
		st_NODE()
		{
			stpNextNode = NULL;
			wMagicNum = dfMAGIC_NUM;
			//printf("st_NODE\n");
		}
		st_NODE *stpNextNode;
		WORD wMagicNum;
	};

	typedef struct st_SEGMENT
	{
		// ������
		st_SEGMENT()
		{
			//printf("st_SEGMENT\n");
		}
		st_NODE stNode;
		DATA Data;
	} SEGMENT;

public:

	//////////////////////////////////////////////////////////////////////////
	// ������, �ı���.
	//
	// Parameters:	(int) �ʱ� �� ����.
	//				(bool) Alloc �� ������ / Free �� �ı��� ȣ�� ����
	// Return:
	//////////////////////////////////////////////////////////////////////////
	CMemoryPool(int iBlockNum = dfINIT_NUM, bool bPlacementNew = false) 
		: m_lAllocCount(0), m_lUseCount(0), m_bPlacementNew(bPlacementNew)
	{
		InitializeSRWLock(&SrwMemPool);

		for (int i = 0; i < iBlockNum; i++)
			Push();
	}

	virtual	~CMemoryPool()
	{
		for (;;) {

			if (Pop() == false)
				break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// �� �ϳ��� �Ҵ�޴´�.  
	//
	// Parameters: ����.
	// Return: (DATA *) ����Ÿ �� ������.
	//////////////////////////////////////////////////////////////////////////
	DATA	*Alloc(void) 
	{
		LOCK(SrwMemPool);
		//������ ����ٸ� ���� push �� �Ŀ� �޸� �Ҵ�
		if (_pTopNode == NULL)
			Push();

		// Alloc ���
		SEGMENT *pSeg = (SEGMENT *)_pTopNode;
		DATA *pAllocData = &pSeg->Data;
		pSeg->stNode.wMagicNum = dfMAGIC_NUM;

		//printf("[0x%p]Chunk Alloc\n", pAllocData);
		//printf("[Alloc] NodePtr : 0x%p, DataPtr : 0x%p\n", &pSeg->stNode, pAllocData);

		// placement �ɼ� ON,
		// Alloc���� ������ ȣ��
		if (m_bPlacementNew == true) {

			//������ ȣ��
			new(pAllocData) DATA();					// placementNew
		}

		// FreeNode ���� ���� �̵�
		_pTopNode = _pTopNode->stpNextNode;
		UNLOCK(SrwMemPool);

		// UseCount++
		InterlockedIncrement(&m_lUseCount);

		return pAllocData;
	}

	//////////////////////////////////////////////////////////////////////////
	// ������̴� ���� �����Ѵ�.
	//
	// Parameters: (DATA *) �� ������.
	// Return: (BOOL) TRUE, FALSE.
	//////////////////////////////////////////////////////////////////////////
	bool	Free(DATA *pData) 
	{
		LOCK(SrwMemPool);
		SEGMENT *pSeg = (SEGMENT *)((st_NODE *)pData - 1); // st_NODE size��ŭ ����
		DATA *pFreeData = pData;

		//printf("[Free] NodePtr : 0x%p DataPtr : 0x%p\n", &pSeg->stNode, pFreeData);

		// MagicNum üũ, �̻��ϸ� false
		if (pSeg->stNode.wMagicNum != dfMAGIC_NUM) {
			printf("MagicNum Error / NodePtr : 0x%p\n", &pSeg->stNode);
			UNLOCK(SrwMemPool);
			return false;
		}

		if (m_bPlacementNew == true) {

			//�Ҹ��� ȣ��
			DATA *pNowData = pData;
			pNowData->~DATA();
		}
		//printf("[0x%p]Chunk Free\n",pFreeData);
		// Free ���
		pSeg->stNode.wMagicNum = 0;
		pSeg->stNode.stpNextNode = _pTopNode;
		_pTopNode = &pSeg->stNode;
		UNLOCK(SrwMemPool);

		// UseCount-- 
		InterlockedDecrement(&m_lUseCount);

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

private:
	// ������ �� �� ���� private�� �ű�	
	CMemoryPool() {}

	//////////////////////////////////////////////////////////////////////////
	// Segment ���� ����(PUSH)
	//
	// Parameters: ����.
	// Return: ����
	//////////////////////////////////////////////////////////////////////////
	void Push()
	{
		SEGMENT *pSeg = (SEGMENT *)malloc(sizeof(SEGMENT));
		pSeg->stNode.stpNextNode = NULL;
		pSeg->stNode.wMagicNum = dfMAGIC_NUM;
	
		// placement �ɼ� OFF 
		// Segment ������, placementNew�� ������ ȣ��
		if (m_bPlacementNew == false) {

			DATA *pNewData = &pSeg->Data;
			new(pNewData) DATA();					// placementNew
		}

		if(_pTopNode != NULL)
			pSeg->stNode.stpNextNode = _pTopNode;

		/*if (_pTopNode == NULL) {

			_pTopNode = &pSeg->stNode;
		}

		else {

			pSeg->stNode.stpNextNode = _pTopNode;
			_pTopNode = &pSeg->stNode;
		}*/

		_pTopNode = &pSeg->stNode;

		// �� �Ҵ�� �޸� ����
		InterlockedIncrement(&m_lAllocCount);

		//printf("_FreeNode add : %d | _FreeNode->Next : %d\n", _pFreeNode, _pFreeNode->stpNextNode);
		DEBUGTEXT
	}

	void PushNew() 
	{

		st_NODE *pNewNode;

		// placement �ɼ� OFF 
		// Segment ������, New�� ������ ȣ�� �� �޸� �Ҵ�
		if (m_bPlacementNew == false)
			pNewNode = (st_NODE *)new st_SEGMENT();	// st_SEGMENT ������ + st_NODE ������ + DATA() ȣ���

		// placement �ɼ� true
		// Segment ������, malloc ���� �޸� �Ҵ�
		else
			pNewNode = (st_NODE *)malloc(sizeof(st_SEGMENT));

		if (_pTopNode == NULL) {

			_pTopNode = pNewNode;
		}

		else {

			pNewNode->stpNextNode = _pTopNode;
			_pTopNode = pNewNode;
		}

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
		if (_pTopNode == NULL)
			return false;

		st_NODE *pNowNode = _pTopNode;

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
		_pTopNode = _pTopNode->stpNextNode;
		free(pNowNode);

		return true;
	}

	bool PopNew() 
	{

		// ������ ����ٸ� false
		if (_pTopNode == NULL)
			return false;

		st_NODE *pNowNode = _pTopNode;

		// placement �ɼ� OFF 
		// �޸�Ǯ �Ҹ��� ȣ���, New�� �Ҵ�� ��ü�� �Ҹ��� ȣ��
		if (m_bPlacementNew == false) {

			st_NODE *pForReleaseNode = pNowNode;
			pForReleaseNode += 1;					// 1 = sizeof(st_NODE)
			DATA *pForDestroyData = (DATA *)pForReleaseNode;
			pForDestroyData->~DATA();				// �Ҹ��� ȣ��
		}

		// placement �ɼ� ON 
		// �޸�Ǯ �Ҹ��� ȣ���, �׳� free

		//printf("_FreeNode add : %d | _FreeNode->Next : %d\n", _pFreeNode, _pFreeNode->stpNextNode);
		DEBUGTEXT

			// free
			_pTopNode = _pTopNode->stpNextNode;
		free(pNowNode);

		return true;
	}


private:

	// ���� ������� ��ȯ�� (�̻��) ������Ʈ ���� ����.
	st_NODE *_pTopNode;

	long m_lAllocCount;
	long m_lUseCount;

	bool m_bPlacementNew;

	SRWLOCK SrwMemPool;
};
#endif
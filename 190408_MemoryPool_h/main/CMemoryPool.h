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

//#define __DEBUG_MODE__

#ifdef __DEBUG_MODE__ 
#include <iostream>
#define DEBUGTEXT printf("_FreeNode add : %d | _FreeNode->Next : %d\n", _pFreeNode, _pFreeNode->stpNextNode);
#endif

#ifndef __DEBUG_MODE__
#define DEBUGTEXT
#endif

template <typename DATA>
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

	struct st_SEGMENT
	{
		// ������
		st_SEGMENT()
		{
			//printf("st_SEGMENT\n");
		}
		st_NODE stNode;
		DATA Data;
	};

public:

	//////////////////////////////////////////////////////////////////////////
	// ������, �ı���.
	//
	// Parameters:	(int) �ʱ� �� ����.
	//				(bool) Alloc �� ������ / Free �� �ı��� ȣ�� ����
	// Return:
	//////////////////////////////////////////////////////////////////////////
	CMemoryPool(int iBlockNum = dfINIT_NUM, bool bPlacementNew = false) 
		: m_iAllocCount(0), m_iUseCount(0), m_bPlacementNew(bPlacementNew)
	{
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

		//������ ����ٸ� ���� push �� �Ŀ� �޸� �Ҵ�
		if (_pFreeNode == NULL)
			Push();

		// Alloc ���
		st_NODE *pForMakeData = _pFreeNode;
		pForMakeData += 1;						// 1 = sizeof(st_NODE)
		DATA *pNewData = (DATA *)pForMakeData;

		// placement �ɼ� ON,
		// Alloc���� ������ ȣ��
		if (m_bPlacementNew == true) {

			//������ ȣ��
			new(pNewData) DATA();					// placementNew
		}

		// FreeNode ���� ���� �̵�
		_pFreeNode = _pFreeNode->stpNextNode;

		// UseCount++
		m_iUseCount++;

		return pNewData;
	}

	//////////////////////////////////////////////////////////////////////////
	// ������̴� ���� �����Ѵ�.
	//
	// Parameters: (DATA *) �� ������.
	// Return: (BOOL) TRUE, FALSE.
	//////////////////////////////////////////////////////////////////////////
	bool	Free(DATA *pData) 
	{

		st_NODE *pNowNode = (st_NODE *)pData;
		pNowNode -= 1;

		// MagicNum üũ, �̻��ϸ� false
		if (pNowNode->wMagicNum != dfMAGIC_NUM)
			return false;

		if (m_bPlacementNew == true) {

			//�Ҹ��� ȣ��
			DATA *pNowData = pData;
			pNowData->~DATA();
		}

		// Free ���
		pNowNode->stpNextNode = _pFreeNode;
		_pFreeNode = pNowNode;

		// UseCount-- 
		m_iUseCount--;

		return true;
	}

	
	//////////////////////////////////////////////////////////////////////////
	// ���� Ȯ�� �� �� ������ ��´�. (�޸�Ǯ ������ ��ü ����)
	//
	// Parameters: ����.
	// Return: (int) �޸� Ǯ ���� ��ü ����
	//////////////////////////////////////////////////////////////////////////
	int		GetAllocCount(void) { return m_iAllocCount; }

	//////////////////////////////////////////////////////////////////////////
	// ���� ������� �� ������ ��´�.
	//
	// Parameters: ����.
	// Return: (int) ������� �� ����.
	//////////////////////////////////////////////////////////////////////////
	int		GetUseCount(void) { return m_iUseCount; }

	//////////////////////////////////////////////////////////////////////////
	// ���� �̻������ �� ������ ��´�.
	//
	// Parameters: ����.
	// Return: (int) �̻������ �� ����.
	//////////////////////////////////////////////////////////////////////////
	int		GetFreeCount(void) { return m_iAllocCount - m_iUseCount; }

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

		st_NODE *pNewNode = (st_NODE *)malloc(sizeof(st_SEGMENT));
		//st_NODE *pNewNode = (st_NODE *)new st_SEGMENT();			// st_SEGMENT ������ + st_NODE ������ + DATA() ȣ���
		pNewNode->stpNextNode = NULL;
		pNewNode->wMagicNum = dfMAGIC_NUM;

		// placement �ɼ� OFF 
		// Segment ������, placementNew�� ������ ȣ��
		if (m_bPlacementNew == false) {

			st_NODE *pForMakeData = pNewNode;
			pForMakeData += 1;						// MovePtr sizeof(st_NODE *)
			DATA *pNewData = (DATA *)pForMakeData;
			new(pNewData) DATA();					// placementNew
		}

		if (_pFreeNode == NULL) {

			_pFreeNode = pNewNode;
		}

		else {

			pNewNode->stpNextNode = _pFreeNode;
			_pFreeNode = pNewNode;
		}

		// �� �Ҵ�� �޸� ����
		m_iAllocCount++;

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

		if (_pFreeNode == NULL) {

			_pFreeNode = pNewNode;
		}

		else {

			pNewNode->stpNextNode = _pFreeNode;
			_pFreeNode = pNewNode;
		}

		// �� �Ҵ�� �޸� ����
		m_iAllocCount++;

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
		if (_pFreeNode == NULL)
			return false;

		st_NODE *pNowNode = _pFreeNode;

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
			_pFreeNode = _pFreeNode->stpNextNode;
		free(pNowNode);

		return true;
	}

	bool PopNew() 
	{

		// ������ ����ٸ� false
		if (_pFreeNode == NULL)
			return false;

		st_NODE *pNowNode = _pFreeNode;

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
			_pFreeNode = _pFreeNode->stpNextNode;
		free(pNowNode);

		return true;
	}


private:

	// ���� ������� ��ȯ�� (�̻��) ������Ʈ ���� ����.
	st_NODE *_pFreeNode;

	int m_iAllocCount;
	int m_iUseCount;

	bool m_bPlacementNew;
};
#endif
#include "pch.h"
#include "CMemoryPool.h"

//#define __DEBUG_MODE__

#ifdef __DEBUG_MODE__ 
#include <iostream>
#define DEBUGTEXT printf("_FreeNode add : %d | _FreeNode->Next : %d\n", _pFreeNode, _pFreeNode->stpNextNode);
#endif

#ifndef __DEBUG_MODE__
#define DEBUGTEXT
#endif

template <typename DATA>
CMemoryPool<DATA>::CMemoryPool()
{
}

template <typename DATA>
CMemoryPool<DATA>::CMemoryPool(int iBlockNum, bool bPlacementNew)
	: m_iAllocCount(0), m_iUseCount(0), m_bPlacementNew(bPlacementNew)
{
	for (int i = 0; i < iBlockNum; i++)
		Push();
}

template <typename DATA>
CMemoryPool<DATA>::~CMemoryPool()
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
template <typename DATA>
DATA *CMemoryPool<DATA>::Alloc(void) {

	//������ ����ٸ� ���� push �� �Ŀ� �޸� �Ҵ�
	if (_pFreeNode == NULL)
		Push();

	// Alloc ���
	st_NODE *pForMakeData = _pFreeNode;
	pForMakeData += 1;						// 1 = sizeof(st_NODE)
	DATA *pNewData = (DATA *)pForMakeData;

	// placement �ɼ� ON,
	// Alloc���� ������ ȣ��
	if(m_bPlacementNew == true) {
		
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
template <typename DATA>
bool CMemoryPool<DATA>::Free(DATA *pData) {

	st_NODE *pNowNode = (st_NODE *)pData;
	pNowNode -= 1;

	// MagicNum üũ, �̻��ϸ� false
	if (pNowNode->wMagicNum != MAGIC_NUM)
		return false;

	if(m_bPlacementNew == true) {
		
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


template <typename DATA>
void CMemoryPool<DATA>::PushNew() {

	st_NODE *pNewNode;
	
	// placement �ɼ� OFF 
	// Segment ������, New�� ������ ȣ�� �� �޸� �Ҵ�
	if (m_bPlacementNew == false)
		pNewNode = (st_NODE *)new st_SEGMENT();	// st_SEGMENT ������ + st_NODE ������ + DATA() ȣ���

	// placement �ɼ� OFF 
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

template <typename DATA>
void CMemoryPool<DATA>::Push() {

	st_NODE *pNewNode = (st_NODE *)malloc(sizeof(st_SEGMENT));
	//st_NODE *pNewNode = (st_NODE *)new st_SEGMENT();			// st_SEGMENT ������ + st_NODE ������ + DATA() ȣ���
	pNewNode->stpNextNode = NULL;
	pNewNode->wMagicNum = MAGIC_NUM;

	// placement �ɼ� OFF 
	// Segment ������, placementNew�� ������ ȣ��
	if (m_bPlacementNew == false) {

		st_NODE *pForMakeData = pNewNode;
		pForMakeData += 1;						// 1 = sizeof(st_NODE)
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

template <typename DATA>
bool CMemoryPool<DATA>::Pop() {

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


template <typename DATA>
bool CMemoryPool<DATA>::PopNew() {

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

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
// 블럭 하나를 할당받는다.  
//
// Parameters: 없음.
// Return: (DATA *) 데이타 블럭 포인터.
//////////////////////////////////////////////////////////////////////////
template <typename DATA>
DATA *CMemoryPool<DATA>::Alloc(void) {

	//스택이 비었다면 새로 push 한 후에 메모리 할당
	if (_pFreeNode == NULL)
		Push();

	// Alloc 기능
	st_NODE *pForMakeData = _pFreeNode;
	pForMakeData += 1;						// 1 = sizeof(st_NODE)
	DATA *pNewData = (DATA *)pForMakeData;

	// placement 옵션 ON,
	// Alloc에서 생성자 호출
	if(m_bPlacementNew == true) {
		
		//생성자 호출
		new(pNewData) DATA();					// placementNew
	}

	// FreeNode 다음 노드로 이동
	_pFreeNode = _pFreeNode->stpNextNode;
	
	// UseCount++
	m_iUseCount++;

	return pNewData;
}

//////////////////////////////////////////////////////////////////////////
// 사용중이던 블럭을 해제한다.
//
// Parameters: (DATA *) 블럭 포인터.
// Return: (BOOL) TRUE, FALSE.
//////////////////////////////////////////////////////////////////////////
template <typename DATA>
bool CMemoryPool<DATA>::Free(DATA *pData) {

	st_NODE *pNowNode = (st_NODE *)pData;
	pNowNode -= 1;

	// MagicNum 체크, 이상하면 false
	if (pNowNode->wMagicNum != MAGIC_NUM)
		return false;

	if(m_bPlacementNew == true) {
		
		//소멸자 호출
		DATA *pNowData = pData;
		pNowData->~DATA();
	}

	// Free 기능
	pNowNode->stpNextNode = _pFreeNode;
	_pFreeNode = pNowNode;

	// UseCount-- 
	m_iUseCount--;

	return true;
}


template <typename DATA>
void CMemoryPool<DATA>::PushNew() {

	st_NODE *pNewNode;
	
	// placement 옵션 OFF 
	// Segment 생성시, New로 생성자 호출 및 메모리 할당
	if (m_bPlacementNew == false)
		pNewNode = (st_NODE *)new st_SEGMENT();	// st_SEGMENT 생성자 + st_NODE 생성자 + DATA() 호출됨

	// placement 옵션 OFF 
	// Segment 생성시, malloc 으로 메모리 할당
	else
		pNewNode = (st_NODE *)malloc(sizeof(st_SEGMENT));

	if (_pFreeNode == NULL) {

		_pFreeNode = pNewNode;
	}

	else {

		pNewNode->stpNextNode = _pFreeNode;
		_pFreeNode = pNewNode;
	}

	// 총 할당된 메모리 증가
	m_iAllocCount++;

	//printf("_FreeNode add : %d | _FreeNode->Next : %d\n", _pFreeNode, _pFreeNode->stpNextNode);
	DEBUGTEXT
}

template <typename DATA>
void CMemoryPool<DATA>::Push() {

	st_NODE *pNewNode = (st_NODE *)malloc(sizeof(st_SEGMENT));
	//st_NODE *pNewNode = (st_NODE *)new st_SEGMENT();			// st_SEGMENT 생성자 + st_NODE 생성자 + DATA() 호출됨
	pNewNode->stpNextNode = NULL;
	pNewNode->wMagicNum = MAGIC_NUM;

	// placement 옵션 OFF 
	// Segment 생성시, placementNew로 생성자 호출
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

	// 총 할당된 메모리 증가
	m_iAllocCount++;
	
	//printf("_FreeNode add : %d | _FreeNode->Next : %d\n", _pFreeNode, _pFreeNode->stpNextNode);
	DEBUGTEXT
}

template <typename DATA>
bool CMemoryPool<DATA>::Pop() {

	// 스택이 비었다면 false
	if (_pFreeNode == NULL)
		return false;

	st_NODE *pNowNode = _pFreeNode;
	
	// placement 옵션 OFF 
	// 메모리풀 소멸자 호출시, placementNew로 할당된 객체들 소멸자 호출
	if (m_bPlacementNew == false) {
		
		st_NODE *pForReleaseNode = pNowNode;
		pForReleaseNode += 1;					// 1 = sizeof(st_NODE)
		DATA *pForDestroyData = (DATA *)pForReleaseNode;
		pForDestroyData->~DATA();				// 소멸자 호출
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

	// 스택이 비었다면 false
	if (_pFreeNode == NULL)
		return false;

	st_NODE *pNowNode = _pFreeNode;

	// placement 옵션 OFF 
	// 메모리풀 소멸자 호출시, New로 할당된 객체들 소멸자 호출
	if (m_bPlacementNew == false) {

		st_NODE *pForReleaseNode = pNowNode;
		pForReleaseNode += 1;					// 1 = sizeof(st_NODE)
		DATA *pForDestroyData = (DATA *)pForReleaseNode;
		pForDestroyData->~DATA();				// 소멸자 호출
	}

	// placement 옵션 ON 
	// 메모리풀 소멸자 호출시, 그냥 free

	//printf("_FreeNode add : %d | _FreeNode->Next : %d\n", _pFreeNode, _pFreeNode->stpNextNode);
	DEBUGTEXT

	// free
	_pFreeNode = _pFreeNode->stpNextNode;
	free(pNowNode);

	return true;
}

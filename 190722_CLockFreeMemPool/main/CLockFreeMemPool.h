//-----------------------------------------------------------------------------------
// 공부 내용 정리
// 
// 0) 메모리풀 사용이유
// 속도 향상 목표(기존의 new - delete 대비)
// 메모리관리를 위해(메모리 관련 디버그 용도) 
//
// 1) template class / typename 차이?
// ref - https://www.ikpil.com/540 
//
// 2) class에서 template 사용시, 
// 내부 선언시에는 template 재선언 필요없음
// 외부 선언시에는 함수(생성자, 소멸자 포함) 마다 재선언필요
// 내부 선언시 h파일에 함수 선언 및 코드 작성 + include h파일
// 외부 선언시 h파일에 함수 선언 / CPP파일에 코드 작성 + include h파일 및 CPP파일
//
// 3-1) placementNew FALSE ?
// push때 메모리 확보 및 생성자 호출, pop때 메모리 해제 및 파괴자 호출
// Free받았다가 다시 Alloc 시에는 생성자를 미호출 (Free시에 받은 메모리를 pop해야하는지?)
// 일반변수 및 생성자 필요없는 클래스 메모리 할당시 사용
//
// 3-2) placementNew TRUE ?
// push때 메모리 확보, pop때 메모리 해제
// Alloc때 생성자 호출, Free때 파괴자 호출
// 생성자가 필요한 클래스 메모리 할당시 사용
//
// 4) 클래스 크기
// 비어있다면(생성자 및 소멸자 존재 + 변수 X) 1byte
// 변수 있다면 sizeof(총 변수)
//
// 5) st_SEGMENT의 경우, 
// new st_SEGMENT를 했을때, 내부에 있는 생성자가 여러개 호출
// 이게 new가 여러번 불리는것인지, 생성자만 여러개 호출되는 것인지?
// new 한번 호출에 생성자 여러번 호출되는것
// 
// 6) template 클래스에서인텔리센스?
// 작동 잘 안함
// template template 안에서는 아예 작동 안함
//
// 7) 노드안에 데이터가 아닌 데이터 안에 노드를 놓고 쓰는이유? (속도 향상 목적)
// list로 사용시, list 외부에서 한번 생성(new or 그냥 생성)해서 리스트에 인자로 넣고,
// list 내부에서도 new로 메모리 확보 후 인자로 받은 것의 DATA를 카피
// 결국 2번 new하게 되는 상황발생 (속도 떨어짐)
// DATA안에 노드를 놓고 사용하게 되면, new 한번으로 끝낼 수 있음(속도 향상) 
// 
// 8) ::operator new(sizeof(type))
// new의 오리진, 타입 사이즈를 스스로 계산하는게 아니라 우리가 넣어줘야함 
// 사이즈 들어가므로 생성자 호출 X
//-----------------------------------------------------------------------------------

#ifndef  __LF_MEMORY_POOL__
#define  __LF_MEMORY_POOL__

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

//template <typename DATA>
typedef int DATA;
class CLockFreeMemPool
{
private:

	/* **************************************************************** */
	// 각 블럭 앞에 사용될 노드 구조체.
	/* **************************************************************** */
	struct st_NODE
	{
		// 생성자
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
		// 생성자
		st_SEGMENT()
		{
			//printf("st_SEGMENT\n");
		}
		st_NODE stNode;
		DATA Data;
	} SEGMENT;

	typedef struct st_NODE128
	{
		// 생성자
		st_NODE128()
		{
			pSeg = NULL;
			l64Key = 0;
		};
		SEGMENT	*pSeg;
		LONG64	l64Key;

	} NODE128;

public:

	//////////////////////////////////////////////////////////////////////////
	// 생성자, 파괴자.
	//
	// Parameters:	(int) 초기 블럭 개수.
	//				(bool) Alloc 시 생성자 / Free 시 파괴자 호출 여부
	// Return:
	//////////////////////////////////////////////////////////////////////////
	CLockFreeMemPool(int iBlockNum = dfINIT_NUM, bool bPlacementNew = false) 
		: _lAllocCount(0), _lUseCount(0), _bPlacementNew(bPlacementNew)
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
	// 블럭 하나를 할당받는다.  
	//
	// Parameters: 없음.
	// Return: (DATA *) 데이타 블럭 포인터.
	//////////////////////////////////////////////////////////////////////////
	DATA	*Alloc(void) 
	{
		//스택이 비었다면 새로 push 한 후에 메모리 할당
		if (_pTopNode == NULL)
			Push();

		// Alloc 기능
		SEGMENT *pSeg = (SEGMENT *)_pTopNode;
		DATA *pAllocData = &pSeg->Data;
		pSeg->stNode.wMagicNum = dfMAGIC_NUM;

		//printf("[0x%p]Chunk Alloc\n", pAllocData);
		//printf("[Alloc] NodePtr : 0x%p, DataPtr : 0x%p\n", &pSeg->stNode, pAllocData);

		// placement 옵션 ON,
		// Alloc에서 생성자 호출
		if (_bPlacementNew == true) {

			//생성자 호출
			new(pAllocData) DATA();					// placementNew
		}

		// FreeNode 다음 노드로 이동 ( LockFreeStack Pop 기능 )
		_pTopNode = _pTopNode->stpNextNode;
		
		// UseCount++
		InterlockedIncrement(&_lUseCount);

		return pAllocData;
	}

	//////////////////////////////////////////////////////////////////////////
	// 사용중이던 블럭을 해제한다.
	//
	// Parameters: (DATA *) 블럭 포인터.
	// Return: (BOOL) TRUE, FALSE.
	//////////////////////////////////////////////////////////////////////////
	bool	Free(DATA *pData) 
	{
		LOCK(SrwMemPool);
		SEGMENT *pSeg = (SEGMENT *)((st_NODE *)pData - 1); // st_NODE size만큼 빼기
		DATA *pFreeData = pData;

		//printf("[Free] NodePtr : 0x%p DataPtr : 0x%p\n", &pSeg->stNode, pFreeData);

		// MagicNum 체크, 이상하면 false
		if (pSeg->stNode.wMagicNum != dfMAGIC_NUM) {
			printf("MagicNum Error / NodePtr : 0x%p\n", &pSeg->stNode);
			UNLOCK(SrwMemPool);
			return false;
		}

		if (_bPlacementNew == true) {

			//소멸자 호출
			DATA *pNowData = pData;
			pNowData->~DATA();
		}
		//printf("[0x%p]Chunk Free\n",pFreeData);
		// Free 기능
		pSeg->stNode.wMagicNum = 0;
		pSeg->stNode.stpNextNode = _pTopNode;
		_pTopNode = &pSeg->stNode;
		UNLOCK(SrwMemPool);

		// UseCount-- 
		InterlockedDecrement(&_lUseCount);

		return true;
	}

	
	//////////////////////////////////////////////////////////////////////////
	// 현재 확보 된 블럭 개수를 얻는다. (메모리풀 내부의 전체 개수)
	//
	// Parameters: 없음.
	// Return: (int) 메모리 풀 내부 전체 개수
	//////////////////////////////////////////////////////////////////////////
	int		GetAllocCount(void) { return _lAllocCount; }

	//////////////////////////////////////////////////////////////////////////
	// 현재 사용중인 블럭 개수를 얻는다.
	//
	// Parameters: 없음.
	// Return: (int) 사용중인 블럭 개수.
	//////////////////////////////////////////////////////////////////////////
	int		GetUseCount(void) { return _lUseCount; }

	//////////////////////////////////////////////////////////////////////////
	// 현재 미사용중인 블럭 개수를 얻는다.
	//
	// Parameters: 없음.
	// Return: (int) 미사용중인 블럭 개수.
	//////////////////////////////////////////////////////////////////////////
	int		GetFreeCount(void) { return _lAllocCount - _lUseCount; }

private:
	// 유저가 쓸 수 없게 private로 옮김	
	CLockFreeMemPool() {}

	//////////////////////////////////////////////////////////////////////////
	// Segment 새로 생성(PUSH)
	//
	// Parameters: 없음.
	// Return: 없음
	//////////////////////////////////////////////////////////////////////////
	void Push()
	{
		SEGMENT *pSeg = (SEGMENT *)malloc(sizeof(SEGMENT));
		pSeg->stNode.stpNextNode = NULL;
		pSeg->stNode.wMagicNum = dfMAGIC_NUM;

		// placement 옵션 OFF 
		// Segment 생성시, placementNew로 생성자 호출
		if (_bPlacementNew == false) {

			DATA *pNewData = &pSeg->Data;
			new(pNewData) DATA();					// placementNew
		}

		if (_pTopNode != NULL)
			pSeg->stNode.stpNextNode = _pTopNode;

		_pTopNode = &pSeg->stNode;

		// 총 할당된 메모리 증가
		InterlockedIncrement(&_lAllocCount);

		//printf("_FreeNode add : %d | _FreeNode->Next : %d\n", _pFreeNode, _pFreeNode->stpNextNode);
		DEBUGTEXT
	}

	//////////////////////////////////////////////////////////////////////////
	// Segment 삭제(POP)
	//
	// Parameters: 없음.
	// Return: false(삭제 실패시-스택 비었을때), true(삭제 성공시)
	//////////////////////////////////////////////////////////////////////////
	bool Pop() 
	{
		// 스택이 비었다면 false
		if (_pTopNode == NULL)
			return false;

		st_NODE *pNowNode = _pTopNode;

		// placement 옵션 OFF 
		// 메모리풀 소멸자 호출시, placementNew로 할당된 객체들 소멸자 호출
		if (_bPlacementNew == false) {

			st_NODE *pForReleaseNode = pNowNode;
			pForReleaseNode += 1;					// 1 = sizeof(st_NODE)
			DATA *pForDestroyData = (DATA *)pForReleaseNode;
			pForDestroyData->~DATA();				// 소멸자 호출
		}

		//printf("_FreeNode add : %d | _FreeNode->Next : %d\n", _pFreeNode, _pFreeNode->stpNextNode);
		DEBUGTEXT

		// free
		_pTopNode = _pTopNode->stpNextNode;
		free(pNowNode);

		return true;
	}

private:

	// 스택 방식으로 반환된 (미사용) 오브젝트 블럭을 관리.
	NODE128 *_pTopNode;

	long _lAllocCount;
	long _lUseCount;

	bool _bPlacementNew;

	SRWLOCK SrwMemPool;
};
#endif
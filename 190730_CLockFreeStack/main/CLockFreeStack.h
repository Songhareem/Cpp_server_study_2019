#pragma once

#include <Windows.h>
#include <list>
#include "CLockFreeMemPool.h"

using namespace std;

template<class DATA>
//typedef int DATA;
class CLockFreeStack
{
private:
	typedef struct st_NODE {

		st_NODE *pNext = NULL;
		DATA	data;
	}NODE;

	_declspec(align(16)) typedef struct st_NODE128 {

		NODE	*pNode = NULL;
		LONG64	l64Key = 0;

	}NODE128;

	void Init() {

		_pHead = new NODE;
		_top.pNode = _pHead;
	}

	void Final() {}

public:
	CLockFreeStack() {
		//Init();
	}
	~CLockFreeStack() {
		Final();
	}

	void	Push(DATA data) {

		NODE *pLocalTop = NULL;
		NODE *pNew = _LFMemPool.Alloc();
		pNew->data = data;

		do {

			pLocalTop = _top.pNode;
			pNew->pNext = pLocalTop;

		} while (InterlockedCompareExchange64((LONG64 *)&_top.pNode, (LONG64)pNew, (LONG64)pLocalTop) != (LONG64)pLocalTop);

		//_top.l64Key = InterlockedIncrement64((LONGLONG *)&_l64KeyCount);
		InterlockedIncrement(&_lStackSize);
	}

	bool	Pop(DATA *pData) {

		NODE128 localTop;
		NODE	*pDel = NULL;
		do {

			localTop = _top;

		} while (!InterlockedCompareExchange128((LONG64 *)&_top, InterlockedIncrement64(&_l64KeyCount), (LONG64)localTop.pNode->pNext, (LONG64 *)&localTop));

		InterlockedDecrement(&_lStackSize);
		*pData = localTop.pNode->data;
		_LFMemPool.Free(localTop.pNode);
		//delete localTop.pNode;
		return true;
	}

	long	GetStackSize() { return _lStackSize; }
	LONG64	GetKeyCount() { return _l64KeyCount; }

private:
	NODE128		_top;
	NODE		*_pHead;
	long		_lStackSize;
	LONG64		_l64KeyCount;

	CLockFreeMemPool<NODE> _LFMemPool;
};


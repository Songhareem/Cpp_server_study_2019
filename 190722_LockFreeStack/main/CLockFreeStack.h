#pragma once

#include <Windows.h>
#include <list>

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

	_declspec(align(128)) typedef struct st_NODE128 {

		NODE	*pNode = NULL;
		NODE	*pNext = NULL;
		//LONG64	l64Key = 0;

	}NODE128;

	void Init() {
		
		_pHead = new NODE;
		_top.pNode = _pHead;
	}

	void Final() {}

public:
	CLockFreeStack() {
		Init();
	}
	~CLockFreeStack() {
		Final();
	}

	void	Push(DATA data) {
	
		NODE128 cmpNode;
		//NODE *pCmpTop = NULL;
		NODE *pNew = new NODE;
		pNew->data = data;

		do {
			
			cmpNode = _top;
			pNew->pNext = cmpNode.pNode;
			
			//pCmpTop = _top.pNode;
			//pNew->pNext = pCmpTop;

		//} while (InterlockedCompareExchange64((LONG64 *)&_top.pNode, (LONG64)pNew, (LONG64)pCmpTop) != (LONG64)pCmpTop);
		} while (!InterlockedCompareExchange128((LONG64 *)&_top, (LONG64)cmpNode.pNode, (LONG64)pNew, (LONG64 *)&cmpNode));

		//_top.l64Key = InterlockedIncrement64((LONGLONG *)&_l64KeyCount);
		InterlockedIncrement(&_lStackSize);
	}

	bool	Pop(DATA *pData) {
	
		NODE128 cmpNode;
		//NODE *pDel;
		do {

			cmpNode = _top;
			//pDel = cmpNode.pNode;
			//cmpNode.pNode->pNext;

		//} while (!InterlockedCompareExchange128((LONG64 *)&_top, InterlockedIncrement64(&_l64KeyCount), (LONG64)_top.pNode->pNext, (LONG64 *)&cmpNode));
		} while (!InterlockedCompareExchange128(
			(LONG64 *)&_top, 
			(LONG64)cmpNode.pNext->pNext, 
			(LONG64)cmpNode.pNext, 
			(LONG64 *)&cmpNode));

		InterlockedDecrement(&_lStackSize);
		*pData = cmpNode.pNode->data;
		delete cmpNode.pNode;

		return true;
	}

	long	GetStackSize() { return _lStackSize; }
	LONG64	GetKeyCount() { return _l64KeyCount; }
private:
	NODE128		_top;
	NODE		*_pHead;
	long		_lStackSize;
	LONG64		_l64KeyCount;
};


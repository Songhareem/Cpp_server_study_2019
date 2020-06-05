
// Enq Deq Enq 시 문제 발생 (Head가 붕 뜨는 문제)
// -> Head와 Tail 위치가 같을 때,
// -> 첫번째 Enq에서 Next만 연결후, 스위칭
// -> 첫번째 Deq 성공으로 Head의 위치가 Next 노드로 이동
// -> 두번째 Enq 성공으로 Head와는 다른 새 노드로 Tail이 이동
// >> 해결 : InterlockedCompareExchangePointer((PVOID *)&_tail.pNode->pNext, (PVOID)pAlloc, (PVOID)localTail.pNode->pNext) == NULL 로 해결
// 
// Deq Deq Enq시 문제 발생 ( Data 중복으로 나가는 현상 )
// -> 노드 1 - 2 - 3이 있다고 가정,
// -> 첫번째 Deq Head 위치를 1 -> 2로 바꾸고 스위칭,
// -> 두번째 Deq에 성공으로 2 가 해제, Head는 3으로 이동
// -> 첫번째 Enq로 해제된 2가 연결됨, 1 - 2 - 3 - 2
// -> 첫번째 Deq에서 LocalHead->Next Data를 뱉으므로, 2(새로 Enq된)의 데이터를 뱉음
// >> 해결 : Head 바꾸기 전에 Data 변수에 미리 받아놓고 작업 성공시, Data를 뱉어줌
//
// Deq Deq Enq Deq Deq시 문제 발생 2 ( Head가 NULL )
// -> 노드 1 - 2 이 있다고 가정
// -> 첫번째 Deq시, LocalHead = Head까지 진행 후 스위칭
// -> 두번째 Deq시, 성공으로 Head는 1->2로 이동, 1 Free
// -> 첫번째 Enq시, 성공으로 1 Alloc, 1의 Next는 NULL
// -> 세번째 Deq시, 성공으로 2 Free, 1로 Head가 이동
// -> 첫번째 Deq에서, LocalHead(주소 1)와 Head(주소 1)은 같으므로 Head는 Next(NULL)로 이동
// -> LocalHead->Next는 2이므로, 안터지고 2의 값 뱉음
// -> 네번째 Deq시, Head는 NULL이므로 Head->Next 접근시 에러
// >> 해결 : Interlocked128을 이용해 작업의 순서 보장
//
// Enq Enq Deq Enq (메모리 재할당 문제 -> LocalTail / Tail->next != NULL 이므로 Enq 무한반복)
// -> 노드 1 이 있다고 가정 (Head = Tail)
// -> 첫번째 Enq시, pNext == NULL을 통과 후, 스위칭  
// -> 두번째 Enq시, 성공으로 2 번 노드 연결, 1 - 2, Tail 이동
// -> 첫번째 Deq시, 성공으로 1 Free, Head 1->2로 이동
// -> 세번째 Enq시, 성공으로 1 Alloc, 1->next = NULL, tail 2->1로 이동
// -> 첫번째 Enq에서, pNext와 tail->next 주소가 NULL로 같으므로 첫번째 CAS 통과
// -> 두번째 CAS는 key값이 다르므로 실패
// >> 해결 : 첫번째 CAS를 통과했다면 반드시 성공시켜줘야함, for문 및 key값 증가, InterlockCmpEx128의 성질을 이용해 해결
//
// Enq x N ( pNext != NULL 이므로 Enq 무한반복 )
// -> 노드 1이 있다고 가정 ( tail은 1에 있음)
// -> 첫번째 Enq시, pNext == NULL을 통과 후, 스위칭
// -> 두번째 Enq가 DoubleCAS까지 성공 후, retuen전에 스위칭, 노드는 1 - 3, tail은 3에 있지만, size는 증가 안됐음 (작업 미완료)
// -> 두번째 Enq가 작업이 미완료임에도, 첫번째 Enq에서 pNext(노드 1의 Next, NULL)와 tail->next(노드 3의 Next, NULL)는 같으므로 CAS 통과
// -> 노드는 1 - 3 - 2가 되고, DoubleCAS 실패
// -> 이후, 두번째 노드가 정상적으로 성공하던 못하던, Enq시, tail ->next는 NULL이 아니므로 무한반복
// >> 해결: 첫번째 CAS를 통과했다면 나머지는 다 실패해야 함, Alloc->pNext = NULL 구문을 DoubleCAS 안에서 해주므로써, 
//          재할당 받은 상태인 Alloc의 pNext는 NULL이 아니게 된다, 따라서 첫번째 CAS를 통과하지 못함

#pragma once

#include <string>
#include "MemoryPool.h"

#define	dfMAX_DEBUG		10000

using namespace std;

template<class DATA>
//typedef __int64	DATA;
class CLockFreeQ
{
// 내부 구조체
private:
	typedef struct st_NODE 
	{
		st_NODE *pNext;
		DATA	data;
	} NODE;

	typedef struct st_DEBUG
	{
		LONG64  keyCount;
		NODE*	pNode;
		LONG64  node;
		LONG64	next;

	} DEBUG;

	_declspec(align(16)) typedef struct st_NODE128
	{
		NODE*	pNode;
		LONG64	keyCount;
	} NODE128;

// 함수부
public:
	CLockFreeQ() 
	{
		Init();
	}
	~CLockFreeQ() 
	{
		Final();
	}

	void Enque(DATA data)
	{
		NODE128 localTail;
		LONG64	keyCount;
		NODE	*pNext = NULL;
		NODE	*pAlloc = _LFMemPool.Alloc();
		//pAlloc->pNext = NULL;
		pAlloc->data = data;

		for (;;) {

			localTail = _tail;
			pNext = localTail.pNode->pNext;

			if (pNext != NULL)
				continue;

			// tail.pNode->pNext == NULL이고, pNext가 NULL 이면 통과 // 문제있음
			if (InterlockedCompareExchangePointer((PVOID *)&_tail.pNode->pNext, (PVOID)pAlloc, (PVOID)pNext) == NULL) {
			
				for (;;) {

					keyCount = InterlockedIncrement64(&_l64Key); // 매 시도마다, 매 실패시마다 Count++
					if (InterlockedCompareExchange128((LONG64 *)&_tail, keyCount, (LONG64)pAlloc, (LONG64 *)&localTail))
					{
						/*DEBUG tmp;
						tmp.keyCount = _l64Key;
						tmp.node = (LONG64)pAlloc;
						tmp.next = (LONG64)pAlloc->pNext;
						tmp.pNode = pAlloc;
						allocNodes[(keyCount % dfMAX_DEBUG)] = tmp;

						DEBUG tmp2;
						tmp2.keyCount = 0;
						tmp2.node = 0;
						tmp2.next = 0;
						tmp2.pNode = NULL;
						freeNodes[(keyCount % dfMAX_DEBUG)] = tmp2;*/

						pAlloc->pNext = NULL;

						InterlockedIncrement64((LONG64 *)&_l64Size);
						return;
					}
				}
			}
		}
		
	}

	bool Deque(DATA *pData)
	{
		NODE128 localHead;
		LONG64	keyCount;
		NODE	*pNext = NULL;
		DATA	data = NULL;

		if (InterlockedDecrement64((LONG64 *)&_l64Size) < 0) {
			InterlockedIncrement64((LONG64 *)&_l64Size);
			return false;
		}
	
		for (;;) {

			localHead = _head;
			pNext = localHead.pNode->pNext;
			if (pNext == NULL)
				continue;
			
			data = pNext->data;

			keyCount = InterlockedIncrement64(&_l64Key);

			if (InterlockedCompareExchange128((LONG64 *)&_head, keyCount, (LONG64)pNext, (LONG64 *)&localHead)) {

				/*DEBUG tmp;
				tmp.keyCount = _l64Key;
				tmp.node = (LONG64)localHead.pNode;
				tmp.next = (LONG64)pNext;
				tmp.pNode = localHead.pNode;
				freeNodes[keyCount % dfMAX_DEBUG] = tmp;
				
				DEBUG tmp2;
				tmp2.keyCount = 0;
				tmp2.node = 0;
				tmp2.next = 0;
				tmp2.pNode = NULL;
				allocNodes[(keyCount % dfMAX_DEBUG)] = tmp2;*/

				*pData = data;
				_LFMemPool.Free(localHead.pNode);

				break;
			}
		}

		return true;
	}

	LONG64 GetQueSize() { return _l64Size; }

private:
	void Init() 
	{
		_head.pNode = _LFMemPool.Alloc();
		_head.pNode->pNext = NULL;
		_tail = _head;

		_l64Size = 0;
	}

	void Final() 
	{
		_LFMemPool.~CMemoryPool();
	}

// 변수부
//private:

	NODE128 _head;
	NODE128 _tail;

	LONG64	_l64Size;
	LONG64  _l64Key = 0;
	
	DEBUG allocNodes[dfMAX_DEBUG];
	DEBUG freeNodes[dfMAX_DEBUG];
	CMemoryPool<NODE> _LFMemPool;
};
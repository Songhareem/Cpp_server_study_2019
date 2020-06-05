
// Enq Deq Enq �� ���� �߻� (Head�� �� �ߴ� ����)
// -> Head�� Tail ��ġ�� ���� ��,
// -> ù��° Enq���� Next�� ������, ����Ī
// -> ù��° Deq �������� Head�� ��ġ�� Next ���� �̵�
// -> �ι�° Enq �������� Head�ʹ� �ٸ� �� ���� Tail�� �̵�
// >> �ذ� : InterlockedCompareExchangePointer((PVOID *)&_tail.pNode->pNext, (PVOID)pAlloc, (PVOID)localTail.pNode->pNext) == NULL �� �ذ�
// 
// Deq Deq Enq�� ���� �߻� ( Data �ߺ����� ������ ���� )
// -> ��� 1 - 2 - 3�� �ִٰ� ����,
// -> ù��° Deq Head ��ġ�� 1 -> 2�� �ٲٰ� ����Ī,
// -> �ι�° Deq�� �������� 2 �� ����, Head�� 3���� �̵�
// -> ù��° Enq�� ������ 2�� �����, 1 - 2 - 3 - 2
// -> ù��° Deq���� LocalHead->Next Data�� �����Ƿ�, 2(���� Enq��)�� �����͸� ����
// >> �ذ� : Head �ٲٱ� ���� Data ������ �̸� �޾Ƴ��� �۾� ������, Data�� �����
//
// Deq Deq Enq Deq Deq�� ���� �߻� 2 ( Head�� NULL )
// -> ��� 1 - 2 �� �ִٰ� ����
// -> ù��° Deq��, LocalHead = Head���� ���� �� ����Ī
// -> �ι�° Deq��, �������� Head�� 1->2�� �̵�, 1 Free
// -> ù��° Enq��, �������� 1 Alloc, 1�� Next�� NULL
// -> ����° Deq��, �������� 2 Free, 1�� Head�� �̵�
// -> ù��° Deq����, LocalHead(�ּ� 1)�� Head(�ּ� 1)�� �����Ƿ� Head�� Next(NULL)�� �̵�
// -> LocalHead->Next�� 2�̹Ƿ�, �������� 2�� �� ����
// -> �׹�° Deq��, Head�� NULL�̹Ƿ� Head->Next ���ٽ� ����
// >> �ذ� : Interlocked128�� �̿��� �۾��� ���� ����
//
// Enq Enq Deq Enq (�޸� ���Ҵ� ���� -> LocalTail / Tail->next != NULL �̹Ƿ� Enq ���ѹݺ�)
// -> ��� 1 �� �ִٰ� ���� (Head = Tail)
// -> ù��° Enq��, pNext == NULL�� ��� ��, ����Ī  
// -> �ι�° Enq��, �������� 2 �� ��� ����, 1 - 2, Tail �̵�
// -> ù��° Deq��, �������� 1 Free, Head 1->2�� �̵�
// -> ����° Enq��, �������� 1 Alloc, 1->next = NULL, tail 2->1�� �̵�
// -> ù��° Enq����, pNext�� tail->next �ּҰ� NULL�� �����Ƿ� ù��° CAS ���
// -> �ι�° CAS�� key���� �ٸ��Ƿ� ����
// >> �ذ� : ù��° CAS�� ����ߴٸ� �ݵ�� �������������, for�� �� key�� ����, InterlockCmpEx128�� ������ �̿��� �ذ�
//
// Enq x N ( pNext != NULL �̹Ƿ� Enq ���ѹݺ� )
// -> ��� 1�� �ִٰ� ���� ( tail�� 1�� ����)
// -> ù��° Enq��, pNext == NULL�� ��� ��, ����Ī
// -> �ι�° Enq�� DoubleCAS���� ���� ��, retuen���� ����Ī, ���� 1 - 3, tail�� 3�� ������, size�� ���� �ȵ��� (�۾� �̿Ϸ�)
// -> �ι�° Enq�� �۾��� �̿Ϸ��ӿ���, ù��° Enq���� pNext(��� 1�� Next, NULL)�� tail->next(��� 3�� Next, NULL)�� �����Ƿ� CAS ���
// -> ���� 1 - 3 - 2�� �ǰ�, DoubleCAS ����
// -> ����, �ι�° ��尡 ���������� �����ϴ� ���ϴ�, Enq��, tail ->next�� NULL�� �ƴϹǷ� ���ѹݺ�
// >> �ذ�: ù��° CAS�� ����ߴٸ� �������� �� �����ؾ� ��, Alloc->pNext = NULL ������ DoubleCAS �ȿ��� ���ֹǷν�, 
//          ���Ҵ� ���� ������ Alloc�� pNext�� NULL�� �ƴϰ� �ȴ�, ���� ù��° CAS�� ������� ����

#pragma once

#include <string>
#include "MemoryPool.h"

#define	dfMAX_DEBUG		10000

using namespace std;

template<class DATA>
//typedef __int64	DATA;
class CLockFreeQ
{
// ���� ����ü
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

// �Լ���
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

			// tail.pNode->pNext == NULL�̰�, pNext�� NULL �̸� ��� // ��������
			if (InterlockedCompareExchangePointer((PVOID *)&_tail.pNode->pNext, (PVOID)pAlloc, (PVOID)pNext) == NULL) {
			
				for (;;) {

					keyCount = InterlockedIncrement64(&_l64Key); // �� �õ�����, �� ���нø��� Count++
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

// ������
//private:

	NODE128 _head;
	NODE128 _tail;

	LONG64	_l64Size;
	LONG64  _l64Key = 0;
	
	DEBUG allocNodes[dfMAX_DEBUG];
	DEBUG freeNodes[dfMAX_DEBUG];
	CMemoryPool<NODE> _LFMemPool;
};
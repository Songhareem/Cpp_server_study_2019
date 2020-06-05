
#include<iostream>

template <typename T>
class CList
{
public:
	struct Node
	{
		T _data;
		Node *_Prev;
		Node *_Next;
	};

	class iterator
	{
	private:
		Node *_node;
	
	public:
		iterator(Node *node = nullptr)
		{
			//인자로 들어온 Node 포인터를 저장
			_node = node;
		}

		iterator operator ++()
		{
			//현재 노드를 다음 노드로 이동
			_node = _node->_Next;
		}

		iterator operator --()
		{
			//현재 노드를 다음 노드로 이동
			_node = _node->_Prev;
		}

		T& operator *()
		{
			//현재 노드의 데이터를 뽑음
			return _node;
		}
	};

private:
	int _size = 0;
	Node _head;
	Node _tail;

public:
	CList() {

		this._head.prev = NULL;
		this._head.next = &(this._tail);

		this._tail.prev = &(this._head);
		this._tail.next = NULL;
	}
	~CList();

	iterator begin()
	{
		//첫번째 노드를 가리키는 이터레이터 리턴
	}
	iterator end()
	{
		//Tail 노드를 가리키는(데이터가 없는 진짜 끝 노드) 이터레이터를 리턴
		//	또는 끝으로 인지할 수 있는 이터레이터를 리턴
	}

	void push_front(T data) {

		Node *node;
		Node *newNode = new Node;
		
		newNode->_data = data;
		newNode->_Prev = &_head;
		newNode->_Next = _head._Next;

		node = &_head;
		node = node->_Next;
		node->_Prev = newNode;

		node = &_head;
		node->_Next = newNode;

		delete newNode;
	}

	void push_back(T data);
	void clear();
	int size() { return _size; };
	bool empty() { };

	iterator erase(iterator itor);
		//- 이터레이터의 그 노드를 지움.
		//- 그리고 지운 노드의 다음 노드를 카리키는 이터레이터 리턴
};

/*//////////////// 순회 샘플 코드 /////////////////////////


CList<int> ListInt;

CList<int>::iterator iter;
for (iter = ListInt.begin(); iter != ListInt.end(); ++iter)
{
	printf("%d", *iter);
}

/////////////////////////////////////////////////////////*/


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
			//���ڷ� ���� Node �����͸� ����
			_node = node;
		}

		iterator operator ++()
		{
			//���� ��带 ���� ���� �̵�
			_node = _node->_Next;
		}

		iterator operator --()
		{
			//���� ��带 ���� ���� �̵�
			_node = _node->_Prev;
		}

		T& operator *()
		{
			//���� ����� �����͸� ����
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
		//ù��° ��带 ����Ű�� ���ͷ����� ����
	}
	iterator end()
	{
		//Tail ��带 ����Ű��(�����Ͱ� ���� ��¥ �� ���) ���ͷ����͸� ����
		//	�Ǵ� ������ ������ �� �ִ� ���ͷ����͸� ����
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
		//- ���ͷ������� �� ��带 ����.
		//- �׸��� ���� ����� ���� ��带 ī��Ű�� ���ͷ����� ����
};

/*//////////////// ��ȸ ���� �ڵ� /////////////////////////


CList<int> ListInt;

CList<int>::iterator iter;
for (iter = ListInt.begin(); iter != ListInt.end(); ++iter)
{
	printf("%d", *iter);
}

/////////////////////////////////////////////////////////*/

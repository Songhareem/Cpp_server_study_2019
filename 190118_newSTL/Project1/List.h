
#ifndef MYLIST
#define MYLIST

template <typename T>
class list
{
private:

	struct Node
	{
		Node* _Prev;
		Node* _Next;
		T _Data;

		Node(T data = 0) :_Data(data)
		{
			_Prev = _Next = 0;
		}
	};

	int _size;
	Node *_head;
	Node *_tail;

public:

	class iterator
	{
	private:
		Node *_Node;

	public:
		operator Node *()
		{
			//node *와 묵시적 형변환 연산자 중복 정의
			return _Node;
		}

		iterator(Node *node = nullptr) {
			//인자로 들어온 Node 포인터를 저장
			this->_Node = node;
		}

		Node* getNode() {

			return _Node;
		}

		iterator operator ++() {
			//현재 노드를 다음 노드로 이동
			_Node = _Node->_Next;
			return (*this);
		}

		iterator operator ++(int) {

			iterator returnNode = *this;
			_Node = _Node->_Next;
			return returnNode;
		}

		iterator operator --() {
			//현재 노드를 이전 노드로 이동
			_Node = _Node->_Prev;
			return *this;
		}

		iterator operator --(int) {
			//현재 노드를 이전 노드로 이동
			iterator returnNode = *this;
			_Node = _Node->_Prev;
			return returnNode;
		}

		T& operator *() {
			//현재 노드의 데이터를 뽑음
			return _Node->_Data;
		}
		
		bool operator == (const iterator &iter) {
			//비교연산
			return _Node == iter._Node;
		}

		bool operator != (const iterator &iter) {
			//비교연산
			return _Node != iter._Node;
		}

		bool operator > (const iterator &iter) {
			//비교연산
			return _Node > iter._Node;
		}

		bool operator >= (const iterator &iter) {
			//비교연산
			return _Node >= iter._Node;
		}

		bool operator < (const iterator &iter) {
			//비교연산
			return _Node < iter._Node;
		}

		bool operator <= (const iterator &iter) {
			//비교연산
			return _Node <= iter._Node;
		}

	};

public:

	list() {

		_head = new Node();
		_tail = new Node();
		_head->_Next = _tail;
		_tail->_Prev = _head;

		_size = 0;
	}

	~list() {

		clear();
		delete _head;
		delete _tail;
	}

	int size() {

		return _size;
	}

	bool empty() {

		return _size == 0;
	}

	T front() {

		return *(begin());
	}

	T back() {

		return *(--end());
	}

	iterator begin() {
		//첫번째 노드를 가리키는 이터레이터 리턴
		return _head->_Next;
	}

	iterator end() {
		//Tail 노드를 가리키는(데이터가 없는 진짜 끝 노드) 이터레이터를 리턴
		//	또는 끝으로 인지할 수 있는 이터레이터를 리턴
		
		return _tail;
	}

	iterator insert(iterator iterNow, T data) {

		//begin
		Node *tmp = iterNow;
		Node *newNode = new Node(data);

		tmp->_Prev->_Next = newNode;
		tmp->_Prev = newNode;

		_size++;
		return newNode;
	}

	iterator erase(iterator iter) {

		//- 이터레이터의 그 노드를 지움.
		//- 그리고 지운 노드의 다음 노드를 카리키는 이터레이터 리턴
		Node *tmp = iter;
		iterator returnNode = tmp->_Next;
		
		tmp->_Prev->_Next = tmp->_Next;
		tmp->_Next->_Prev = tmp->_Prev;
		
		delete tmp;
		_size--;
		return returnNode;
	}

	void clear() {

		for (iterator iter = begin(); iter != end();) {

			iter = erase(iter);
		}
	}

	void push_front(T data) {

		Node *node = nullptr;
		//*
		Node *newNode = new Node;

		newNode->_Data = data;
		newNode->_Prev = _head;
		newNode->_Next = _head->_Next;

		node = _head;
		node = node->_Next;
		node->_Prev = newNode;

		node = _head;
		node->_Next = newNode;

		_size++;
		//*/
	}

	void pop_front() {

		Node *node = _head->_Next;
		Node *tmp = node->_Next;

		_head->_Next = node->_Next;
		tmp->_Prev = node->_Prev;

		delete node;
		_size--;
	}

	void push_back(T data) {

		Node *node = nullptr;
		Node *newNode = new Node;

		newNode->_Data = data;
		newNode->_Prev = _tail->_Prev;
		newNode->_Next = _tail;

		node = _tail;
		node = node->_Prev;
		node->_Next = newNode;

		node = _tail;
		node->_Prev = newNode;

		_size++;
	}

	void pop_back() {

		Node *node = _tail->_Prev;
		Node *tmp = node->_Prev;

		_tail->_Prev = node->_Prev;
		tmp->_Next = node->_Next;

		delete node;
		_size--;
	}

	// 간단하게 선택정렬로 구현
	void sort() {

		iterator PivotStart = begin();
		iterator PivotEnd = end();
		


		for(iter=begin(); iter != end())
	}
};

#endif
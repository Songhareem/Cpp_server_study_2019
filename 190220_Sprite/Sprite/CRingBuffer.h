#pragma once

class CRingBuffer {

public:
	CRingBuffer(void);
	CRingBuffer(int iBufferSize);
	~CRingBuffer(void);

	void Initial(int iBufferSize);

	void Resize(int size);

	int	GetBufferSize(void);

	/////////////////////////////////////////////////////////////////////////
	// ���� ������� �뷮 ���.
	//
	// Parameters: ����.
	// Return: (int)������� �뷮.
	/////////////////////////////////////////////////////////////////////////
	int	GetUseSize(void);
	
	/////////////////////////////////////////////////////////////////////////
	// ���� ���ۿ� ���� �뷮 ���. 
	//
	// Parameters: ����.
	// Return: (int)�����뷮.
	/////////////////////////////////////////////////////////////////////////
	int	GetFreeSize(void);
	/////////////////////////////////////////////////////////////////////////
	// ���� �����ͷ� �ܺο��� �ѹ濡 �а�, �� �� �ִ� ����.
	// (������ ���� ����)
	//
	// ���� ť�� ������ ������ ���ܿ� �ִ� �����ʹ� �� -> ó������ ���ư���
	// 2���� �����͸� ��ų� ���� �� ����. �� �κп��� �������� ���� ���̸� �ǹ�
	//
	// Parameters: ����.
	// Return: (int)��밡�� �뷮.
	////////////////////////////////////////////////////////////////////////
	int	GetNotBrokenGetSize(void);
	int	GetNotBrokenPutSize(void);

	/////////////////////////////////////////////////////////////////////////
	// WritePos �� ����Ÿ ����.
	//
	// Parameters: (char *)����Ÿ ������. (int)ũ��. 
	// Return: (int)���� ũ��.
	/////////////////////////////////////////////////////////////////////////
	bool Enqueue(char *chpData, int iSize);

	/////////////////////////////////////////////////////////////////////////
	// ReadPos ���� ����Ÿ ������. ReadPos �̵�.
	//
	// Parameters: (char *)����Ÿ ������. (int)ũ��.
	// Return: (int)������ ũ��.
	/////////////////////////////////////////////////////////////////////////
	bool Dequeue(char *chpDest, int iSize);

	/////////////////////////////////////////////////////////////////////////
	// ReadPos ���� ����Ÿ �о��. ReadPos ����.
	//
	// Parameters: (char *)����Ÿ ������. (int)ũ��.
	// Return: (int)������ ũ��.
	/////////////////////////////////////////////////////////////////////////
	bool Peek(char *chpDest, int iSize);
		 
	/////////////////////////////////////////////////////////////////////////
	// ���ϴ� ���̸�ŭ �б���ġ ���� ���� / ���� ��ġ �̵�
	//
	// Parameters: ����.
	// Return: ����.
	/////////////////////////////////////////////////////////////////////////
	void MoveRear(int iSize);
	void MoveFront(int iSize);

	/////////////////////////////////////////////////////////////////////////
	// ������ ��� ����Ÿ ����.
	//
	// Parameters: ����.
	// Return: ����.
	/////////////////////////////////////////////////////////////////////////
	void ClearBuffer(void);

	/////////////////////////////////////////////////////////////////////////
	// ������ Front ������ ����.
	//
	// Parameters: ����.
	// Return: (char *) ���� ������.
	/////////////////////////////////////////////////////////////////////////
	char *GetFrontBufferPtr(void);
	 
	/////////////////////////////////////////////////////////////////////////
	// ������ RearPos ������ ����.
	//
	// Parameters: ����.
	// Return: (char *) ���� ������.
	/////////////////////////////////////////////////////////////////////////
	char *GetRearBufferPtr(void);

private:
	char *_pBuffer;

	int _BufferSize;
	int _Front;
	int _Rear;
};

// �׽�Ʈ�� ��Ȱȭ �� �ʿ�( ���̺귯��, ����, ��Ʈ��ũ ��Ʈ �� ���� �ʿ��ϸ� )
// RingBuff �׽�Ʈ �ڵ�?
// random �� ��ť, ��ť�ؼ� ���
// Q(200);
// char Data[] ="asdasfasf12312312 123141241 43454 asdfas q"
// char OutData[300];
// char PeekData[300];
// int iInDataPos;
// srand( ������ �������� �༭ ������Ȳ ���� )
// memcmp( OutData�� PeekData ) ��
// 12�ð� ���� RingBuff �׽�Ʈ �ʿ�
// CPacket Packet;

// Packet << ID;
// Packet << x;
// Packet << y;
// Packet << HP;
// Packet << etc;

// send(...Packet...)
// recv(...Packet...)

// Packet >> ID;
// Packet >> x;
// Packet >> y;
// Packet >> HP;
// Packet >> etc;

// �̸� ���� ��Ʈ���� ����� ����, �����۸� �����

// CRingBuffer Buffer;

// Buffer.Enqueue(p, 10);
// Buffer.Dequeue(p, 10);

// ���
// retval = recv(socket,buff, 1024, 0)
// RecvQ.Enqueue(buff, retval)

// ��� ����
// _pBuffer
// int _Front ( byte �뷮 ����)
// int _Rear  ( byte �뷮 ����)

// Enqueue
// bool size ���ڶ�� return false? ������ŭ return retval? ���
// memcpy ��� (�� ���� ���� �� ������ �� ����, ��� �ɫ����� �� �� memcpy)

// freeSize ���� front ���� �� ���� ���� �����ؼ� ����

// front, Rear�� �Լ� �� �Ʒ����� �ѹ��� ���� ( ��Ƽ������ ȯ�濡�� ���װ� ���� �ʰ� �ϱ� ���� )

// Peek �����? ( ���� ������ ����� �ƴ��� Ȯ���ϱ� ���� )

// Move �Լ�? Peek�� ����� �� �����Ͷ��? �ٽ� ���� �ʿ� ������ Move

// clearBuffer ��, Front = Rear�� �ٲ��ָ� �� ( �ʱ�ȭ ��Ű�� �ʴ� ����, ���� ã�� ���� )

// *GetFrontBufferPtr, *GetRearBufferPrt �� ���� ������ ���� 
// ( �߰����� ���� ���� send, recv ���ؼ�, �̷��� ������� ���� Move �Լ� ���� ��� )

// GetNotBrokenGetSize (Rear�κ��� �������� size�� ���ϴ� �Լ� �ʿ�) GetBounderySize()?
// GetNotBrokenPutSize

// Session {
// socket
// RecvQ
// SendQ
// }

// ���ź� ó��
//for (;;) {
//
//  select �� 
// 
//	char buff[1024]
//
//	re = recv(socket, buff, 1024, 0); // ���� Ŭ�� ��� ������ �����Ͱ� ���ٸ� timeout ó��
//	size = recvQ.enqueue(buff, re);   // buff ���� recv�� �ϸ�! ��ü������ �����ϴ� ����, ��趧���� �� �� recv�� �ؾ��ϹǷ� �ϴ��� �̷���
//									  // ���Ŀ��� buff ���� ������ ����
//
//	if (re != size) break;	// recv ���� �����Ͱ� ���� ť ������ ���̿� �ٸ��ٸ� break
//
//	if (recvQ.GetUseSize < sizeof(stHeader)) break; // ť���� ������� ���� �����Ͱ� �ִٸ� break
// 
//	re = recvQ.peek(&Header, sizeof(Header));
//	if (re != sizeof(Header)) LogAndCrash(); // ������ Ȯ���ߴµ��� �ұ��ϰ� ���� ������� ���� �ȵǴ� ��Ȳ(�޸� ħ�� etc)
//
//	if (Header.Len(payload) + sizeof(Header) > recvQ.size()) break; // ���̷ε� + ���ũ�� ��ŭ �����Ͱ�  ���ú� ť���� ũ�ٸ�, ������ �����Ƿ� break;
//
//	recvQ.MoveFront(sizeof(Header));
//	recv.Dequeue(PacketBuffer, Header.Len);
//
//	��Ŷó��
//  
//  sendQ �� ���� ��Ŷ ���
//  select �� writeSet�� ��ϵ� �� send
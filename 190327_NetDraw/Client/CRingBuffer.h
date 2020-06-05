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
	// 현재 사용중인 용량 얻기.
	//
	// Parameters: 없음.
	// Return: (int)사용중인 용량.
	/////////////////////////////////////////////////////////////////////////
	int	GetUseSize(void);
	
	/////////////////////////////////////////////////////////////////////////
	// 현재 버퍼에 남은 용량 얻기. 
	//
	// Parameters: 없음.
	// Return: (int)남은용량.
	/////////////////////////////////////////////////////////////////////////
	int	GetFreeSize(void);
	/////////////////////////////////////////////////////////////////////////
	// 버퍼 포인터로 외부에서 한방에 읽고, 쓸 수 있는 길이.
	// (끊기지 않은 길이)
	//
	// 원형 큐의 구조상 버퍼의 끝단에 있는 데이터는 끝 -> 처음으로 돌아가서
	// 2번에 데이터를 얻거나 넣을 수 있음. 이 부분에서 끊어지지 않은 길이를 의미
	//
	// Parameters: 없음.
	// Return: (int)사용가능 용량.
	////////////////////////////////////////////////////////////////////////
	int	GetNotBrokenGetSize(void);
	int	GetNotBrokenPutSize(void);

	/////////////////////////////////////////////////////////////////////////
	// WritePos 에 데이타 넣음.
	//
	// Parameters: (char *)데이타 포인터. (int)크기. 
	// Return: (int)넣은 크기.
	/////////////////////////////////////////////////////////////////////////
	bool Enqueue(char *chpData, int iSize);

	/////////////////////////////////////////////////////////////////////////
	// ReadPos 에서 데이타 가져옴. ReadPos 이동.
	//
	// Parameters: (char *)데이타 포인터. (int)크기.
	// Return: (int)가져온 크기.
	/////////////////////////////////////////////////////////////////////////
	bool Dequeue(char *chpDest, int iSize);

	/////////////////////////////////////////////////////////////////////////
	// ReadPos 에서 데이타 읽어옴. ReadPos 고정.
	//
	// Parameters: (char *)데이타 포인터. (int)크기.
	// Return: (int)가져온 크기.
	/////////////////////////////////////////////////////////////////////////
	bool Peek(char *chpDest, int iSize);
		 
	/////////////////////////////////////////////////////////////////////////
	// 원하는 길이만큼 읽기위치 에서 삭제 / 쓰기 위치 이동
	//
	// Parameters: 없음.
	// Return: 없음.
	/////////////////////////////////////////////////////////////////////////
	void MoveRear(int iSize);
	void MoveFront(int iSize);

	/////////////////////////////////////////////////////////////////////////
	// 버퍼의 모든 데이타 삭제.
	//
	// Parameters: 없음.
	// Return: 없음.
	/////////////////////////////////////////////////////////////////////////
	void ClearBuffer(void);

	/////////////////////////////////////////////////////////////////////////
	// 버퍼의 Front 포인터 얻음.
	//
	// Parameters: 없음.
	// Return: (char *) 버퍼 포인터.
	/////////////////////////////////////////////////////////////////////////
	char *GetFrontBufferPtr(void);
	 
	/////////////////////////////////////////////////////////////////////////
	// 버퍼의 RearPos 포인터 얻음.
	//
	// Parameters: 없음.
	// Return: (char *) 버퍼 포인터.
	/////////////////////////////////////////////////////////////////////////
	char *GetRearBufferPtr(void);

private:
	char *_pBuffer;

	int _BufferSize;
	int _Front;
	int _Rear;
};

// 테스트의 생활화 꼭 필요( 라이브러리, 엔진, 네트워크 파트 등 검증 필요하면 )
// RingBuff 테스트 코드?
// random 값 인큐, 디큐해서 출력
// Q(200);
// char Data[] ="asdasfasf12312312 123141241 43454 asdfas q"
// char OutData[300];
// char PeekData[300];
// int iInDataPos;
// srand( 랜덤값 고정으로 줘서 문제상황 재현 )
// memcmp( OutData와 PeekData ) 비교
// 12시간 정도 RingBuff 테스트 필요
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

// 이를 위해 스트리밍 방식의 버퍼, 링버퍼를 만들기

// CRingBuffer Buffer;

// Buffer.Enqueue(p, 10);
// Buffer.Dequeue(p, 10);

// 사용
// retval = recv(socket,buff, 1024, 0)
// RecvQ.Enqueue(buff, retval)

// 멤버 변수
// _pBuffer
// int _Front ( byte 용량 가짐)
// int _Rear  ( byte 용량 가짐)

// Enqueue
// bool size 모자라면 return false? 넣은만큼 return retval? 고민
// memcpy 사용 (한 번에 넣을 수 있으면 한 번에, 경계 걸쳣으면 두 번 memcpy)

// freeSize 에는 front 뒤의 빈 공간 까지 포함해서 리턴

// front, Rear은 함수 맨 아래에서 한번만 변경 ( 멀티스레드 환경에서 버그가 나지 않게 하기 위해 )

// Peek 사용은? ( 들어온 데이터 제대로 됐는지 확인하기 위해 )

// Move 함수? Peek후 제대로 된 데이터라면? 다시 뽑을 필요 없으니 Move

// clearBuffer 시, Front = Rear로 바꿔주면 됨 ( 초기화 시키지 않는 이유, 버그 찾기 위해 )

// *GetFrontBufferPtr, *GetRearBufferPrt 은 버퍼 포인터 리턴 
// ( 중간버퍼 없이 직접 send, recv 위해서, 이러한 방식으로 사용시 Move 함수 같이 사용 )

// GetNotBrokenGetSize (Rear로부터 경계까지의 size를 구하는 함수 필요) GetBounderySize()?
// GetNotBrokenPutSize

// Session {
// socket
// RecvQ
// SendQ
// }

// 수신부 처리
//for (;;) {
//
//  select 후 
// 
//	char buff[1024]
//
//	re = recv(socket, buff, 1024, 0); // 만약 클라가 계속 보내는 데이터가 없다면 timeout 처리
//	size = recvQ.enqueue(buff, re);   // buff 없이 recv를 하면! 객체지향을 위반하는 행위, 경계때문에 두 번 recv를 해야하므로 일단은 이렇게
//									  // 차후에는 buff 없이 진행할 것임
//
//	if (re != size) break;	// recv 받은 데이터가 넣은 큐 데이터 길이와 다르다면 break
//
//	if (recvQ.GetUseSize < sizeof(stHeader)) break; // 큐내에 헤더보다 작은 데이터가 있다면 break
// 
//	re = recvQ.peek(&Header, sizeof(Header));
//	if (re != sizeof(Header)) LogAndCrash(); // 위에서 확인했는데도 불구하고 같은 문제라면 말도 안되는 상황(메모리 침범 etc)
//
//	if (Header.Len(payload) + sizeof(Header) > recvQ.size()) break; // 페이로드 + 헤더크기 만큼 데이터가  리시브 큐보다 크다면, 데이터 없으므로 break;
//
//	recvQ.MoveFront(sizeof(Header));
//	recv.Dequeue(PacketBuffer, Header.Len);
//
//	패킷처리
//  
//  sendQ 에 보낼 패킷 등록
//  select 후 writeSet에 등록된 것 send
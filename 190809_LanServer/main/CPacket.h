//
// 1) Packet Header 공간 만들기
//		Packet Header 공간은 MAX = 5Byte
//		용도에 따라 내부망은 2Byte 외부망은 5Byte 사용
//		헤더 세팅 함수 따로 만들기( Set망세팅, SetHeaderData ) / private;
//		
// 2) Packet 브로드캐스트시?
//		Packet의 갯수를 세놓고 나중에 없애야함
//
// 의문점?
// 1) OnRecv 내부에서 새로운 패킷(Send용 Pack) 을 만들어서 써야하는 이유?
//		-> Recv통지단에서 계속 pPack 값을 바꿔주므로, OnRecv에서 pPack을 재사용 하면 안된다
// 2) Packet Ref Count를 쓰는 이유 + new/delete를 막는 이유?
//		-> Pack의 소멸(파괴)은 Send완료 통지단에서 이루어지는데,
//			멀티스레드 상황에서, BroadCast시, 모든 세션에게 패킷을 보낸 후, 삭제되어야 하므로
// 3) Pack의 멤버로 MemPoolTLS를 사용하는 이유 ?
//		->
// 4) CPacket 생성자가 private인 이유?
//		-> PackMemPoolTLS 에 밖에서 만든 pPacket을 Free시킨다거나 하면 안되므로, 
//			Packet의 new/delete사용을 막을 것임

#pragma once

#include "MemoryPoolTLS.h"

#define dfEXTERNAL_NET	true
#define dfINTERNAL_NET	false

class CPacket
{
private:

	enum en_PACKET
	{
		ePACKET_MAX_SIZE = 200,								// 패킷의 최대 사이즈
		eHEADER_MAX_SIZE = 5,									// 패킷 헤더의 최대 사이즈
		eBUFFER_DEFAULT = ePACKET_MAX_SIZE + eHEADER_MAX_SIZE	// 패킷의 총 버퍼 사이즈.
	};

public:
	//////////////////////////////////////////////////////////////////////////
	// 생성자, 파괴자.
	//
	// Parameters:
	// ---------------------------------------------------------
	// (int) 버퍼 사이즈.
	//	---------------------------------------------------------
	//	(const CAyaPacket &) 패킷의 복사를 위한 패킷 레퍼런스.
	// Return:
	//////////////////////////////////////////////////////////////////////////
	CPacket();
	CPacket(int iBufferSize);
	CPacket(const CPacket &clSrcPacket); // 복사생성자

	virtual	~CPacket();

	//////////////////////////////////////////////////////////////////////////
	// 헤더 옵션 세팅.
	//
	// Parameters: dfEXTERNAL_NET(true) / dfINTERNAL_NET(false)
	// Return: 없음.
	//////////////////////////////////////////////////////////////////////////
	void InitHeaderOption(bool bNetOption = dfINTERNAL_NET);

	//////////////////////////////////////////////////////////////////////////
	// 헤더 넣기.
	//
	// Parameters: char *pBuf
	// Return: 성공시 true, 실패시 false
	//////////////////////////////////////////////////////////////////////////
	bool SetHeader(char *pHeader);

protected:
	//////////////////////////////////////////////////////////////////////////
	// 패킷 초기화.
	//
	// 메모리 할당을 여기서 하므로, 함부로 호출하면 안된다. 
	//
	// Parameters: (int)BufferSize.
	// Return: 없음.
	//////////////////////////////////////////////////////////////////////////
	void	Initial(int iBufferSize = eBUFFER_DEFAULT);
	
	//////////////////////////////////////////////////////////////////////////
	// 패킷  파괴.
	//
	// Parameters: 없음.
	// Return: 없음.
	//////////////////////////////////////////////////////////////////////////
	void	Release(void);

public:
	//////////////////////////////////////////////////////////////////////////
	// 패킷 청소.
	//
	// Parameters: 없음.
	// Return: 없음.
	//////////////////////////////////////////////////////////////////////////
	void	Clear(void);

	//////////////////////////////////////////////////////////////////////////
	// 버퍼 사이즈 얻기 / Read 얻기 / Write 얻기
	//
	// Parameters: 없음.
	// Return: (int)패킷 버퍼 사이즈 얻기.
	//////////////////////////////////////////////////////////////////////////
	int		GetBufferSize(void);
	int		GetReadPos(void);
	int		GetWritePos(void);

	//////////////////////////////////////////////////////////////////////////
	// 패킷 실제 총 사이즈
	//////////////////////////////////////////////////////////////////////////
	int		GetPayloadSize(void);

	//////////////////////////////////////////////////////////////////////////
	// 패킷 실제 총 사이즈
	//////////////////////////////////////////////////////////////////////////
	int		GetPacketSize(void);

	//////////////////////////////////////////////////////////////////////////
	// 버퍼 남은 사이즈
	//////////////////////////////////////////////////////////////////////////
	int		GetFreeSize(void);

	//////////////////////////////////////////////////////////////////////////
	// 버퍼 포인터 얻기.
	//
	// 
	//
	// Parameters: 없음.
	// Return: (char *)버퍼 포인터.
	//////////////////////////////////////////////////////////////////////////
	char	*GetBufferPtr(void);

	//////////////////////////////////////////////////////////////////////////
	// 버퍼 포인터 얻기.
	//
	// 
	//
	// Parameters: 없음.
	// Return: (char *)버퍼 포인터.
	//////////////////////////////////////////////////////////////////////////
	char	*GetHeadPtr(void);

	//////////////////////////////////////////////////////////////////////////
	// Read 포인터 얻기.
	//
	// 
	//
	// Parameters: 없음.
	// Return: (char *)버퍼 포인터.
	//////////////////////////////////////////////////////////////////////////
	char	*GetReadPtr(void);

	//////////////////////////////////////////////////////////////////////////
	// Write 포인터 얻기.
	//
	// 
	//
	// Parameters: 없음.
	// Return: (char *)버퍼 포인터.
	//////////////////////////////////////////////////////////////////////////
	char	*GetWritePtr(void);

	//////////////////////////////////////////////////////////////////////////
	// 버퍼 Pos 이동. (음수이동은 안됨)
	// GetBufferPtr 함수를 이용하여 외부에서 강제로 버퍼 내용을 수정할 경우 사용. 
	//
	// Parameters: (int) 이동 사이즈.
	// Return: (int) 이동된 사이즈.
	//////////////////////////////////////////////////////////////////////////
	int		MoveWritePos(int iSize);
	int		MoveReadPos(int iSize);

	/*================================================= */
	// 연산자 오퍼레이터.
	/*================================================= */
	// 대입 연산자 =
	CPacket &operator = (const CPacket &Pack);

	//////////////////////////////////////////////////////////////////////////
	// 넣기.	각 변수 타입마다 모두 제작
	//////////////////////////////////////////////////////////////////////////
	//<<
	CPacket &operator << (bool	blVal);
	CPacket &operator << (char	chVal);
	CPacket &operator << (short	shVal);
	CPacket &operator << (int	iVal);
	CPacket &operator << (long	lgVal);
	CPacket &operator << (float	fVal);
	CPacket &operator << (double dVal);
	//CPacket &operator << (__int8 i8Val);
	//CPacket &operator << (__int16 i16Val);
	//CPacket &operator << (__int32 i32Val);
	CPacket &operator << (__int64 i64Val);
	CPacket &operator << (unsigned char		uchVal);
	CPacket &operator << (unsigned short	ushVal);
	CPacket &operator << (unsigned int		uiVal);
	CPacket &operator << (unsigned long		ulgVal);

	//////////////////////////////////////////////////////////////////////////
	// 빼기.	각 변수 타입마다 모두 제작
	//////////////////////////////////////////////////////////////////////////
	//>>
	CPacket &operator >> (bool	&blVal);
	CPacket &operator >> (char	&chVal);
	CPacket &operator >> (short	&shVal);
	CPacket &operator >> (int	&iVal);
	CPacket &operator >> (long	&lgVal);
	CPacket &operator >> (float	&fVal);
	CPacket &operator >> (double &dVal);
	//CPacket &operator >> (__int8 &i8Val);
	//CPacket &operator >> (__int16 &i16Val);
	//CPacket &operator >> (__int32 &i32Val);
	CPacket &operator >> (__int64 &i64Val);
	CPacket &operator >> (unsigned char		&uchVal);
	CPacket &operator >> (unsigned short	&ushVal);
	CPacket &operator >> (unsigned int		&uiVal);
	CPacket &operator >> (unsigned long		&ulgVal);

	//////////////////////////////////////////////////////////////////////////
	// 데이타 얻기.
	//
	// Parameters: (char *)Dest 포인터. (int)Size.
	// Return: (int)복사한 사이즈.
	//////////////////////////////////////////////////////////////////////////
	int		Enqueue(char *chpDest, int iSize);

	//////////////////////////////////////////////////////////////////////////
	// 데이타 삽입.
	//
	// Parameters: (char *)Src 포인터. (int)SrcSize.
	// Return: (int)복사한 사이즈.
	//////////////////////////////////////////////////////////////////////////
	int		Dequeue(char *chpSrc, int iSrcSize);

	long	RefIncrement();
	
	long	RefDecrement();

	void	PrintPacket();

	static CPacket *Alloc();
	static bool		Free(CPacket *pPack);

private:
	char *_pBuffer;
	
	int _iBufSize;
	int _iWrite;
	int _iRead;
	int	_iHead;

	bool _bHeaderOpt;
	long _lRefCnt;

	static CMemoryPoolTLS<CPacket> _packetPoolTls;
	
	//friend class CMemoryPoolTLS<CPacket>;
	//friend class CMemoryPool<CChunk<CPacket>>;
	friend class CChunk<CPacket>;
};

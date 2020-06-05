//
// 1) Packet Header ���� �����
//		Packet Header ������ MAX = 5Byte
//		�뵵�� ���� ���θ��� 2Byte �ܺθ��� 5Byte ���
//		��� ���� �Լ� ���� �����( Set������, SetHeaderData ) / private;
//		
// 2) Packet ��ε�ĳ��Ʈ��?
//		Packet�� ������ ������ ���߿� ���־���
//
// �ǹ���?
// 1) OnRecv ���ο��� ���ο� ��Ŷ(Send�� Pack) �� ���� ����ϴ� ����?
//		-> Recv�����ܿ��� ��� pPack ���� �ٲ��ֹǷ�, OnRecv���� pPack�� ���� �ϸ� �ȵȴ�
// 2) Packet Ref Count�� ���� ���� + new/delete�� ���� ����?
//		-> Pack�� �Ҹ�(�ı�)�� Send�Ϸ� �����ܿ��� �̷�����µ�,
//			��Ƽ������ ��Ȳ����, BroadCast��, ��� ���ǿ��� ��Ŷ�� ���� ��, �����Ǿ�� �ϹǷ�
// 3) Pack�� ����� MemPoolTLS�� ����ϴ� ���� ?
//		->
// 4) CPacket �����ڰ� private�� ����?
//		-> PackMemPoolTLS �� �ۿ��� ���� pPacket�� Free��Ų�ٰų� �ϸ� �ȵǹǷ�, 
//			Packet�� new/delete����� ���� ����

#pragma once

#include "MemoryPoolTLS.h"

#define dfEXTERNAL_NET	true
#define dfINTERNAL_NET	false

class CPacket
{
private:

	enum en_PACKET
	{
		ePACKET_MAX_SIZE = 200,								// ��Ŷ�� �ִ� ������
		eHEADER_MAX_SIZE = 5,									// ��Ŷ ����� �ִ� ������
		eBUFFER_DEFAULT = ePACKET_MAX_SIZE + eHEADER_MAX_SIZE	// ��Ŷ�� �� ���� ������.
	};

public:
	//////////////////////////////////////////////////////////////////////////
	// ������, �ı���.
	//
	// Parameters:
	// ---------------------------------------------------------
	// (int) ���� ������.
	//	---------------------------------------------------------
	//	(const CAyaPacket &) ��Ŷ�� ���縦 ���� ��Ŷ ���۷���.
	// Return:
	//////////////////////////////////////////////////////////////////////////
	CPacket();
	CPacket(int iBufferSize);
	CPacket(const CPacket &clSrcPacket); // ���������

	virtual	~CPacket();

	//////////////////////////////////////////////////////////////////////////
	// ��� �ɼ� ����.
	//
	// Parameters: dfEXTERNAL_NET(true) / dfINTERNAL_NET(false)
	// Return: ����.
	//////////////////////////////////////////////////////////////////////////
	void InitHeaderOption(bool bNetOption = dfINTERNAL_NET);

	//////////////////////////////////////////////////////////////////////////
	// ��� �ֱ�.
	//
	// Parameters: char *pBuf
	// Return: ������ true, ���н� false
	//////////////////////////////////////////////////////////////////////////
	bool SetHeader(char *pHeader);

protected:
	//////////////////////////////////////////////////////////////////////////
	// ��Ŷ �ʱ�ȭ.
	//
	// �޸� �Ҵ��� ���⼭ �ϹǷ�, �Ժη� ȣ���ϸ� �ȵȴ�. 
	//
	// Parameters: (int)BufferSize.
	// Return: ����.
	//////////////////////////////////////////////////////////////////////////
	void	Initial(int iBufferSize = eBUFFER_DEFAULT);
	
	//////////////////////////////////////////////////////////////////////////
	// ��Ŷ  �ı�.
	//
	// Parameters: ����.
	// Return: ����.
	//////////////////////////////////////////////////////////////////////////
	void	Release(void);

public:
	//////////////////////////////////////////////////////////////////////////
	// ��Ŷ û��.
	//
	// Parameters: ����.
	// Return: ����.
	//////////////////////////////////////////////////////////////////////////
	void	Clear(void);

	//////////////////////////////////////////////////////////////////////////
	// ���� ������ ��� / Read ��� / Write ���
	//
	// Parameters: ����.
	// Return: (int)��Ŷ ���� ������ ���.
	//////////////////////////////////////////////////////////////////////////
	int		GetBufferSize(void);
	int		GetReadPos(void);
	int		GetWritePos(void);

	//////////////////////////////////////////////////////////////////////////
	// ��Ŷ ���� �� ������
	//////////////////////////////////////////////////////////////////////////
	int		GetPayloadSize(void);

	//////////////////////////////////////////////////////////////////////////
	// ��Ŷ ���� �� ������
	//////////////////////////////////////////////////////////////////////////
	int		GetPacketSize(void);

	//////////////////////////////////////////////////////////////////////////
	// ���� ���� ������
	//////////////////////////////////////////////////////////////////////////
	int		GetFreeSize(void);

	//////////////////////////////////////////////////////////////////////////
	// ���� ������ ���.
	//
	// 
	//
	// Parameters: ����.
	// Return: (char *)���� ������.
	//////////////////////////////////////////////////////////////////////////
	char	*GetBufferPtr(void);

	//////////////////////////////////////////////////////////////////////////
	// ���� ������ ���.
	//
	// 
	//
	// Parameters: ����.
	// Return: (char *)���� ������.
	//////////////////////////////////////////////////////////////////////////
	char	*GetHeadPtr(void);

	//////////////////////////////////////////////////////////////////////////
	// Read ������ ���.
	//
	// 
	//
	// Parameters: ����.
	// Return: (char *)���� ������.
	//////////////////////////////////////////////////////////////////////////
	char	*GetReadPtr(void);

	//////////////////////////////////////////////////////////////////////////
	// Write ������ ���.
	//
	// 
	//
	// Parameters: ����.
	// Return: (char *)���� ������.
	//////////////////////////////////////////////////////////////////////////
	char	*GetWritePtr(void);

	//////////////////////////////////////////////////////////////////////////
	// ���� Pos �̵�. (�����̵��� �ȵ�)
	// GetBufferPtr �Լ��� �̿��Ͽ� �ܺο��� ������ ���� ������ ������ ��� ���. 
	//
	// Parameters: (int) �̵� ������.
	// Return: (int) �̵��� ������.
	//////////////////////////////////////////////////////////////////////////
	int		MoveWritePos(int iSize);
	int		MoveReadPos(int iSize);

	/*================================================= */
	// ������ ���۷�����.
	/*================================================= */
	// ���� ������ =
	CPacket &operator = (const CPacket &Pack);

	//////////////////////////////////////////////////////////////////////////
	// �ֱ�.	�� ���� Ÿ�Ը��� ��� ����
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
	// ����.	�� ���� Ÿ�Ը��� ��� ����
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
	// ����Ÿ ���.
	//
	// Parameters: (char *)Dest ������. (int)Size.
	// Return: (int)������ ������.
	//////////////////////////////////////////////////////////////////////////
	int		Enqueue(char *chpDest, int iSize);

	//////////////////////////////////////////////////////////////////////////
	// ����Ÿ ����.
	//
	// Parameters: (char *)Src ������. (int)SrcSize.
	// Return: (int)������ ������.
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

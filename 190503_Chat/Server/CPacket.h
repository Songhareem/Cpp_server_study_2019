#pragma once

class CPacket
{
public:

	enum en_PACKET
	{
		eBUFFER_DEFAULT = 1024,		// ��Ŷ�� �⺻ ���� ������.
		ePACKET_MAX_SIZE = 1024,		// ��Ŷ�� �ִ� ������
	};

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

	void	PrintPacket(bool isHex, int iSize);

private:
	char *_Buffer;
	
	int _BufSize;
	int _Write;
	int _Read;
};

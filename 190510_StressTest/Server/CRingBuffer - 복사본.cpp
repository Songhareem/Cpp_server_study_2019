/////////////////////////////////////////////////////////////////////
// www.gamecodi.com						이주행 master@gamecodi.com
//
//
/////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------

	Aya Library - Stream SingleQueue

	원형 FIFO 스트리밍 큐.


	완성된 패킷을 만들기 위해서 StreamDQ 앞단에 사용을 목적으로 한다.


	- 사용법.

	CAyaStreamSQ	clStreamQ;
	CAyaStreamSQ	clStreamQ(1024);




----------------------------------------------------------------*/
#include "pch.h"
#include <windows.h>
#include "CRingBuffer.h"


/////////////////////////////////////////////////////////////////////////
// 생성자, 파괴자.
//
//
/////////////////////////////////////////////////////////////////////////
CRingBuffer::CRingBuffer()
{
	m_chpBuffer = NULL;
	m_iBufferSize = 0;

	m_iReadPos = 0;
	m_iWritePos = 0;


	/*-----------------------------------------------------------------
	버퍼 생성.
	-------------------------------------------------------------------*/
	Initial(eBUFFER_DEFAULT);

}
CRingBuffer::CRingBuffer(int iBufferSize)
{

	m_chpBuffer = NULL;
	m_iBufferSize = 0;

	m_iReadPos = 0;
	m_iWritePos = 0;

	/*-----------------------------------------------------------------
	버퍼 생성.
	-------------------------------------------------------------------*/
	Initial(iBufferSize);
}
CRingBuffer::~CRingBuffer()
{

	if ( NULL != m_chpBuffer )
		delete[] m_chpBuffer;

	m_chpBuffer = NULL;
	m_iBufferSize = 0;

	m_iReadPos = 0;
	m_iWritePos = 0;

}



/////////////////////////////////////////////////////////////////////////
// 버퍼 생성 & 큐의 초기화.
//
// Parameters: (int)버퍼용량.
// Return: 없음.
/////////////////////////////////////////////////////////////////////////
void CRingBuffer::Initial(int iBufferSize)
{
	if ( NULL != m_chpBuffer )
		delete[] m_chpBuffer;

	if ( 0 >= iBufferSize ) return;

	m_iBufferSize = iBufferSize;

	m_iReadPos = 0;
	m_iWritePos = 0;

	m_chpBuffer = new char[iBufferSize];

}


/////////////////////////////////////////////////////////////////////////
// 버퍼 전체의 용량 얻음.
//
// Parameters: 없음.
// Return: (int)버퍼용량.
/////////////////////////////////////////////////////////////////////////
int	CRingBuffer::GetBufferSize(void)
{
	if ( NULL != m_chpBuffer )
	{
		return m_iBufferSize - eBUFFER_BLANK;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////
// 현재 사용중인 용량 얻기.
//
// Parameters: 없음.
// Return: (int)사용중인 용량.
/////////////////////////////////////////////////////////////////////////
int CRingBuffer::GetUseSize(void)
{
	/*-----------------------------------------------------------------
	쓸 위치가 더 뒤에 있거나, 데이타가 없다.
	-------------------------------------------------------------------*/
	if ( m_iReadPos <= m_iWritePos )
	{
		return m_iWritePos - m_iReadPos;
	}
	else
	{
		return m_iBufferSize - m_iReadPos + m_iWritePos;
	}

}


/////////////////////////////////////////////////////////////////////////
// 현재 버퍼에 남은 용량 얻기.
//
// Parameters: 없음.
// Return: (int)남은용량.
/////////////////////////////////////////////////////////////////////////
int CRingBuffer::GetFreeSize(void)
{
	return m_iBufferSize - (GetUseSize() + eBUFFER_BLANK);
}



/////////////////////////////////////////////////////////////////////////
// 버퍼 포인터로 외부에서 한방에 읽고, 쓸 수 있는 길이.
// (끊기지 않은 길이)
//
// Parameters: 없음.
// Return: (int)사용가능 용량.
/////////////////////////////////////////////////////////////////////////
int CRingBuffer::GetNotBrokenGetSize(void)
{
	if ( m_iReadPos <= m_iWritePos )
	{
		return m_iWritePos - m_iReadPos;
	}
	else
	{
		return m_iBufferSize - m_iReadPos ;
	}
}

int CRingBuffer::GetNotBrokenPutSize(void)
{
	if ( m_iWritePos <= m_iReadPos )
	{
		return (m_iReadPos - m_iWritePos) - eBUFFER_BLANK;
	}
	else
	{
		if ( m_iReadPos < eBUFFER_BLANK )
		{
			return (m_iBufferSize - m_iWritePos) - (eBUFFER_BLANK - m_iReadPos);
		}
		else
		{
			return m_iBufferSize - m_iWritePos;
		}
	}
}


/////////////////////////////////////////////////////////////////////////
// WritePos 에 데이타 넣음.
//
// Parameters: (char *)데이타 포인터. (int)크기.
// Return: (int)넣은 크기.
/////////////////////////////////////////////////////////////////////////
int CRingBuffer::Enqueue(char *chpData, int iSize)
{
	int iWrite;

	/*-----------------------------------------------------------------
	남은 용량이 적을 경우 ...  남은만큼만 넣자.
	-------------------------------------------------------------------*/
	if ( GetFreeSize() < iSize )
	{
		return 0;
		iSize = GetFreeSize();
	}

	if ( 0 >= iSize ) 
		return 0;

	if ( m_iReadPos <= m_iWritePos )
	{
		/*-----------------------------------------------------------------
		쓰기가 뒤에 있다.  원형으로 돌아가므로 짤라서 복사해야할 경우가 생긴다.
		-------------------------------------------------------------------*/
		iWrite = m_iBufferSize - m_iWritePos;

		if ( iWrite >= iSize )
		{
			/*-----------------------------------------------------------------
			한번에 복사 가능.
			-------------------------------------------------------------------*/
			memcpy(m_chpBuffer + m_iWritePos, chpData, iSize);
			m_iWritePos += iSize;
		}
		else
		{
			/*-----------------------------------------------------------------
			뒤가 짤리므로 두번에 복사.
			-------------------------------------------------------------------*/
			memcpy(m_chpBuffer + m_iWritePos, chpData, iWrite);
			memcpy(m_chpBuffer, chpData + iWrite, iSize - iWrite);
			m_iWritePos = iSize - iWrite;
		}
	}
	else
	{
		/*-----------------------------------------------------------------
		쓰기가 앞에. 한번에 복사 가능. 
		-------------------------------------------------------------------*/
		memcpy(m_chpBuffer + m_iWritePos, chpData, iSize);
		m_iWritePos += iSize;
	}

	m_iWritePos = m_iWritePos == m_iBufferSize ? 0 : m_iWritePos;

	return iSize;
}


/////////////////////////////////////////////////////////////////////////
// ReadPos 에서 데이타 가져옴. ReadPos 이동.
//
// Parameters: (char *)데이타 포인터. (int)크기.
// Return: (int)가져온 크기.
/////////////////////////////////////////////////////////////////////////
int CRingBuffer::Dequeue(char *chpDest, int iSize)
{
	int iRead;

	/*-----------------------------------------------------------------
	저장된 데이타가 더 적다면.. 있는 만큼만.
	-------------------------------------------------------------------*/
	if ( GetUseSize() < iSize )
		iSize = GetUseSize();

	if ( 0 >= iSize )
		return 0;

	if ( m_iReadPos <= m_iWritePos )
	{
		/*-----------------------------------------------------------------
		쓰기가 뒤에 있다.  한방에 가능.
		-------------------------------------------------------------------*/
		memcpy(chpDest, m_chpBuffer + m_iReadPos, iSize);
		m_iReadPos += iSize;
	}
	else
	{
		/*-----------------------------------------------------------------
		쓰기가 앞에 있으므로 짤라서 읽어야할 경우가 있다.
		-------------------------------------------------------------------*/
		iRead = m_iBufferSize - m_iReadPos;

		/*-----------------------------------------------------------------
		얻고자 하는 길이가 뒤에남은 걸로 한번에 가능한지 검사.
		-------------------------------------------------------------------*/
		if ( iRead >= iSize )
		{
			/*-----------------------------------------------------------------
			한번에 복사 가능.
			-------------------------------------------------------------------*/
			memcpy(chpDest, m_chpBuffer + m_iReadPos, iSize);
			m_iReadPos += iSize;
		}
		else
		{
			/*-----------------------------------------------------------------
			뒤가 짤리므로 두번에 복사.
			-------------------------------------------------------------------*/
			memcpy(chpDest, m_chpBuffer + m_iReadPos, iRead);
			memcpy(chpDest + iRead, m_chpBuffer, iSize - iRead);
			m_iReadPos = iSize - iRead;
		}
	}

	return iSize;
}


/////////////////////////////////////////////////////////////////////////
// ReadPos 에서 데이타 읽어옴. ReadPos 고정.
//
// Parameters: (char *)데이타 포인터. (int)크기.
// Return: (int)가져온 크기.
/////////////////////////////////////////////////////////////////////////
int	CRingBuffer::Peek(char *chpDest, int iSize)
{
	int iRead;

	/*-----------------------------------------------------------------
	저장된 데이타가 더 적다면.. 있는 만큼만.
	-------------------------------------------------------------------*/
	if ( GetUseSize() < iSize )
		iSize = GetUseSize();

	if ( 0 >= iSize )
		return 0;
	
	if ( m_iReadPos <= m_iWritePos )
	{
		/*-----------------------------------------------------------------
		쓰기가 뒤에 있다.  한방에 가능.
		-------------------------------------------------------------------*/
		memcpy(chpDest, m_chpBuffer + m_iReadPos, iSize);
	}
	else
	{
		/*-----------------------------------------------------------------
		쓰기가 앞에 있으므로 짤라서 읽어야할 경우가 있다.
		-------------------------------------------------------------------*/
		iRead = m_iBufferSize - m_iReadPos;

		/*-----------------------------------------------------------------
		얻고자 하는 길이가 뒤에남은 걸로 한번에 가능한지 검사.
		-------------------------------------------------------------------*/
		if ( iRead >= iSize )
		{
			/*-----------------------------------------------------------------
			한번에 복사 가능.
			-------------------------------------------------------------------*/
			memcpy(chpDest, m_chpBuffer + m_iReadPos, iSize);
		}
		else
		{
			/*-----------------------------------------------------------------
			뒤가 짤리므로 두번에 복사.
			-------------------------------------------------------------------*/
			memcpy(chpDest, m_chpBuffer + m_iReadPos, iRead);
			memcpy(chpDest + iRead, m_chpBuffer, iSize - iRead);
		}
	}

	return iSize;

}


/////////////////////////////////////////////////////////////////////////
// 원하는 길이만큼 읽기위치 에서 삭제.
//
// Parameters: 없음.
// Return: 없음.
/////////////////////////////////////////////////////////////////////////
void CRingBuffer::MoveFront(int iSize)
{
	int iRead;

	/*-----------------------------------------------------------------
	저장된 데이타가 더 적다면.. 있는 만큼만.
	-------------------------------------------------------------------*/
	if ( GetUseSize() < iSize )
		iSize = GetUseSize();

	if ( 0 >= iSize )
		return;

	if ( m_iReadPos < m_iWritePos )
	{
		/*-----------------------------------------------------------------
		ReadPos 이동.
		-------------------------------------------------------------------*/
		m_iReadPos += iSize;
	}
	else
	{
		/*-----------------------------------------------------------------
		지울 길이가 한번에 가능한지 검사.
		-------------------------------------------------------------------*/
		iRead = m_iBufferSize - m_iReadPos;

		if ( iRead >= iSize )
		{
			/*-----------------------------------------------------------------
			ReadPos 이동.
			-------------------------------------------------------------------*/
			m_iReadPos += iSize;
		}
		else
		{
			/*-----------------------------------------------------------------
			뒤가 짤리므로 앞쪽으로 ReadPos 돌림.
			-------------------------------------------------------------------*/
			m_iReadPos = iSize - iRead;
		}
	}

	m_iReadPos = m_iReadPos == m_iBufferSize ? 0 : m_iReadPos;
}

/////////////////////////////////////////////////////////////////////////
// 원하는 길이만큼 읽기위치 에서 쓰기.
//
// Parameters: 없음.
// Return: 없음.
/////////////////////////////////////////////////////////////////////////
void CRingBuffer::MoveRear(int iSize)
{
	int iRead;

	/*-----------------------------------------------------------------
	빈공간이 더 적다면.. 있는 만큼만.
	-------------------------------------------------------------------*/
	if (GetFreeSize() < iSize)
		iSize = GetFreeSize();

	if (0 >= iSize)
		return;

	if (m_iReadPos < m_iWritePos)
	{
		/*-----------------------------------------------------------------
		ReadPos 이동.
		-------------------------------------------------------------------*/
		m_iReadPos += iSize;
	}
	else
	{
		/*-----------------------------------------------------------------
		지울 길이가 한번에 가능한지 검사.
		-------------------------------------------------------------------*/
		iRead = m_iBufferSize - m_iReadPos;

		if (iRead >= iSize)
		{
			/*-----------------------------------------------------------------
			ReadPos 이동.
			-------------------------------------------------------------------*/
			m_iReadPos += iSize;
		}
		else
		{
			/*-----------------------------------------------------------------
			뒤가 짤리므로 앞쪽으로 ReadPos 돌림.
			-------------------------------------------------------------------*/
			m_iReadPos = iSize - iRead;
		}
	}

	m_iReadPos = m_iReadPos == m_iBufferSize ? 0 : m_iReadPos;
}


/////////////////////////////////////////////////////////////////////////
// 버퍼의 모든 데이타 삭제.
//
// Parameters: 없음.
// Return: 없음.
/////////////////////////////////////////////////////////////////////////
void CRingBuffer::Clear(void)
{
	m_iReadPos = 0;
	m_iWritePos = 0;
}

/////////////////////////////////////////////////////////////////////////
// 버퍼의 포인터 얻음.
//
// Parameters: 없음.
// Return: (char *) 버퍼 포인터.
/////////////////////////////////////////////////////////////////////////
char *CRingBuffer::GetBufferPtr(void)
{
	return m_chpBuffer;
}

/////////////////////////////////////////////////////////////////////////
// 버퍼의 ReadPos 포인터 얻음.
//
// Parameters: 없음.
// Return: (char *) 버퍼 포인터.
/////////////////////////////////////////////////////////////////////////
char *CRingBuffer::GetReadBufferPtr(void)
{
	return m_chpBuffer + m_iReadPos;
}

/////////////////////////////////////////////////////////////////////////
// 버퍼의 WritePos 포인터 얻음.
//
// Parameters: 없음.
// Return: (char *) 버퍼 포인터.
/////////////////////////////////////////////////////////////////////////
char *CRingBuffer::GetWriteBufferPtr(void)
{
	return m_chpBuffer + m_iWritePos;
}
 

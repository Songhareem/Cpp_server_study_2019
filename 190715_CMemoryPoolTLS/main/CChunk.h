#pragma once

#include <Windows.h>

#define dfMAGIC_NUM			((WORD)0x5a5a)
#define dfDEFAULT_CHUNK		200		

template<typename DATA>
class CChunk
{
	typedef struct st_BLOCK
	{
		CChunk	*_pChunk;
		WORD	_FrontBump;
		DATA	_Data;
		WORD	_RearBump;

		void SetBlock(CChunk *pChunk) {

			_FrontBump = _RearBump = dfMAGIC_NUM;
			_pChunk = pChunk;
			// �޸� ������ ���� ����, ���Ŀ� Ȯ���� ���ؼ�? ZeroMemory �ʿ��ұ�?
		}

	} BLOCK;

public:
	//=============================
	// ������
	//=============================
	CChunk() {}

	//=============================
	// �Ҹ���
	//=============================
	~CChunk() {
	
		ChunkFin();
	}

	//=============================
	// Alloc
	//=============================
	DATA*	Alloc() {

		if (_AllocCnt >= _NumOfChunk)
			return NULL;
		
		return &_pBlock[_AllocCnt++].Data;
	}

	//=============================
	// Free
	//=============================
	bool	Free(DATA* pData) {

		CChunk	*pChunk = NULL;
		BYTE	*pByte = NULL;
		WORD	*pFront = NULL;
		WORD	*pRear = NULL;

		if ((_FreeCnt + 1) >= _NumOfChunk) {

			// ���� �� �̻����� Ȯ��
			pFront = pRear = (WORD)pData;

			pFront -= 1; // WORD��ŭ �ּҰ� ���̳ʽ�
			//pRear += 1; // DATA �� ��ŭ �ּҰ� �÷���

			// ���۰� �������� Ȯ��
			if (*pFront != dfMAGIC_NUM || *pRear != dfMAGIC_NUM) {
				
				Crash();
			}

			// ChunkPtr ã��

			pByte = (BYTE)pData;
			pByte -= sizeof(WORD);
			pByte -= sizeof(CChunk *);

			pChunk = (CChunk *)pByte;

			// MemoryPool Free ȣ��
		}
	}

	//=============================
	// Initialize
	// ���� : ûũ ����, �޸�Ǯ���� �Ҵ���� �ּ�
	// ���� : ���� 
	// ������ ���� �������� ��������� ��
	//=============================
	void ChunkInit(int iNumOfChunk, void* pOriginAddr) {

		// ���� �ʱ�ȭ
		_AllocCnt = _FreeCnt = 0;
		_NumOfChunk = iNumOfChunk;
		_pMemoryPoolAddr = pOriginAddr;

		_pBlock = new BLOCK[iNumOfChunk];

		for (int i = 0; i < _NumOfChunk; i++)
			_pBlock[i].SetBlock(this);
	}

	//=============================
	// Finalize
	// ���� : ����
	// ���� : ���� 
	//=============================
	void ChunkFin() {

		// ��� �迭 ����
		delete _pBlock;
	}

	void Crash() {

		int *p = NULL;
		*p = 0;
	}

private:

	int		_AllocCnt;
	int		_FreeCnt;
	int		_NumOfChunk;
	void	*_pMemoryPoolAddr;
	BLOCK	*_pBlock;
};


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
			// 메모리 쓰레기 값인 상태, 차후에 확인을 위해서? ZeroMemory 필요할까?
		}

	} BLOCK;

public:
	//=============================
	// 생성자
	//=============================
	CChunk() {}

	//=============================
	// 소멸자
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

			// 범퍼 값 이상한지 확인
			pFront = pRear = (WORD)pData;

			pFront -= 1; // WORD만큼 주소값 마이너스
			//pRear += 1; // DATA 값 만큼 주소값 플러스

			// 범퍼값 깨졌는지 확인
			if (*pFront != dfMAGIC_NUM || *pRear != dfMAGIC_NUM) {
				
				Crash();
			}

			// ChunkPtr 찾기

			pByte = (BYTE)pData;
			pByte -= sizeof(WORD);
			pByte -= sizeof(CChunk *);

			pChunk = (CChunk *)pByte;

			// MemoryPool Free 호출
		}
	}

	//=============================
	// Initialize
	// 인자 : 청크 갯수, 메모리풀에서 할당받은 주소
	// 리턴 : 없음 
	// 생성자 이후 수동으로 선언해줘야 함
	//=============================
	void ChunkInit(int iNumOfChunk, void* pOriginAddr) {

		// 변수 초기화
		_AllocCnt = _FreeCnt = 0;
		_NumOfChunk = iNumOfChunk;
		_pMemoryPoolAddr = pOriginAddr;

		_pBlock = new BLOCK[iNumOfChunk];

		for (int i = 0; i < _NumOfChunk; i++)
			_pBlock[i].SetBlock(this);
	}

	//=============================
	// Finalize
	// 인자 : 없음
	// 리턴 : 없음 
	//=============================
	void ChunkFin() {

		// 블록 배열 생성
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


#pragma once

#ifndef __SERVER__
#define __SERVER__

#include <WS2tcpip.h>
#include <Windows.h>
#include <conio.h>
#include <map>
#include <list>
#include "CRingBuffer.h"
#include "PacketDefine.h"
#include "CLogMaker.h"
#include "CPacket.h"

#pragma comment(lib,"ws2_32")
#pragma comment(lib,"winmm")

using namespace std;

//============================
// 구조체 선언
//============================

// 섹터 단일 좌표 구조체
typedef struct st_SECTOR_POS {

	int iX = -1;
	int iY = -1;
}SECTOR_POS;

// 특정 위치 주변 9개의 섹터 정보 구조체
typedef struct st_SECTOR_AROUND {

	int				iCount;	// 사용하는 섹터 갯수
	st_SECTOR_POS	Around[9];
}SECTOR_AROUND;

// 세션정보 구조체
typedef struct st_SESSION {

	SOCKET		Socket;
	DWORD		dwSessionID;
	CRingBuffer	SendQ;
	CRingBuffer	RecvQ;
	DWORD		dwTrafficTick;
	DWORD		dwTrafficCount;
	DWORD		dwTrafficPerSecTick;

}SESSION;

// 캐릭터 정보 구조체
typedef struct st_CHARACTER {

	st_SESSION		*pSession;
	DWORD			dwSessionID;

	DWORD			dwAction;
	DWORD			dwActionTick;
	BYTE			byDirection;
	BYTE			byMoveDirection;

	short			shX;
	short			shY;
	short			shActionX;
	short			shActionY;

	st_SECTOR_POS	CurSector;
	st_SECTOR_POS	OldSector;

	char			chHP;

}CHARACTER;

//============================
// 전역변수 선언
//============================

//----------------------------------------
// 컨텐츠부 변수
//----------------------------------------

// 캐릭터 map
map<DWORD, CHARACTER *>g_CharacterMap;

// 월드맵 캐릭터 섹터 list
list<st_CHARACTER *> g_Sector[dfSECTOR_MAX_Y][dfSECTOR_MAX_X];

//----------------------------------------
// 제어부 변수
//----------------------------------------

// 이 값에 따라 서버 메인 루프 종료 결정
bool g_bShutdown = false;

// 이 값에 따라 키보드 잠금 및 잠금 해제
bool g_bControlMode = false;

// FPS 표기
INT64 g_TotalFps = 0;
DWORD g_MaxFps = 0;
DWORD g_MinFps = 0xffffffff;

//----------------------------------------
// 네트워크부 변수
//----------------------------------------

// 리슨 소켓
SOCKET g_ListenSock;

// 서버 Addr
SOCKADDR_IN g_ServerAddr;

// WSA
WSADATA g_wsa;

// 세션 고유 ID
int g_iSessionID = 0;

// 세션 구조체 관리 map
map<SOCKET, SESSION *>g_SessionMap;

//============================
// 함수 선언
//============================

//----------------------------
// 함수 헤더 정리
//----------------------------
void LoadData(void);
void ServerControl(void);
void Monitoring();

int DeadReckoningPos(DWORD dwAction, DWORD dwActionTick, int iOldPosX, int iOldPosY, int *piPosX, int *piPosY);
bool CharacterMoveCheck(short iX, short iY);
void SectorAddCharacter(st_CHARACTER *pCharacter);
void SectorRemoveCharacter(st_CHARACTER *pCharacter);
bool SectorUpdateCharacter(st_CHARACTER *pCharacter);
void GetSectorAround(int iSectorX, int iSectorY, st_SECTOR_AROUND *pSectorAround);
void GetUpdateSectorAround(st_CHARACTER *pCharacter, st_SECTOR_AROUND *pRemoveSector, st_SECTOR_AROUND *pAddSector);
void CharacterSectorUpdatePacket(st_CHARACTER *pCharacter);
bool FrameSkip();
void Update();

void SendPacket_Unicast(st_SESSION *pSession, CPacket *pPacket);
void SendPacket_Broadcast(CPacket *pPacket);
void SendPacket_SectorOne(int iSectorX, int iSectorY, CPacket *pPacket, st_SESSION *pExceptSession);
void SendPacket_Around(st_SESSION *pSession, CPacket *pPacket, bool bSendMe = false);
st_SESSION *FindSession(SOCKET Sock);
st_SESSION *CreateSession(SOCKET Sock);
st_CHARACTER *FindCharacter(DWORD dwSessionID);
st_CHARACTER *CreateCharacter(st_SESSION *pSession);
void Disconnect(SOCKET Sock);
bool NetStartUp(void);
void NetCleanUp(void);
void mpCreateCharacter(CPacket *pPacket, DWORD dwSessionID, BYTE byDir, short shX, short shY, char chHP);
void mpCreateOtherCharacter(CPacket *pPacket, DWORD dwSessionID, BYTE byDir, short shX, short shY, char chHP);
void mpDeleteCharacter(CPacket *pPacket, DWORD dwSessionID);
void mpMoveStart(CPacket *pPacket, DWORD dwSessionID, BYTE byDir, short shX, short shY);
void mpMoveStop(CPacket *pPacket, DWORD dwSessionID, BYTE byDir, short shX, short shY);
void mpAttack1(CPacket *pPacket, DWORD dwSessionID, BYTE byDir, short shX, short shY);
void mpAttack2(CPacket *pPacket, DWORD dwSessionID, BYTE byDir, short shX, short shY);
void mpAttack3(CPacket *pPacket, DWORD dwSessionID, BYTE byDir, short shX, short shY);
void mpDamage(CPacket *pPacket, DWORD dwAttackerID, DWORD dwDamagedID, char chDamagedHP);
void mpSync(CPacket *pPacket, DWORD dwSessionID, short shX, short shY);
bool NetPakcetProc_MoveStart(st_SESSION *pSession, CPacket *pPacket);
bool NetPacketProc_MoveStop(st_SESSION *pSession, CPacket *pPacket);
bool PacketProc(st_SESSION *pSession, BYTE byPacketType, CPacket *pPacket);
int ComplateRecvPacket(st_SESSION *pSession);
bool NetProcSend(SOCKET Socket);
void NetProcAccept();
void NetProcRecv(SOCKET Socket);
void NetSelectSocket(SOCKET *pTableSocket, FD_SET *pReadSet, FD_SET *pWriteSet);
void NetIOProc(void);

//----------------------------
// 제어부 함수
//----------------------------

// 설정 및 게임 데이터, DB 데이터 로딩
void LoadData(void) {

	// 로그 파일 초기화
	CLogMaker::GetInstance()->MakeLogFile("Log");
}

// 키보드 입력을 통해 서버를 제어
void ServerControl(void) {

	// D : 컨트롤 Lock / A : 컨트롤 Unlock / S : 서버 종료 / L : 로그 남기기
	
	// A : 컨트롤 모드 활성화
	if (GetAsyncKeyState(0x41) & 0x0001) {

		wprintf(L"Activate Console Mode\n");
		wprintf(L"S : Shutdown Server\n");
		g_bControlMode = true;
	}

	// D : 컨트롤 모드 비활성화
	if (GetAsyncKeyState(0x44) & 0x0001) {
		wprintf(L"Deactivate Console Mode\n");
		g_bControlMode = false;
	}

	// 컨트롤 모드가 활성화되어 있지 않다면 종료
	if (g_bControlMode == false)
		return;

	// 로그 남기기 (모드 변환도 되야함)
	//if (L'L' == ControlKey || L'l' == ControlKey)

	// S : 서버 종료
	if (GetAsyncKeyState(0x53) & 0x0001)
		g_bShutdown = true;
}

// 서버 모니터링 정보를 표시 및 저장, 전송 하는 경우 사용
void Monitoring() {

	static DWORD dwOldTime = NULL;
	static DWORD dwNowTime = 0;

	static int Loop = 1;

	if (dwOldTime == NULL) {

		dwOldTime = timeGetTime();

		return;
	}

	dwNowTime = timeGetTime();

	// 1초마다
	if (dwNowTime - dwOldTime >= 1000) {

		DWORD AvrFps = (DWORD)g_TotalFps / 1000;

		printf("================================================================\n");
		printf("|| Loop : %d | AvrFPS : %d | MaxFPS : %d | Min FPS : %d ||\n", Loop, AvrFps, g_MaxFps, g_MinFps);
		printf("================================================================\n");

		// Fps 변수들 초기화
		g_TotalFps = 0;
		g_MaxFps = 0;
		g_MinFps = 0xffffffff;

		dwOldTime = dwNowTime;

		Loop = 1;
	}

	Loop++;
}

//--------------------------------------------------------
// 컨텐츠 함수부
//--------------------------------------------------------
int DeadReckoningPos(DWORD dwAction, DWORD dwActionTick, int iOldPosX, int iOldPosY, int *piPosX, int *piPosY) {

	DWORD dwIntervalTick = timeGetTime() - dwActionTick;
	int iActionFrame = dwIntervalTick / 20;
	int iRemoveFrame = 0;
	int iVal;

	int iRPosX = iOldPosX;
	int iRPosY = iOldPosY;

	int iDX = iActionFrame * dfSPEED_PLAYER_X;
	int iDY = iActionFrame * dfSPEED_PLAYER_Y;

	switch (dwAction) {

	case dfPACKET_MOVE_DIR_LL:
		iRPosX = iOldPosX - iDX;
		iRPosY = iOldPosY;
		break;
	case dfPACKET_MOVE_DIR_LU:
		iRPosX = iOldPosX - iDX;
		iRPosY = iOldPosY - iDY;
		break;
	case dfPACKET_MOVE_DIR_UU:
		iRPosX = iOldPosX;
		iRPosY = iOldPosY - iDY;
		break;
	case dfPACKET_MOVE_DIR_RU:
		iRPosX = iOldPosX + iDX;
		iRPosY = iOldPosY - iDY;
		break;
	case dfPACKET_MOVE_DIR_RR:
		iRPosX = iOldPosX + iDX;
		iRPosY = iOldPosY;
		break;
	case dfPACKET_MOVE_DIR_RD:
		iRPosX = iOldPosX + iDX;
		iRPosY = iOldPosY + iDY;
		break;
	case dfPACKET_MOVE_DIR_DD:
		iRPosX = iOldPosX;
		iRPosY = iOldPosY + iDY;
		break;
	case dfPACKET_MOVE_DIR_LD:
		iRPosX = iOldPosX - iDX;
		iRPosY = iOldPosY + iDY;
		break;
	}

	if (iRPosX <= dfRANGE_MOVE_LEFT) {
		iVal = abs(dfRANGE_MOVE_LEFT - abs(iRPosX)) / dfSPEED_PLAYER_X;
		iRemoveFrame = max(iVal, iRemoveFrame);
	}
	if (iRPosX >= dfRANGE_MOVE_RIGHT) {
		iVal = abs(dfRANGE_MOVE_RIGHT - iRPosX) / dfSPEED_PLAYER_X;
		iRemoveFrame = max(iVal, iRemoveFrame);
	}
	if (iRPosY <= dfRANGE_MOVE_TOP) {
		iVal = abs(dfRANGE_MOVE_TOP - abs(iRPosY)) / dfSPEED_PLAYER_Y;
		iRemoveFrame = max(iVal, iRemoveFrame);
	}
	if (iRPosY >= dfRANGE_MOVE_BOTTOM) {
		iVal = abs(dfRANGE_MOVE_BOTTOM - iRPosY) / dfSPEED_PLAYER_Y;
		iRemoveFrame = max(iVal, iRemoveFrame);
	}

	if (iRemoveFrame > 0) {

		iActionFrame -= iRemoveFrame;

		iDX = iActionFrame * dfSPEED_PLAYER_X;
		iDY = iActionFrame * dfSPEED_PLAYER_Y;

		switch (dwAction) {

		case dfPACKET_MOVE_DIR_LL:
			iRPosX = iOldPosX - iDX;
			iRPosY = iOldPosY;
			break;
		case dfPACKET_MOVE_DIR_LU:
			iRPosX = iOldPosX - iDX;
			iRPosY = iOldPosY - iDY;
			break;
		case dfPACKET_MOVE_DIR_UU:
			iRPosX = iOldPosX;
			iRPosY = iOldPosY - iDY;
			break;
		case dfPACKET_MOVE_DIR_RU:
			iRPosX = iOldPosX + iDX;
			iRPosY = iOldPosY - iDY;
			break;
		case dfPACKET_MOVE_DIR_RR:
			iRPosX = iOldPosX + iDX;
			iRPosY = iOldPosY;
			break;
		case dfPACKET_MOVE_DIR_RD:
			iRPosX = iOldPosX + iDX;
			iRPosY = iOldPosY + iDY;
			break;
		case dfPACKET_MOVE_DIR_DD:
			iRPosX = iOldPosX;
			iRPosY = iOldPosY + iDY;
			break;
		case dfPACKET_MOVE_DIR_LD:
			iRPosX = iOldPosX - iDX;
			iRPosY = iOldPosY + iDY;
			break;
		}
	}

	iRPosX = min(iRPosX, dfRANGE_MOVE_RIGHT);
	iRPosX = max(iRPosX, dfRANGE_MOVE_LEFT);
	iRPosY = min(iRPosY, dfRANGE_MOVE_BOTTOM);
	iRPosY = max(iRPosY, dfRANGE_MOVE_TOP);

	if (iRPosX < dfRANGE_MOVE_LEFT) {
		iRPosX = dfRANGE_MOVE_LEFT;
	}
	if (iRPosX >= dfRANGE_MOVE_RIGHT) {
		iRPosX = dfRANGE_MOVE_RIGHT - 1;
	}
	if (iRPosY < dfRANGE_MOVE_TOP) {
		iRPosY = dfRANGE_MOVE_TOP;
	}
	if (iRPosY >= dfRANGE_MOVE_BOTTOM) {
		iRPosY = dfRANGE_MOVE_BOTTOM - 1;
	}

	*piPosX = iRPosX;
	*piPosY = iRPosY;

	if (iRPosX == 6400 || iRPosY == 6400) {
		int a = 0;
	}


	return iActionFrame;
}	

bool CharacterMoveCheck(short iX, short iY) {

	if (iX < dfRANGE_MOVE_LEFT || iX >= dfRANGE_MOVE_RIGHT)
		return false;

	if (iY < dfRANGE_MOVE_TOP || iY >= dfRANGE_MOVE_BOTTOM)
		return false;

	return true;
}

// 캐릭터 현재좌표(shX, shY)로 섹터위치 계산하여 해당 섹터에 넣기
void SectorAddCharacter(st_CHARACTER *pCharacter) {

	if (pCharacter->CurSector.iX != -1 || pCharacter->CurSector.iY != -1) {
		printf("AddSector Reject SessionID : %d\n",pCharacter->dwSessionID);
		return;
	}
	int iSectorX = pCharacter->shX / dfSECTOR_PIXEL_WIDTH;
	int iSectorY = pCharacter->shY / dfSECTOR_PIXEL_HEIGHT;

	if (iSectorX >= dfSECTOR_MAX_X || iSectorY >= dfSECTOR_MAX_Y) {
		printf("AddSector Out of Range : %d\n",pCharacter->dwSessionID);
		return;
	}

	g_Sector[iSectorY][iSectorX].push_back(pCharacter);
	pCharacter->OldSector.iX = pCharacter->CurSector.iX = iSectorX;
	pCharacter->OldSector.iY = pCharacter->CurSector.iY = iSectorY;

	if (iSectorX == -1 || iSectorY == -1) {
		int a = 0;
	}
	//printf("# AddSector # pSessionID : %d, InSectorX : %d, InSectorY : %d, PosX : %d, PosY : %d\n",
	//	pCharacter->dwSessionID, pCharacter->CurSector.iX, pCharacter->CurSector.iY,
	//	pCharacter->shX, pCharacter->shY);
}

// 캐릭터의 현재좌표(shX, shY)로 섹터위치 계산하여 해당 섹터에서 삭제
void SectorRemoveCharacter(st_CHARACTER *pCharacter) {

	// 캐릭터에 섹터정보가 없다면 문제있는 상황
	if (pCharacter->CurSector.iX == -1 || pCharacter->CurSector.iY == -1) {
		printf("SectorRemove Reject, SessionID : %d\n", pCharacter->dwSessionID);
		return;
	}

	list<st_CHARACTER *> &SectorList = g_Sector[pCharacter->CurSector.iY][pCharacter->CurSector.iX];
	list<st_CHARACTER *>::iterator SectorIter = SectorList.begin();
	list<st_CHARACTER *>::iterator SectorIterEnd = SectorList.end();

	while (SectorIter != SectorIterEnd) {

		if (pCharacter == *SectorIter) {

			SectorList.erase(SectorIter);
			break;
		}
		// 문제여지 있지 않음?
		++SectorIter;
	}

	pCharacter->OldSector.iX = pCharacter->CurSector.iX;
	pCharacter->OldSector.iY = pCharacter->CurSector.iY;
	pCharacter->CurSector.iX = -1;
	pCharacter->CurSector.iY = -1;

	//printf("# RemoveSector # pSessionID : %d, OutSectorX : %d, OutSectorY : %d, PosX : %d, PosY : %d\n",
	//	pCharacter->dwSessionID, pCharacter->OldSector.iX, pCharacter->OldSector.iY,
	//	pCharacter->shX, pCharacter->shY);
}

// 현재 위치한 섹터에서 삭제 후, 현재 좌표로 새로운 해당 섹터에 넣기
bool SectorUpdateCharacter(st_CHARACTER *pCharacter) {

	int iBeforeSectorX = pCharacter->CurSector.iX;
	int iBeforeSectorY = pCharacter->CurSector.iY;

	int iNewSectorX = pCharacter->shX / dfSECTOR_PIXEL_WIDTH;
	int iNewSectorY = pCharacter->shY / dfSECTOR_PIXEL_HEIGHT;

	if (iBeforeSectorX == iNewSectorX && iBeforeSectorY == iNewSectorY)
		return false;

	SectorRemoveCharacter(pCharacter);
	SectorAddCharacter(pCharacter);

	pCharacter->OldSector.iX = iBeforeSectorX;
	pCharacter->OldSector.iY = iBeforeSectorY;

	return true;
}

// 특정 섹터 좌표 기준 영향권 섹터 얻기
void GetSectorAround(int iSectorX, int iSectorY, st_SECTOR_AROUND *pSectorAround) {

	int iCntX;
	int iCntY;

	iSectorX--;
	iSectorY--;

	pSectorAround->iCount = 0;

	for (iCntY = 0; iCntY < 3; iCntY++) {

		if (iSectorY + iCntY < 0 || iSectorY + iCntY >= dfSECTOR_MAX_Y)
			continue;

		for (iCntX = 0; iCntX < 3; iCntX++) {

			if (iSectorX + iCntX < 0 || iSectorX + iCntX >= dfSECTOR_MAX_X)
				continue;

			pSectorAround->Around[pSectorAround->iCount].iX = iSectorX + iCntX;
			pSectorAround->Around[pSectorAround->iCount].iY = iSectorY + iCntY;
			pSectorAround->iCount++;
		}
	}
}

// 섹터에서 섹터를 이동 했을때, 섹터 영향권에서 빠진 섹터, 새로 추가된 섹터의 정보 구함
void GetUpdateSectorAround(st_CHARACTER *pCharacter, st_SECTOR_AROUND *pRemoveSector, st_SECTOR_AROUND *pAddSector) {

	int iCntOld, iCntCur;
	bool bFind;
	st_SECTOR_AROUND OldSectorAround;
	st_SECTOR_AROUND CurSectorAround;

	OldSectorAround.iCount = 0;
	CurSectorAround.iCount = 0;

	pRemoveSector->iCount = 0;
	pAddSector->iCount = 0;

	GetSectorAround(pCharacter->OldSector.iX, pCharacter->OldSector.iY, &OldSectorAround);
	GetSectorAround(pCharacter->CurSector.iX, pCharacter->CurSector.iY, &CurSectorAround);

	for (iCntOld = 0; iCntOld < OldSectorAround.iCount; iCntOld++) {

		bFind = false;
		for (iCntCur = 0; iCntCur < CurSectorAround.iCount; iCntCur++) {

			if (OldSectorAround.Around[iCntOld].iX == CurSectorAround.Around[iCntCur].iX &&
				OldSectorAround.Around[iCntOld].iY == CurSectorAround.Around[iCntCur].iY) {

				bFind = true;
				break;
			}
		}

		if (bFind == false) {
			
			// 신기하게 짜놨음
			pRemoveSector->Around[pRemoveSector->iCount] = OldSectorAround.Around[iCntOld];
			pRemoveSector->iCount++;
		}
	}

	for (iCntCur = 0; iCntCur < CurSectorAround.iCount; iCntCur++) {

		bFind = false;
		for (iCntOld = 0; iCntOld < OldSectorAround.iCount; iCntOld++) {

			if (CurSectorAround.Around[iCntCur].iX == OldSectorAround.Around[iCntOld].iX &&
				CurSectorAround.Around[iCntCur].iY == OldSectorAround.Around[iCntOld].iY) {

				bFind = true;
				break;
			}
		}

		if (bFind == false) {

			pAddSector->Around[pAddSector->iCount] = CurSectorAround.Around[iCntCur];
			pAddSector->iCount++;
		}
	}
}

// 섹터가 변경된 경우 클라에게 관련 정보 전송
void CharacterSectorUpdatePacket(st_CHARACTER *pCharacter) {

	st_SECTOR_AROUND RemoveSector;
	st_SECTOR_AROUND AddSector;

	st_CHARACTER *pExistCharacter;

	list<st_CHARACTER *> *pSectorList = NULL;
		
	CPacket Packet;
	int iCnt;

	GetUpdateSectorAround(pCharacter, &RemoveSector, &AddSector);

	// 1. RemoveSector에 캐릭터 삭제 패킷보내기
	mpDeleteCharacter(&Packet, pCharacter->dwSessionID);

	for (iCnt = 0; iCnt < RemoveSector.iCount; iCnt++) {

		// 특정 섹터 공간에만 메시지를 전달하는 함수
		SendPacket_SectorOne(RemoveSector.Around[iCnt].iX, 
			RemoveSector.Around[iCnt].iY, 
			&Packet, NULL);
	}

	// 2. 지금 움직이는 녀석에게, RemoveSector의 캐릭터들 삭제 패킷 보내기
	for (iCnt = 0; iCnt < RemoveSector.iCount; iCnt++) {

		pSectorList = &g_Sector[RemoveSector.Around[iCnt].iY][RemoveSector.Around[iCnt].iX];

		list<st_CHARACTER *>::iterator ListIter;
		list<st_CHARACTER *>::iterator ListIterEnd = pSectorList->end();
		for (ListIter = pSectorList->begin(); ListIter != ListIterEnd; ++ListIter) {

			mpDeleteCharacter(&Packet, (*ListIter)->dwSessionID);

			// 특정 클라이언트 한 명에게 메시지 전송
			SendPacket_Unicast(pCharacter->pSession, &Packet);
		}
	}

	// 3. AddSectors 에 생성 패킷 보내기
	mpCreateOtherCharacter(&Packet, pCharacter->dwSessionID,
		pCharacter->byDirection,
		pCharacter->shX,
		pCharacter->shY,
		pCharacter->chHP);

	for (iCnt = 0; iCnt < AddSector.iCount; iCnt++) {

		SendPacket_SectorOne(AddSector.Around[iCnt].iX, AddSector.Around[iCnt].iY, &Packet, NULL);
	}

	// 3-1. AddSector에 캐릭터 이동 패킷 보내기
	mpMoveStart(&Packet, pCharacter->dwSessionID,
		pCharacter->byMoveDirection,
		pCharacter->shX,
		pCharacter->shY);

	for (iCnt = 0; iCnt < AddSector.iCount; iCnt++) {

		SendPacket_SectorOne(AddSector.Around[iCnt].iX, AddSector.Around[iCnt].iY, &Packet, NULL);
	}

	// 4. 이동한 녀석에게 AddSector에 있는 캐릭터 생성 패킷 보내기
	for (iCnt = 0; iCnt < AddSector.iCount; iCnt++) {

		// 얻어진 섹터를 돌면서 섹터리스트 접근
		pSectorList = &g_Sector[AddSector.Around[iCnt].iY][AddSector.Around[iCnt].iX];

		list<st_CHARACTER *>::iterator ListIter;
		list<st_CHARACTER *>::iterator ListIterEnd = pSectorList->end();
		// 해당 섹터마다 등록된 캐릭터들을 뽑아서 생성 패킷 만들어 보냄
		for (ListIter = pSectorList->begin(); ListIter != ListIterEnd; ++ListIter) {

			pExistCharacter = *ListIter;

			// 내가 아닌 경우에만
			if (pExistCharacter != pCharacter) {

				mpCreateOtherCharacter(&Packet,
					pExistCharacter->dwSessionID,
					pExistCharacter->byDirection,
					pExistCharacter->shX,
					pExistCharacter->shY,
					pExistCharacter->chHP);

				SendPacket_Unicast(pCharacter->pSession, &Packet);

				// 새 Add 캐릭터의 액션에 따른 패킷 만들어서 보냄
				switch (pExistCharacter->dwAction) {

				case dfPACKET_MOVE_DIR_LL:
				case dfPACKET_MOVE_DIR_LU:
				case dfPACKET_MOVE_DIR_UU:
				case dfPACKET_MOVE_DIR_RU:
				case dfPACKET_MOVE_DIR_RR:
				case dfPACKET_MOVE_DIR_RD:
				case dfPACKET_MOVE_DIR_DD:
				case dfPACKET_MOVE_DIR_LD:
					mpMoveStart(&Packet,
						pExistCharacter->dwSessionID,
						pExistCharacter->byDirection,
						pExistCharacter->shX,
						pExistCharacter->shY);
					SendPacket_Unicast(pCharacter->pSession, &Packet);
					break;
				case dfPACKET_MOVE_ATTACK1:
					mpAttack1(&Packet,
						pExistCharacter->dwSessionID,
						pExistCharacter->byDirection,
						pExistCharacter->shX,
						pExistCharacter->shY);
					SendPacket_Unicast(pCharacter->pSession, &Packet);
					break;
				case dfPACKET_MOVE_ATTACK2:
					mpAttack2(&Packet,
						pExistCharacter->dwSessionID,
						pExistCharacter->byDirection,
						pExistCharacter->shX,
						pExistCharacter->shY);
					SendPacket_Unicast(pCharacter->pSession, &Packet);
					break;
				case dfPACKET_MOVE_ATTACK3:
					mpAttack3(&Packet,
						pExistCharacter->dwSessionID,
						pExistCharacter->byDirection,
						pExistCharacter->shX,
						pExistCharacter->shY);
					SendPacket_Unicast(pCharacter->pSession, &Packet);
					break;
				}
			}
		}
	}
}

// Frame()함수, true : 아래로 진행, false : 진행 안함
bool FrameSkip() {

	int iResult = true;

	static DWORD dwOldTime = NULL;
	static DWORD dwRemainTime = 0;
	
	if (dwOldTime == NULL) {

		dwOldTime = timeGetTime();

		return true;
	}

	DWORD dwTime = timeGetTime() - dwOldTime;
	//g_TotalFps += dwTime;
	//g_MaxFps = max(dwTime, g_MaxFps);
	//g_MinFps = min(dwTime, g_MinFps);

	// 넘어간 시간이 20을 넘었다면
	if (dwRemainTime >= dfFRAME_SLEEP_TIME) {

		dwRemainTime += dwTime;
		dwRemainTime -= dfFRAME_SLEEP_TIME;

		iResult = true;
	}

	// 시간이 20 이하
	else if (dwTime < dfFRAME_SLEEP_TIME) {

		dwRemainTime += dwTime;

		iResult = false;
	}

	// 시간이 20ms 이상
	else if (dwTime >= dfFRAME_SLEEP_TIME) {

		dwRemainTime += dwTime - dfFRAME_SLEEP_TIME;

		iResult = true;
	}

	dwOldTime = timeGetTime();
	return iResult;
}

// Update 함수
void Update() {

	st_CHARACTER *pCharacter = NULL;

	map<DWORD, st_CHARACTER *>::iterator CharacterIter = g_CharacterMap.begin();
	map<DWORD, st_CHARACTER *>::iterator CharacterIterEnd = g_CharacterMap.end();

	for (; CharacterIter != CharacterIterEnd;) {

		pCharacter = CharacterIter->second;
		++CharacterIter;
		// 서버에서 테스트를 위해 주석
		//if (pCharacter->chHP <= 0) {
		//	// 사망처리
		//	printf("SessionID : %d HP = 0\n", pCharacter->dwSessionID);
		//	Disconnect(pCharacter->pSession->Socket);
		//}
		
		// 일정 시간동안 수신 없으면 종료처리
		/*DWORD dwCurrTick = timeGetTime();
		if (dwCurrTick - pCharacter->pSession->dwTrafficTick > dfNETWORK_PACKET_RECV_TIMEOUT) {

			printf("Timeout SessionID: %d\n", pCharacter->dwSessionID);
			Disconnect(pCharacter->pSession->Socket);
			continue;
		}*/

		// 현재 동작에 따른 처리
		switch (pCharacter->dwAction) {

		case dfPACKET_MOVE_DIR_LL:
			if (CharacterMoveCheck(pCharacter->shX - dfSPEED_PLAYER_X,
				pCharacter->shY)) {

				pCharacter->shX -= dfSPEED_PLAYER_X;
			}
			break;
		case dfPACKET_MOVE_DIR_LU:
			if (CharacterMoveCheck(pCharacter->shX - dfSPEED_PLAYER_X,
				pCharacter->shY - dfSPEED_PLAYER_Y)) {

				pCharacter->shX -= dfSPEED_PLAYER_X;
				pCharacter->shY -= dfSPEED_PLAYER_Y;
			}
			break;
		case dfPACKET_MOVE_DIR_UU:
			if (CharacterMoveCheck(pCharacter->shX,
				pCharacter->shY - dfSPEED_PLAYER_Y)) {

				pCharacter->shY -= dfSPEED_PLAYER_Y;
			}
			break;
		case dfPACKET_MOVE_DIR_RU:
			if (CharacterMoveCheck(pCharacter->shX + dfSPEED_PLAYER_X,
				pCharacter->shY - dfSPEED_PLAYER_Y)) {

				pCharacter->shX += dfSPEED_PLAYER_X;
				pCharacter->shY -= dfSPEED_PLAYER_Y;
			}
			break;
		case dfPACKET_MOVE_DIR_RR:
			if (CharacterMoveCheck(pCharacter->shX + dfSPEED_PLAYER_X,
				pCharacter->shY)) {

				pCharacter->shX += dfSPEED_PLAYER_X;
				if (pCharacter->shX == 6400) {
					int a = 0;
				}
			}
			break;
		case dfPACKET_MOVE_DIR_RD:
			if (CharacterMoveCheck(pCharacter->shX + dfSPEED_PLAYER_X,
				pCharacter->shY + dfSPEED_PLAYER_Y)) {

				pCharacter->shX += dfSPEED_PLAYER_X;
				pCharacter->shY += dfSPEED_PLAYER_Y;
			}
			break;
		case dfPACKET_MOVE_DIR_DD:
			if (CharacterMoveCheck(pCharacter->shX,
				pCharacter->shY + dfSPEED_PLAYER_Y)) {

				pCharacter->shY += dfSPEED_PLAYER_Y;
			}
			break;
		case dfPACKET_MOVE_DIR_LD:
			if (CharacterMoveCheck(pCharacter->shX - dfSPEED_PLAYER_X,
				pCharacter->shY + dfSPEED_PLAYER_Y)) {

				pCharacter->shX -= dfSPEED_PLAYER_X;
				pCharacter->shY += dfSPEED_PLAYER_Y;
			}
			break;
		}

		if (pCharacter->dwAction >= dfPACKET_MOVE_DIR_LL && pCharacter->dwAction <= dfPACKET_MOVE_DIR_LD) {

			// 이동인 경우 섹터 업데이트를 함
			if (SectorUpdateCharacter(pCharacter)) {

				// 섹터가 변경된 경우는 클라에게 관련 정보를 보낸다
				CharacterSectorUpdatePacket(pCharacter);
			}
		}
	}
}

//--------------------------------------------------------
// 네트워크 함수부
//--------------------------------------------------------

// SendUnicast ( 한 세션에 대해 )
void SendPacket_Unicast(st_SESSION *pSession, CPacket *pPacket) {

	// Session이 NULL
	if (pSession == NULL) {
		// Error 로그처리?
		return;
	}

	// 패킷에 내용이 없을 때
	if (pPacket->GetPacketSize() <= 0) {

		return;
	}

	pSession->SendQ.Enqueue((char *)pPacket->GetReadPtr(), pPacket->GetPacketSize());
}

// SendBroadcast (모든 세션에 대해)
void SendPacket_Broadcast(CPacket *pPacket) {

	st_SESSION *pSession = NULL;

	map<SOCKET, st_SESSION *>::iterator SessionIter;
	map<SOCKET, st_SESSION *>::iterator SessionIterEnd = g_SessionMap.end();

	for (SessionIter = g_SessionMap.begin(); SessionIter != SessionIterEnd; ++SessionIter) {

		pSession = SessionIter->second;
		SendPacket_Unicast(pSession, pPacket);
	}
}

// SendSectorOne ( 한 섹터에 있고 ExceptSession을 제외한 모든 세션에 보내기 )
void SendPacket_SectorOne(int iSectorX, int iSectorY, CPacket *pPacket, st_SESSION *pExceptSession) {

	st_CHARACTER *pCharacter = NULL;
	list<st_CHARACTER *>::iterator SectorIter = g_Sector[iSectorY][iSectorX].begin();
	list<st_CHARACTER *>::iterator SectorIterEnd = g_Sector[iSectorY][iSectorX].end();

	for (; SectorIter != SectorIterEnd; ++SectorIter) {

		pCharacter = *SectorIter;
		if (pExceptSession == pCharacter->pSession)
			continue;
		
		SendPacket_Unicast(pCharacter->pSession, pPacket);
	}
}

// SendSectorAround
void SendPacket_Around(st_SESSION *pSession, CPacket *pPacket, bool bSendMe) {

	st_CHARACTER *pCharacter = FindCharacter(pSession->dwSessionID);

	int CurSectorX = pCharacter->CurSector.iX;
	int CurSectorY = pCharacter->CurSector.iY;

	int StartSectorX = CurSectorX - 1;
	int StartSectorY = CurSectorY - 1;
	int EndSectorX = CurSectorX + 1;
	int EndSectorY = CurSectorY + 1;

	if (StartSectorX < 0)
		StartSectorX = 0;
	if (StartSectorY < 0)
		StartSectorY = 0;
	if (EndSectorX >= dfSECTOR_MAX_X)
		EndSectorX = dfSECTOR_MAX_X - 1;
	if (EndSectorY >= dfSECTOR_MAX_Y)
		EndSectorY = dfSECTOR_MAX_Y - 1;

	if(bSendMe) {
		for (int iY = StartSectorY; iY <= EndSectorY; iY++) {

			for (int iX = StartSectorX; iX <= EndSectorX; iX++) {
				
				SendPacket_SectorOne(iX, iY, pPacket, NULL);
			}
		}
	}

	else {

		for (int iY = StartSectorY; iY <= EndSectorY; iY++) {

			for (int iX = StartSectorX; iX <= EndSectorX; iX++) {

				st_SESSION *pExceptSession = NULL;
				if (iY == CurSectorY && iX == CurSectorX)
					pExceptSession = pSession;
				
				SendPacket_SectorOne(iX, iY, pPacket, pExceptSession);
			}
		}
	}
}

// Socket으로 Session 찾기
st_SESSION *FindSession(SOCKET Sock) {

	return g_SessionMap.find(Sock)->second;
}

// 새 Session 생성
st_SESSION *CreateSession(SOCKET Sock) {

	st_SESSION *pSession = new st_SESSION;
	pSession->Socket = Sock;
	pSession->dwSessionID = ++g_iSessionID;
	pSession->dwTrafficCount = 0;
	pSession->dwTrafficTick = timeGetTime();
	pSession->dwTrafficPerSecTick = 0;

	g_SessionMap.insert(make_pair(Sock, pSession));

	return pSession;
}

// SessionID로 Chracter 찾기
st_CHARACTER *FindCharacter(DWORD dwSessionID) {

	return g_CharacterMap.find(dwSessionID)->second;
}

// Session을 넣어 새로운 Character 생성
st_CHARACTER *CreateCharacter(st_SESSION *pSession) {

	st_CHARACTER *pCharacter = new st_CHARACTER;
	short shX = rand() % 6400;
	short shY = rand() % 6400;

	if (shX == 6400 || shY == 6400) {
		int a = 0;
	}

	pCharacter->pSession = pSession;
	pCharacter->dwSessionID = pSession->dwSessionID;
	pCharacter->byDirection = dfPACKET_MOVE_DIR_LL;
	pCharacter->dwAction = dfPACKET_MOVE_STAND;
	pCharacter->shX = shX;
	pCharacter->shY = shY;
	pCharacter->chHP = 100;
	
	// 섹터부분
	SectorAddCharacter(pCharacter);

	// 캐릭터 맵에 추가
	g_CharacterMap.insert(make_pair(pSession->dwSessionID, pCharacter));
	
	return pCharacter;
}

// 세션 및 캐릭터 지우기 ( 꼭 찾게 만들어야하나? pSession 넣어서 바로 해결 불가?)
void Disconnect(SOCKET Sock) {

	// 캐릭터 및 세션 지우기
	st_SESSION *pSession = NULL;
	st_CHARACTER *pCharacter = NULL;
	st_SECTOR_AROUND SectorAround;

	pSession = g_SessionMap.find(Sock)->second;
	g_SessionMap.erase(Sock);
	
	pCharacter = g_CharacterMap.find(pSession->dwSessionID)->second;
	g_CharacterMap.erase(pSession->dwSessionID);

	// 관련 섹터에 해당 캐릭터 접속 끊기
	// 9개 섹터에 있는 모든 유저에게 메시지 송신
	CPacket Packet;
	mpDeleteCharacter(&Packet, pSession->dwSessionID);
	
	int iSectorX = pCharacter->CurSector.iX;
	int iSectorY = pCharacter->CurSector.iY;
	GetSectorAround(iSectorX, iSectorY, &SectorAround);
	
	for (int iCnt = 0; iCnt < SectorAround.iCount; iCnt++) {

		int iX = SectorAround.Around[iCnt].iX;
		int iY = SectorAround.Around[iCnt].iY;
		list<st_CHARACTER *> *CharacterInSector = &g_Sector[iY][iX];

		list<st_CHARACTER *>::iterator CharacterIter = CharacterInSector->begin();
		list<st_CHARACTER *>::iterator CharacterIterEnd = CharacterInSector->end();

		int iSize = (int)CharacterInSector->size();

		for (; CharacterIter != CharacterIterEnd; ++CharacterIter) {

			st_CHARACTER *pCharacterOther = *CharacterIter;
			
			if (pCharacterOther == pCharacter) {

				SOCKET Sock = pCharacterOther->pSession->Socket;
				send(Sock, Packet.GetReadPtr(), Packet.GetPacketSize(), 0);
				continue;
			}

			pCharacterOther->pSession->SendQ.Enqueue(Packet.GetReadPtr(), Packet.GetPacketSize());
		}
	}

	//printf("DisConnect SessionID : %d\n", pSession->dwSessionID);

	// 해당 섹터 지우기
	SectorRemoveCharacter(pCharacter);

	//CLogMaker::GetInstance()->Log(dfLOG_LEVEL_DEBUG, 
	//	"Disconnect SessionID : %d\n", pSession->dwSessionID);
	
	// closesocket
	closesocket(pSession->Socket);

	// 메모리 해제
	delete pSession;
	delete pCharacter;
}

// 네트워크 초기화 및 listen
bool NetStartUp(void) {

	// 윈속 시작
	if (WSAStartup(MAKEWORD(2, 2), &g_wsa) != 0)
		return false;

	// ListenSocket
	g_ListenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_ListenSock == INVALID_SOCKET) {
		wprintf(L"socket()_ERROR\n");
		return false;
	}

	// bind()
	ZeroMemory(&g_ServerAddr, sizeof(g_ServerAddr));
	g_ServerAddr.sin_family = AF_INET;
	InetPton(AF_INET, INADDR_ANY, &g_ServerAddr.sin_addr);
	g_ServerAddr.sin_port = htons(dfSERVERPORT);
	int retval = bind(g_ListenSock, (SOCKADDR *)&g_ServerAddr, sizeof(g_ServerAddr));
	if (retval == SOCKET_ERROR) {
		wprintf(L"bind()_ERROR\n");
		return false;
	}

	// listen()
	retval = listen(g_ListenSock, SOMAXCONN);
	if (retval == SOCKET_ERROR) {
		wprintf(L"listen()_ERROR\n");
		return false;
	}

	// change nonblock sock
	u_long on = 1;
	retval = ioctlsocket(g_ListenSock, FIONBIO, &on);
	if (retval == SOCKET_ERROR) {
		wprintf(L"ioctlsocket()_ERROR\n");
	}

	wprintf(L"Server Open...\n");

	return true;
}

// 네트워크 종료
void NetCleanUp(void) {

	closesocket(g_ListenSock);

	WSACleanup();
}

// mpCreateCharacter
void mpCreateCharacter(CPacket *pPacket, DWORD dwSessionID, BYTE byDir, short shX, short shY, char chHP) {

	st_PACKET_HEADER stHeader;
	stHeader.byCode = dfNETWORK_PACKET_CODE;
	stHeader.bySize = 10;
	stHeader.byType = dfPACKET_SC_CREATE_MY_CHARACTER;

	pPacket->Clear();
	pPacket->Enqueue((char *)&stHeader, sizeof(st_PACKET_HEADER));
	*pPacket << dwSessionID;
	*pPacket << byDir;
	*pPacket << shX;
	*pPacket << shY;
	*pPacket << chHP;
	*pPacket << (BYTE)dfNETWORK_PACKET_END;

	//char buf[20] = { 0, };
	//memcpy(buf, pPacket->GetReadPtr(), pPacket->GetPacketSize());
	int breakPoint = 0;
}

// mpCreateOtherCharacter
void mpCreateOtherCharacter(CPacket *pPacket, DWORD dwSessionID, BYTE byDir, short shX, short shY, char chHP) {

	st_PACKET_HEADER stHeader;
	stHeader.byCode = dfNETWORK_PACKET_CODE;
	stHeader.bySize = 10;
	stHeader.byType = dfPACKET_SC_CREATE_OTHER_CHARACTER;

	pPacket->Clear();
	pPacket->Enqueue((char *)&stHeader, sizeof(st_PACKET_HEADER));
	*pPacket << dwSessionID;
	*pPacket << byDir;
	*pPacket << shX;
	*pPacket << shY;
	*pPacket << chHP;
	*pPacket << (BYTE)dfNETWORK_PACKET_END;
}

// mpDeleteCharacter
void mpDeleteCharacter(CPacket *pPacket, DWORD dwSessionID) {

	st_PACKET_HEADER stHeader;
	stHeader.byCode = dfNETWORK_PACKET_CODE;
	stHeader.bySize = 4;
	stHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

	pPacket->Clear();
	pPacket->Enqueue((char *)&stHeader, sizeof(st_PACKET_HEADER));
	*pPacket << dwSessionID;
	*pPacket << (BYTE)dfNETWORK_PACKET_END;
}

// mpMoveStart
void mpMoveStart(CPacket *pPacket, DWORD dwSessionID, BYTE byDir, short shX, short shY) {

	st_PACKET_HEADER stHeader;
	stHeader.byCode = dfNETWORK_PACKET_CODE;
	stHeader.bySize = 9;
	stHeader.byType = dfPACKET_SC_MOVE_START;

	pPacket->Clear();
	pPacket->Enqueue((char *)&stHeader, sizeof(st_PACKET_HEADER));
	*pPacket << dwSessionID;
	*pPacket << byDir;
	*pPacket << shX;
	*pPacket << shY;
	*pPacket << (BYTE)dfNETWORK_PACKET_END;
}

// mpMoveStop
void mpMoveStop(CPacket *pPacket, DWORD dwSessionID, BYTE byDir, short shX, short shY) {

	st_PACKET_HEADER stHeader;
	stHeader.byCode = dfNETWORK_PACKET_CODE;
	stHeader.bySize = 9;
	stHeader.byType = dfPACKET_SC_MOVE_STOP;

	pPacket->Clear();
	pPacket->Enqueue((char *)&stHeader, sizeof(st_PACKET_HEADER));
	*pPacket << dwSessionID;
	*pPacket << byDir;
	*pPacket << shX;
	*pPacket << shY;
	*pPacket << (BYTE)dfNETWORK_PACKET_END;
}

// mpAttack1
void mpAttack1(CPacket *pPacket, DWORD dwSessionID, BYTE byDir, short shX, short shY) {

	st_PACKET_HEADER stHeader;
	stHeader.byCode = dfNETWORK_PACKET_CODE;
	stHeader.bySize = 9;
	stHeader.byType = dfPACKET_SC_ATTACK1;

	pPacket->Clear();
	pPacket->Enqueue((char *)&stHeader, sizeof(st_PACKET_HEADER));
	*pPacket << dwSessionID;
	*pPacket << byDir;
	*pPacket << shX;
	*pPacket << shY;
	*pPacket << (BYTE)dfNETWORK_PACKET_END;
}

// mpAttack2
void mpAttack2(CPacket *pPacket, DWORD dwSessionID, BYTE byDir, short shX, short shY) {

	st_PACKET_HEADER stHeader;
	stHeader.byCode = dfNETWORK_PACKET_CODE;
	stHeader.bySize = 9;
	stHeader.byType = dfPACKET_SC_ATTACK2;

	pPacket->Clear();
	pPacket->Enqueue((char *)&stHeader, sizeof(st_PACKET_HEADER));
	*pPacket << dwSessionID;
	*pPacket << byDir;
	*pPacket << shX;
	*pPacket << shY;
	*pPacket << (BYTE)dfNETWORK_PACKET_END;
}

// mpAttack3
void mpAttack3(CPacket *pPacket, DWORD dwSessionID, BYTE byDir, short shX, short shY) {

	st_PACKET_HEADER stHeader;
	stHeader.byCode = dfNETWORK_PACKET_CODE;
	stHeader.bySize = 9;
	stHeader.byType = dfPACKET_SC_ATTACK3;

	pPacket->Clear();
	pPacket->Enqueue((char *)&stHeader, sizeof(st_PACKET_HEADER));
	*pPacket << dwSessionID;
	*pPacket << byDir;
	*pPacket << shX;
	*pPacket << shY;
	*pPacket << (BYTE)dfNETWORK_PACKET_END;
}

// mpDamage
void mpDamage(CPacket *pPacket, DWORD dwAttackerID, DWORD dwDamagedID, char chDamagedHP){

	st_PACKET_HEADER stHeader;
	stHeader.byCode = dfNETWORK_PACKET_CODE;
	stHeader.bySize = 9;
	stHeader.byType = dfPACKET_SC_DAMAGE;

	pPacket->Clear();
	pPacket->Enqueue((char *)&stHeader, sizeof(st_PACKET_HEADER));
	*pPacket << dwAttackerID;
	*pPacket << dwDamagedID;
	*pPacket << chDamagedHP;
	*pPacket << (BYTE)dfNETWORK_PACKET_END;
}

// mpSync
void mpSync(CPacket *pPacket, DWORD dwSessionID, short shX, short shY) {

	st_PACKET_HEADER stHeader;
	stHeader.byCode = dfNETWORK_PACKET_CODE;
	stHeader.bySize = 8;
	stHeader.byType = dfPACKET_SC_SYNC;

	pPacket->Clear();
	pPacket->Enqueue((char *)&stHeader, sizeof(st_PACKET_HEADER));
	*pPacket << dwSessionID;
	*pPacket << shX;
	*pPacket << shY;
	*pPacket << (BYTE)dfNETWORK_PACKET_END;
}

// mpEcho
void mpEcho(CPacket *pPacket) {

	DWORD dwEcho;
	*pPacket >> dwEcho;

	*pPacket << (BYTE)dfNETWORK_PACKET_CODE;
	*pPacket << (BYTE)4;
	*pPacket << (BYTE)dfPACKET_SC_ECHO;
	*pPacket << (BYTE)0;

	*pPacket << dwEcho;
	*pPacket << (BYTE)dfNETWORK_PACKET_END;
}

// NetMoveStart
bool NetPakcetProc_MoveStart(st_SESSION *pSession, CPacket *pPacket) {

	if (pSession == NULL) {
		// 큰에러임
		printf("# MoveStart # Session is NULL!!!\n");
	}

	BYTE byDirection = 0;
	short shX = 0;
	short shY = 0;

	*pPacket >> byDirection;
	*pPacket >> shX;
	*pPacket >> shY;

	//printf("# MoveStart # Session ID : %d, Direction : %d, X : %d, Y : %d\n",
	//	pSession->dwSessionID, byDirection, shX, shY);

	st_CHARACTER *pCharacter = FindCharacter(pSession->dwSessionID);
	if (pCharacter == NULL) {

		/*CLogMaker::GetInstance()->Log(dfLOG_LEVEL_ERROR,
			"# MoveStart > SessionID : %d Character Not Found!",
			pSession->dwSessionID);*/
		printf("# MoveStart > SessionID : %d Character Not Found!\n",
			pSession->dwSessionID);

		return false;
	}

	if (abs(pCharacter->shX - shX) > dfERROR_RANGE ||
		abs(pCharacter->shY - shY) > dfERROR_RANGE) {

		int idrX;
		int idrY;

		int iDeadFrame = DeadReckoningPos(pCharacter->dwAction,
											pCharacter->dwActionTick,
											pCharacter->shActionX,
											pCharacter->shActionY,
											&idrX, &idrY);
		
		if (abs(idrX - shX) > dfERROR_RANGE || 
			abs(idrY - shY) > dfERROR_RANGE) {

			mpSync(pPacket, pCharacter->dwSessionID, idrX, idrY);
			SendPacket_Around(pCharacter->pSession, pPacket, true);
			// SendPacket_Broadcast(pPacket);
			printf("# MoveStart # Sync!!!! SessionID : %d\n", pCharacter->dwSessionID);
			printf("# MoveStart # Sync!!!! SessionID : %d\n", pCharacter->dwSessionID);
		}
		shX = idrX;
		shY = idrY;
	}

	pCharacter->dwAction = byDirection;

	pCharacter->byMoveDirection = byDirection;

	switch (byDirection) {

	case dfPACKET_MOVE_DIR_RR:
	case dfPACKET_MOVE_DIR_RU:
	case dfPACKET_MOVE_DIR_RD:
		pCharacter->byDirection = dfPACKET_MOVE_DIR_RR;
		break;
	case dfPACKET_MOVE_DIR_LL:
	case dfPACKET_MOVE_DIR_LU:
	case dfPACKET_MOVE_DIR_LD:
		pCharacter->byDirection = dfPACKET_MOVE_DIR_LL;
		break;
	}

	// 실제 좌표 변경( shX, shY 는 이미 데드레커닝 작업 후)
	pCharacter->shX = shX;
	pCharacter->shY = shY;

	if (SectorUpdateCharacter(pCharacter)) {

		CharacterSectorUpdatePacket(pCharacter);
	}

	// 데드레커닝을 위한 정보 저장
	pCharacter->dwActionTick = timeGetTime();
	pCharacter->shActionX = pCharacter->shX;
	pCharacter->shActionY = pCharacter->shY;

	mpMoveStart(pPacket, pSession->dwSessionID, byDirection, pCharacter->shX, pCharacter->shY);

	SendPacket_Around(pSession, pPacket);
	//SendPacket_Broadcast(pPacket);

	return true;
}

// NetMoveStop
bool NetPacketProc_MoveStop(st_SESSION *pSession, CPacket *pPacket) {

	if (pSession == NULL) {
		// 큰에러임
		printf("# MoveStop  # Session is NULL!!!\n");
	}

	BYTE byDirection = 0;
	short shX = 0;
	short shY = 0;

	*pPacket >> byDirection;
	*pPacket >> shX;
	*pPacket >> shY;

	//printf("# MoveStop  # Session ID : %d, Direction : %d, X : %d, Y : %d\n",
	//	pSession->dwSessionID, byDirection, shX, shY);

	// ID 로 캐릭터 검색
	st_CHARACTER *pCharacter = FindCharacter(pSession->dwSessionID);
	if (pCharacter == NULL) {

		/*CLogMaker::GetInstance()->Log(dfLOG_LEVEL_ERROR,
			"# MoveStart > SessionID : %d Character Not Found!",
			pSession->dwSessionID);*/
		printf("# MoveStop  > SessionID : %d Character Not Found!\n",
			pSession->dwSessionID);

		return false;
	}

	if (abs(pCharacter->shX - shX) > dfERROR_RANGE ||
		abs(pCharacter->shY - shY) > dfERROR_RANGE) {

		int idrX;
		int idrY;

		int iDeadFrame = DeadReckoningPos(pCharacter->dwAction,
			pCharacter->dwActionTick,
			pCharacter->shActionX,
			pCharacter->shActionY,
			&idrX, &idrY);

		if (abs(idrX - shX) > dfERROR_RANGE ||
			abs(idrY - shY) > dfERROR_RANGE) {

			mpSync(pPacket, pCharacter->dwSessionID, idrX, idrY);
			SendPacket_Around(pCharacter->pSession, pPacket, true);
			// SendPacket_Broadcast(pPacket);
			printf("# MoveStop  # Sync!!!! SessionID : %d\n", pCharacter->dwSessionID);
			printf("# MoveStop  # Sync!!!! SessionID : %d\n", pCharacter->dwSessionID);
		}
		shX = idrX;
		shY = idrY;
	}

	// 캐릭터 정보 변경
	pCharacter->dwAction = dfPACKET_MOVE_STAND;

	switch (byDirection) {

	case dfPACKET_MOVE_DIR_RR:
	case dfPACKET_MOVE_DIR_RU:
	case dfPACKET_MOVE_DIR_RD:
		pCharacter->byDirection = dfPACKET_MOVE_DIR_RR;
		break;
	case dfPACKET_MOVE_DIR_LL:
	case dfPACKET_MOVE_DIR_LU:
	case dfPACKET_MOVE_DIR_LD:
		pCharacter->byDirection = dfPACKET_MOVE_DIR_LL;
		break;
	}
	pCharacter->byMoveDirection = byDirection;

	// 실제 좌표 변경( shX, shY 는 이미 데드레커닝 작업 후)
	pCharacter->shX = shX;
	pCharacter->shY = shY;

	if (SectorUpdateCharacter(pCharacter)) {

		CharacterSectorUpdatePacket(pCharacter);
	}

	// 데드레커닝을 위한 정보 저장
	pCharacter->dwActionTick = timeGetTime();
	pCharacter->shActionX = pCharacter->shX;
	pCharacter->shActionY = pCharacter->shY;

	// mpMoveStop
	mpMoveStop(pPacket, pSession->dwSessionID, pCharacter->byDirection, pCharacter->shX, pCharacter->shY);
	
	SendPacket_Around(pSession, pPacket);
	//SendPacket_Broadcast(pPacket);

	return true;
}

// 충돌 체크, 맞는 적이 있다면 맞는 적 ptr 리턴, 없다면 NULL 리턴
st_CHARACTER *Collision(st_CHARACTER *pAttacker, int iHitRangeX, int iHitRangeY) {

	// 공격자 캐릭터가 있는지 에러체크
	if (pAttacker == NULL) {

		printf("Attacker Not Found! Error\n");
		return NULL;
	}

	// 현 섹터 X Y 픽셀 범위 알기
	int iSectorX = pAttacker->CurSector.iX;
	int iSectorY = pAttacker->CurSector.iY;

	int iSectorStartX = iSectorX * dfSECTOR_PIXEL_WIDTH;
	int iSectorEndX = iSectorStartX + dfSECTOR_PIXEL_WIDTH;
	int iSectorStartY = iSectorY * dfSECTOR_PIXEL_HEIGHT;
	int iSectorEndY = iSectorStartY + dfSECTOR_PIXEL_HEIGHT;

	// 공격자 기준, 공격 픽셀 범위 알기
	short shRangeXStart = 0;
	short shRangeXEnd = 0;
	short shRangeYStart = 0;
	short shRangeYEnd = 0;

	// 검색해야할 섹터 찾기위한 충돌 체크 BYTE
	BYTE byCollisionFlag = 0x00;

	// 검색할 섹터 좌표 순차 저장한 Sector배열 및 초기화 
	// (섹터가 적절할 크기일 때 최대 4개, 섹터가 너무 좁을 때 최대 9개 Sector 검색)
	st_SECTOR_POS stSectorCollision[9] = { {-1, -1}, };

	// stSectorCollision의 Index
	int iIndex = 0;

	// 내 캐릭터의 공격 방향에 따라 검색해야될 섹터 설정 (LL)
	if (pAttacker->byDirection == dfPACKET_MOVE_DIR_LL) {

		// pAttacker의 공격 범위
		shRangeXStart = pAttacker->shX - iHitRangeX;
		shRangeXEnd = pAttacker->shX;
		shRangeYStart = pAttacker->shY - (iHitRangeY / 3) * 2;
		shRangeYEnd = pAttacker->shY + iHitRangeY / 3;

		if (shRangeXStart < dfRANGE_MOVE_LEFT)
			shRangeXStart = 0;
		if (shRangeYStart < dfRANGE_MOVE_TOP)
			shRangeYStart = 0;
		if (shRangeYEnd >= dfRANGE_MOVE_BOTTOM)
			shRangeYEnd = dfRANGE_MOVE_BOTTOM - 1;
	}

	// 내 캐릭터의 공격 방향에 따라 검색해야될 섹터 설정 (RR)
	else if (pAttacker->byDirection == dfPACKET_MOVE_DIR_RR) {

		shRangeXStart = pAttacker->shX;
		shRangeXEnd = pAttacker->shX + iHitRangeX;
		shRangeYStart = pAttacker->shY - (iHitRangeY / 3) * 2;
		shRangeYEnd = pAttacker->shY + iHitRangeY / 3;

		if (shRangeXEnd >= dfRANGE_MOVE_RIGHT)
			shRangeXEnd = dfRANGE_MOVE_RIGHT - 1;
		if (shRangeYStart < dfRANGE_MOVE_TOP)
			shRangeYStart = 0;
		if (shRangeYEnd >= dfRANGE_MOVE_BOTTOM)
			shRangeYEnd = dfRANGE_MOVE_BOTTOM - 1;
	}

	// 8방 섹터 중, 체크해 볼 섹터 정리
	if (shRangeXStart < iSectorStartX)
		byCollisionFlag |= dfCOLLISION_LL;

	if (shRangeXEnd > iSectorEndX)
		byCollisionFlag |= dfCOLLISION_RR;

	if (shRangeYStart < iSectorStartY)
		byCollisionFlag |= dfCOLLISION_UU;

	if (shRangeYEnd > iSectorEndY)
		byCollisionFlag |= dfCOLLISION_DD;

	if ((byCollisionFlag & dfCOLLISION_LL) && (byCollisionFlag & dfCOLLISION_UU))
		byCollisionFlag |= dfCOLLISION_LU;

	if ((byCollisionFlag & dfCOLLISION_RR) && (byCollisionFlag & dfCOLLISION_UU))
		byCollisionFlag |= dfCOLLISION_RU;

	if ((byCollisionFlag & dfCOLLISION_LL) && (byCollisionFlag & dfCOLLISION_DD))
		byCollisionFlag |= dfCOLLISION_LD;

	if ((byCollisionFlag & dfCOLLISION_RR) && (byCollisionFlag & dfCOLLISION_DD))
		byCollisionFlag |= dfCOLLISION_RD;

	// byCollisionFlag 값에 따라 체크할 섹터 좌표 배열에 넣기
	// 자신의 섹터 먼저 넣기 (검색 0순위)
	stSectorCollision[iIndex].iX = iSectorX;
	stSectorCollision[iIndex].iY = iSectorY;
	iIndex++;

	// 좌우 POS 넣기 (검색 1순위)
	bool isOkLLSector = iSectorX - 1 >= 0;
	if ((byCollisionFlag & dfCOLLISION_LL) && isOkLLSector) {
		stSectorCollision[iIndex].iX = iSectorX - 1;
		stSectorCollision[iIndex].iY = iSectorY;
		iIndex++;
	}
	bool isOkRRSector = iSectorX + 1 < dfSECTOR_MAX_X;
	if ((byCollisionFlag & dfCOLLISION_RR) && isOkRRSector) {
		stSectorCollision[iIndex].iX = iSectorX + 1;
		stSectorCollision[iIndex].iY = iSectorY;
		iIndex++;
	}

	// 상하 POS넣기 (검색 2순위)
	bool isOkUUSector = iSectorY - 1 >= 0;
	if ((byCollisionFlag & dfCOLLISION_UU) && isOkUUSector) {
		stSectorCollision[iIndex].iX = iSectorX;
		stSectorCollision[iIndex].iY = iSectorY - 1;
		iIndex++;
	}
	bool isOkDDSector = iSectorY + 1 < dfSECTOR_MAX_Y;
	if ((byCollisionFlag & dfCOLLISION_RR) && isOkDDSector) {
		stSectorCollision[iIndex].iX = iSectorX;
		stSectorCollision[iIndex].iY = iSectorY + 1;
		iIndex++;
	}

	// 대각선 POS넣기 (검색 3순위)
	// LU Check
	if ((byCollisionFlag & dfCOLLISION_LU) && (isOkLLSector & isOkUUSector)) {
		stSectorCollision[iIndex].iX = iSectorX - 1;
		stSectorCollision[iIndex].iY = iSectorY - 1;
		iIndex++;
	}
	// RU Check
	if ((byCollisionFlag & dfCOLLISION_RU) && (isOkRRSector & isOkUUSector)) {
		stSectorCollision[iIndex].iX = iSectorX + 1;
		stSectorCollision[iIndex].iY = iSectorY - 1;
		iIndex++;
	}
	// LD Check
	if ((byCollisionFlag & dfCOLLISION_LD) && (isOkLLSector & isOkDDSector)) {
		stSectorCollision[iIndex].iX = iSectorX - 1;
		stSectorCollision[iIndex].iY = iSectorY + 1;
		iIndex++;
	}
	// RD Check
	if ((byCollisionFlag & dfCOLLISION_RD) && (isOkRRSector & isOkDDSector)) {
		stSectorCollision[iIndex].iX = iSectorX + 1;
		stSectorCollision[iIndex].iY = iSectorY + 1;
		iIndex++;
	}

	int iEndIndex = iIndex;
	// stSectorCollision에 넣은 Pos 토대로 iter 돌면서 pDamaged 리턴
	for (iIndex = 0; iIndex < iEndIndex; iIndex++) {

		int iX = stSectorCollision[iIndex].iX;
		int iY = stSectorCollision[iIndex].iY;

		list<st_CHARACTER *> *pSector = &g_Sector[iY][iX];
		list<st_CHARACTER *>::iterator CharacterIter = pSector->begin();
		list<st_CHARACTER *>::iterator CharacterIterEnd = pSector->end();

		for (; CharacterIter != CharacterIterEnd; ++CharacterIter) {

			st_CHARACTER *pDamaged = *CharacterIter;

			if (pDamaged->shX >= shRangeXStart && pDamaged->shX <= shRangeXEnd) {

				if (pDamaged->shY >= shRangeYStart && pDamaged->shY <= shRangeYEnd) {

					if (pDamaged != pAttacker)
						return pDamaged;
				}
			}
		}
	}

	return NULL;
}

// NetProcAttack1
bool NetPacketProc_Attack1(st_SESSION *pSession, CPacket *pPacket) {
	
	if (pSession == NULL) {
		// 큰에러임
		printf("# Attack_1 # Session is NULL!!!\n");
	}

	BYTE byDirection = 0;
	short shX = 0;
	short shY = 0;

	*pPacket >> byDirection;
	*pPacket >> shX;
	*pPacket >> shY;

	// Attacker(Character) 찾아서 충돌처리
	st_CHARACTER *pCharacter = FindCharacter(pSession->dwSessionID);
	st_CHARACTER *pDamaged = NULL;

	// DeadReckoning 처리
	if (abs(pCharacter->shX - shX) > dfERROR_RANGE ||
		abs(pCharacter->shY - shY) > dfERROR_RANGE) {

		int idrX;
		int idrY;

		int iDeadFrame = DeadReckoningPos(pCharacter->dwAction,
			pCharacter->dwActionTick,
			pCharacter->shActionX,
			pCharacter->shActionY,
			&idrX, &idrY);

		if (abs(idrX - shX) > dfERROR_RANGE ||
			abs(idrY - shY) > dfERROR_RANGE) {

			mpSync(pPacket, pCharacter->dwSessionID, idrX, idrY);
			SendPacket_Around(pCharacter->pSession, pPacket, true);
			// SendPacket_Broadcast(pPacket);
			printf("# Attack_1  # Sync!!!! SessionID : %d\n", pCharacter->dwSessionID);
			printf("# Attack_1  # Sync!!!! SessionID : %d\n", pCharacter->dwSessionID);
		}
		shX = idrX;
		shY = idrY;
	}

	// 캐릭터 정보 변경
	pCharacter->dwAction = dfPACKET_MOVE_STAND;

	switch (byDirection) {

	case dfPACKET_MOVE_DIR_RR:
	case dfPACKET_MOVE_DIR_RU:
	case dfPACKET_MOVE_DIR_RD:
		pCharacter->byDirection = dfPACKET_MOVE_DIR_RR;
		break;
	case dfPACKET_MOVE_DIR_LL:
	case dfPACKET_MOVE_DIR_LU:
	case dfPACKET_MOVE_DIR_LD:
		pCharacter->byDirection = dfPACKET_MOVE_DIR_LL;
		break;
	}
	pCharacter->byMoveDirection = byDirection;

	// 실제 좌표 변경( shX, shY 는 이미 데드레커닝 작업 후)
	pCharacter->shX = shX;
	pCharacter->shY = shY;

	// 공격 모션 최대 9개 섹터에 반영 ( 자신 제외 )
	mpAttack1(pPacket, 
		pSession->dwSessionID, 
		pCharacter->byDirection, 
		pCharacter->shX, pCharacter->shY);

	SendPacket_Around(pSession, pPacket);

	// 데미지 정보는 최대 9개 섹터에 반영 ( 자신 포함 )
	pDamaged = Collision(pCharacter, dfATTACK1_RANGE_X, dfATTACK1_RANGE_Y);

	if (pDamaged == NULL) {

		//printf("ID :%d - Attack1 Miss\n", pCharacter->dwSessionID);
		return true;
	}

	// hp 줄이기
	pDamaged->chHP -= dfATTACK1_DAMAGE * 2;

	// DamagePacket
	mpDamage(pPacket, pSession->dwSessionID, pDamaged->dwSessionID, pDamaged->chHP);
	
	SendPacket_Around(pSession, pPacket, true);

	return true;
}

// NetProcAttack2
bool NetPacketProc_Attack2(st_SESSION *pSession, CPacket *pPacket) {

	if (pSession == NULL) {
		// 큰에러임
		printf("# Attack_2 # Session is NULL!!!\n");
	}

	BYTE byDirection = 0;
	short shX = 0;
	short shY = 0;

	*pPacket >> byDirection;
	*pPacket >> shX;
	*pPacket >> shY;

	// Attacker(Character) 찾아서 충돌처리
	st_CHARACTER *pCharacter = FindCharacter(pSession->dwSessionID);
	st_CHARACTER *pDamaged = NULL;

	// DeadReckoning 처리
	if (abs(pCharacter->shX - shX) > dfERROR_RANGE ||
		abs(pCharacter->shY - shY) > dfERROR_RANGE) {

		int idrX;
		int idrY;

		int iDeadFrame = DeadReckoningPos(pCharacter->dwAction,
			pCharacter->dwActionTick,
			pCharacter->shActionX,
			pCharacter->shActionY,
			&idrX, &idrY);

		if (abs(idrX - shX) > dfERROR_RANGE ||
			abs(idrY - shY) > dfERROR_RANGE) {

			mpSync(pPacket, pCharacter->dwSessionID, idrX, idrY);
			SendPacket_Around(pCharacter->pSession, pPacket, true);
			// SendPacket_Broadcast(pPacket);
			printf("# Attack_2  # Sync!!!! SessionID : %d\n", pCharacter->dwSessionID);
			printf("# Attack_2  # Sync!!!! SessionID : %d\n", pCharacter->dwSessionID);
		}
		shX = idrX;
		shY = idrY;
	}

	// 캐릭터 정보 변경
	pCharacter->dwAction = dfPACKET_MOVE_STAND;

	switch (byDirection) {

	case dfPACKET_MOVE_DIR_RR:
	case dfPACKET_MOVE_DIR_RU:
	case dfPACKET_MOVE_DIR_RD:
		pCharacter->byDirection = dfPACKET_MOVE_DIR_RR;
		break;
	case dfPACKET_MOVE_DIR_LL:
	case dfPACKET_MOVE_DIR_LU:
	case dfPACKET_MOVE_DIR_LD:
		pCharacter->byDirection = dfPACKET_MOVE_DIR_LL;
		break;
	}
	pCharacter->byMoveDirection = byDirection;

	// 실제 좌표 변경( shX, shY 는 이미 데드레커닝 작업 후)
	pCharacter->shX = shX;
	pCharacter->shY = shY;

	// 공격 모션 최대 9개 섹터에 반영 ( 자신 제외 )
	mpAttack2(pPacket,
		pSession->dwSessionID,
		pCharacter->byDirection,
		pCharacter->shX, pCharacter->shY);

	SendPacket_Around(pSession, pPacket);

	// 데미지 정보는 최대 9개 섹터에 반영 ( 자신 포함 )
	pDamaged = Collision(pCharacter, dfATTACK2_RANGE_X, dfATTACK2_RANGE_Y);

	if (pDamaged == NULL) {

		//printf("ID :%d - Attack2 Miss\n", pCharacter->dwSessionID);
		return true;
	}

	// hp 줄이기
	pDamaged->chHP -= dfATTACK2_DAMAGE * 2;

	// DamagePacket
	mpDamage(pPacket, pSession->dwSessionID, pDamaged->dwSessionID, pDamaged->chHP);

	SendPacket_Around(pDamaged->pSession, pPacket, true);

	return true;
}

// NetProcAttack3
bool NetPacketProc_Attack3(st_SESSION *pSession, CPacket *pPacket) {
	
	if (pSession == NULL) {
		// 큰에러임
		printf("# Attack_3 # Session is NULL!!!\n");
	}

	BYTE byDirection = 0;
	short shX = 0;
	short shY = 0;

	*pPacket >> byDirection;
	*pPacket >> shX;
	*pPacket >> shY;

	// Attacker(Character) 찾아서 충돌처리
	st_CHARACTER *pCharacter = FindCharacter(pSession->dwSessionID);
	st_CHARACTER *pDamaged = NULL;

	// DeadReckoning 처리
	if (abs(pCharacter->shX - shX) > dfERROR_RANGE ||
		abs(pCharacter->shY - shY) > dfERROR_RANGE) {

		int idrX;
		int idrY;

		int iDeadFrame = DeadReckoningPos(pCharacter->dwAction,
			pCharacter->dwActionTick,
			pCharacter->shActionX,
			pCharacter->shActionY,
			&idrX, &idrY);

		if (abs(idrX - shX) > dfERROR_RANGE ||
			abs(idrY - shY) > dfERROR_RANGE) {

			mpSync(pPacket, pCharacter->dwSessionID, idrX, idrY);
			SendPacket_Around(pCharacter->pSession, pPacket, true);
			// SendPacket_Broadcast(pPacket);
			printf("# Attack_3  # Sync!!!! SessionID : %d\n", pCharacter->dwSessionID);
			printf("# Attack_3  # Sync!!!! SessionID : %d\n", pCharacter->dwSessionID);
		}
		shX = idrX;
		shY = idrY;
	}

	// 캐릭터 정보 변경
	pCharacter->dwAction = dfPACKET_MOVE_STAND;

	switch (byDirection) {

	case dfPACKET_MOVE_DIR_RR:
	case dfPACKET_MOVE_DIR_RU:
	case dfPACKET_MOVE_DIR_RD:
		pCharacter->byDirection = dfPACKET_MOVE_DIR_RR;
		break;
	case dfPACKET_MOVE_DIR_LL:
	case dfPACKET_MOVE_DIR_LU:
	case dfPACKET_MOVE_DIR_LD:
		pCharacter->byDirection = dfPACKET_MOVE_DIR_LL;
		break;
	}
	pCharacter->byMoveDirection = byDirection;

	// 실제 좌표 변경( shX, shY 는 이미 데드레커닝 작업 후)
	pCharacter->shX = shX;
	pCharacter->shY = shY;

	// 공격 모션 최대 9개 섹터에 반영 ( 자신 제외 )
	mpAttack3(pPacket,
		pSession->dwSessionID,
		pCharacter->byDirection,
		pCharacter->shX, pCharacter->shY);

	SendPacket_Around(pSession, pPacket);

	// 데미지 정보는 최대 9개 섹터에 반영 ( 자신 포함 )
	pDamaged = Collision(pCharacter, dfATTACK3_RANGE_X, dfATTACK3_RANGE_Y);

	if (pDamaged == NULL) {

		//printf("ID :%d - Attack3 Miss\n", pCharacter->dwSessionID);
		return true;
	}

	// hp 줄이기
	pDamaged->chHP -= dfATTACK3_DAMAGE * 2;

	// DamagePacket
	mpDamage(pPacket, pSession->dwSessionID, pDamaged->dwSessionID, pDamaged->chHP);

	SendPacket_Around(pSession, pPacket, true);

	return true;
}

bool NetPacketProc_Echo(st_SESSION *pSession, CPacket *pPacket) {

	if (pSession == NULL) {
		// 큰에러임
		printf("# Echo # Session is NULL!!!\n");
	}
	
	// mpEcho
	mpEcho(pPacket);

	SendPacket_Unicast(pSession, pPacket);

	return true;
}

// PacketProc
bool PacketProc(st_SESSION *pSession, BYTE byPacketType, CPacket *pPacket) {

	switch (byPacketType) {

	case dfPACKET_CS_MOVE_START:
		return NetPakcetProc_MoveStart(pSession, pPacket);
		break;
	case dfPACKET_CS_MOVE_STOP:
		return NetPacketProc_MoveStop(pSession, pPacket);
		break;
	case dfPACKET_CS_ATTACK1:
		return NetPacketProc_Attack1(pSession, pPacket);
		break;
	case dfPACKET_CS_ATTACK2:
		return NetPacketProc_Attack2(pSession, pPacket);
		break;
	case dfPACKET_CS_ATTACK3:
		return NetPacketProc_Attack3(pSession, pPacket);
		break;
	case dfPACKET_CS_ECHO:
		return NetPacketProc_Echo(pSession, pPacket);
	default :
		CLogMaker::GetInstance()->Log(dfLOG_LEVEL_DEBUG,"PacketType Error : %d", byPacketType);
		break;
	}

	return true;
}

// 네트워크 받은 패킷 처리
int ComplateRecvPacket(st_SESSION *pSession) {

	st_PACKET_HEADER	stHeader;
	
	int		iRecvQSize;

	iRecvQSize = pSession->RecvQ.GetUseSize();

	if (iRecvQSize < sizeof(stHeader))
		return 1;

	pSession->RecvQ.Peek((char *)&stHeader, sizeof(st_PACKET_HEADER));
	if (stHeader.byCode != dfNETWORK_PACKET_CODE)
		return -1;

	// +1 = EndCode 크기
	if (iRecvQSize < stHeader.bySize + sizeof(st_PACKET_HEADER) + 1)
		return 1;

	pSession->RecvQ.MoveFront(sizeof(st_PACKET_HEADER));


	CPacket Packet;
	BYTE	byEndCode;
	if (!pSession->RecvQ.Dequeue(Packet.GetBufferPtr(), stHeader.bySize))
		return -1;

	if (!pSession->RecvQ.Dequeue((char *)&byEndCode, 1))
		return -1;
	if (byEndCode != dfNETWORK_PACKET_END)
		return -1;

	Packet.MoveWritePos(stHeader.bySize);

	if (!PacketProc(pSession, stHeader.byType, &Packet))
		return -1;

	return 0;
}

// 네트워크 Send 처리
bool NetProcSend(SOCKET Socket) {

	// 클라 접속 종료시 return false;

	st_SESSION *pSession;
	int iBuffSize;
	int iResult;

	pSession = FindSession(Socket);

	if (pSession == NULL) {

		//CLogMaker::GetInstance()->Log(dfLOG_LEVEL_ERROR, "NetProcSend Error / Session NULL");
		printf("NetProcSend Error / Session NULL\n");
		return false;
	}

	// SendQ가 가득 찼다면 이상한 상황 Disconnect
	int iUseSize = pSession->SendQ.GetUseSize();
	if (iUseSize >= pSession->SendQ.GetBufferSize() - 1) {

		//CLogMaker::GetInstance()->Log(dfLOG_LEVEL_ERROR, "NetProcSend Error / Full Use Size");
		printf("NetProcSend Error / Full Use Size\n");
		Disconnect(pSession->Socket);
		return false;
	}

	iBuffSize = pSession->SendQ.GetDeqSize();
	//char buf[20] = { 0, };
	//pSession->SendQ.Peek((char *)buf, iBuffSize);
	iResult = send(pSession->Socket, pSession->SendQ.GetFrontBufferPtr(), iBuffSize, 0);
	if (iResult == SOCKET_ERROR || iResult == 0) {

		//CLogMaker::GetInstance()->Log(dfLOG_LEVEL_ERROR, "NetProcSend Error / send Error");
		printf("NetProcSend Error / Full Use Size\n");
		Disconnect(pSession->Socket);
		return false;
	}

	if (iResult > 0)
		pSession->SendQ.MoveFront(iResult);

	return true;
}

// 네트워크 Accept 처리
void NetProcAccept() {

	st_SESSION *pSession;
	st_CHARACTER *pCharacter;
	st_SECTOR_AROUND SectorAround;
	CPacket Packet;

	// accept()
	int AddrLen = sizeof(SOCKADDR_IN);
	SOCKADDR_IN ClientAddr;
	SOCKET ClientSock = accept(g_ListenSock, (SOCKADDR *)&ClientAddr, &AddrLen);
	if (ClientSock == INVALID_SOCKET) {
		printf("accept()_ERROR\n");
		return;
	}

	// 새 세션 생성
	pSession = CreateSession(ClientSock);

	// 새 캐릭터 생성
	pCharacter = CreateCharacter(pSession);

	int iSectorX = pCharacter->CurSector.iX;
	int iSectorY = pCharacter->CurSector.iY;
	GetSectorAround(iSectorX, iSectorY, &SectorAround);

	// 1. 내 정보 내게 생성
	// 2. 섹터 내의 플레이어들의 정보 내게 생성
	// 3. 섹터 내의 플레이어들에게 내 정보 생성 (Other)
	for (int iCnt = 0; iCnt < SectorAround.iCount; iCnt++) {

		int iX = SectorAround.Around[iCnt].iX;
		int iY = SectorAround.Around[iCnt].iY;
		list<st_CHARACTER *> *CharacterInSector = &g_Sector[iY][iX];

		list<st_CHARACTER *>::iterator CharacterIter = CharacterInSector->begin();
		list<st_CHARACTER *>::iterator CharacterIterEnd = CharacterInSector->end();

		//int iSize = CharacterInSector->size();

		for (; CharacterIter != CharacterIterEnd; ++CharacterIter) {

			st_CHARACTER *pCharacterOther = *CharacterIter;
			if (pCharacterOther != pCharacter) {

				mpCreateOtherCharacter(&Packet,
					pCharacter->dwSessionID,
					pCharacter->byDirection,
					pCharacter->shX,
					pCharacter->shY,
					pCharacter->chHP);

				pCharacterOther->pSession->SendQ.Enqueue(Packet.GetReadPtr(), Packet.GetPacketSize());
			
				mpCreateOtherCharacter(&Packet,
					pCharacterOther->dwSessionID,
					pCharacterOther->byDirection,
					pCharacterOther->shX,
					pCharacterOther->shY,
					pCharacterOther->chHP);

				pCharacter->pSession->SendQ.Enqueue(Packet.GetReadPtr(), Packet.GetPacketSize());
			}

			else if(pCharacterOther == pCharacter) {

				mpCreateCharacter(&Packet,
					pCharacter->dwSessionID,
					pCharacter->byDirection,
					pCharacter->shX,
					pCharacter->shY,
					pCharacter->chHP);

				pCharacter->pSession->SendQ.Enqueue(Packet.GetReadPtr(), Packet.GetPacketSize());
			}
		}
	}

	// 예전 버전 (섹터 없을 때)

	// 나에게, 주변 섹터 및 내 섹터 내의 다른 캐릭터 생성 메시지 송신
	// 내 캐릭터 패킷 보내기
	//mpCreateCharacter(&Packet,
	//	pCharacter->dwSessionID,
	//	pCharacter->byDirection,
	//	pCharacter->shX,
	//	pCharacter->shY,
	//	pCharacter->chHP);
	//
	//// 새로 만들어지는 캐릭터를 모든 클라에게 보내기 (나중엔 섹터?)
	//pSession->SendQ.Enqueue(Packet.GetReadPtr(), Packet.GetPacketSize());
	//int a = 0;
	//
	// 캐릭터 보내기(지금은 broadcast 나중에는 Sector)
	//map<DWORD, st_CHARACTER *>::iterator iter = g_CharacterMap.begin();
	//map<DWORD, st_CHARACTER *>::iterator iterEnd = g_CharacterMap.end();
	//
	//st_CHARACTER *pCharacterNow;
	//for (; iter != iterEnd; ++iter) {
	//
	//	pCharacterNow = iter->second;
	//	// 지금 생성된 캐릭터가 아니라면
	//	if (pCharacterNow != pCharacter) {
	//
	//		mpCreateOtherCharacter(&Packet,
	//			pCharacter->dwSessionID,
	//			pCharacter->byDirection,
	//			pCharacter->shX,
	//			pCharacter->shY,
	//			pCharacter->chHP);
	//
	//		pCharacterNow->pSession->SendQ.Enqueue(Packet.GetReadPtr(), Packet.GetPacketSize());
	//	
	//		mpCreateOtherCharacter(&Packet,
	//			pCharacterNow->dwSessionID,
	//			pCharacterNow->byDirection,
	//			pCharacterNow->shX,
	//			pCharacterNow->shY,
	//			pCharacterNow->chHP);
	//
	//		pSession->SendQ.Enqueue(Packet.GetReadPtr(), Packet.GetPacketSize());
	//	}
	//
	//	else {
	//
	//		mpCreateCharacter(&Packet,
	//			pCharacter->dwSessionID,
	//			pCharacter->byDirection,
	//			pCharacter->shX,
	//			pCharacter->shY,
	//			pCharacter->chHP);
	//
	//		pSession->SendQ.Enqueue(Packet.GetReadPtr(), Packet.GetPacketSize());
	//	}
	//}
}

// 네트워크 Recv 처리
void NetProcRecv(SOCKET Socket) {

	st_SESSION *pSession;
	int iBuffSize;
	int iResult;

	pSession = FindSession(Socket);

	if (pSession == NULL) {

		//CLogMaker::GetInstance()->Log(0,"NetProcRecv Error / Session NULL");
		printf("NetProcRecv Error / Session NULL\n");
		return;
	}

	// RecvQ에 빈공간이 없다면, 비정상 클라이므로 종료
	int iFreeSize = pSession->RecvQ.GetFreeSize();
	if (iFreeSize <= 0) {
		
		//CLogMaker::GetInstance()->Log(dfLOG_LEVEL_ERROR, "NetProcSend Error / 0 FREE Size");
		printf("NetProcSend Error / 0 FREE Size\n");
		Disconnect(pSession->Socket);
		return;
	}

	// 마지막으로 받은 메시지 타임
	pSession->dwTrafficTick = timeGetTime();

	// 받기작업
	iBuffSize = pSession->RecvQ.GetEnqSize();
	iResult = recv(pSession->Socket, pSession->RecvQ.GetRearBufferPtr(), iBuffSize, 0);
	if (iResult == SOCKET_ERROR || iResult == 0) {

		//CLogMaker::GetInstance()->Log(dfLOG_LEVEL_ERROR, "NetProcSend Error / recv Error");
		printf("NetProcRecv Error / recv Error\n");
		Disconnect(pSession->Socket);
		return;
	}

	// 받은 데이터가 있음
	if (iResult > 0) {

		// RecvQ에 반영
		pSession->RecvQ.MoveRear(iResult);

		// RecvQ에 있는 모든 패킷 처리
		for (;;) {

			iResult = ComplateRecvPacket(pSession);
			if (iResult == 1)
				break;
			if (iResult == -1) {

				printf("NetProcRecv Error SessionID : %d", pSession->dwSessionID);
				return;
			}
		}
	}
}

// 네트워크 Select 처리
void NetSelectSocket(SOCKET *pTableSocket, FD_SET *pReadSet, FD_SET *pWriteSet) {

	timeval Time;
	
	int iResult = 0;
	int iCnt = 0;
	bool bProcFlag = false;

	Time.tv_sec = 0;
	Time.tv_usec = 0;

	iResult = select(0, pReadSet, pWriteSet, 0, &Time);
	if (iResult == SOCKET_ERROR) {

		wprintf(L"Select() Error\n");
	}

	else if (iResult > 0) {

		for (iCnt = 0; iCnt < FD_SETSIZE && iResult > 0; ++iCnt) {

			bProcFlag = true;
			if (pTableSocket[iCnt] == INVALID_SOCKET)
				continue;

			if (FD_ISSET(pTableSocket[iCnt], pWriteSet)) {

				--iResult;
				bProcFlag = NetProcSend(pTableSocket[iCnt]);
			}

			if (FD_ISSET(pTableSocket[iCnt], pReadSet)) {

				--iResult;
				
				// 해당클라가 접속 종료한 경우가 있으므로 확인하고 진행
				if (bProcFlag) {

					if (pTableSocket[iCnt] == g_ListenSock) {

						NetProcAccept();
					}
					else if (pTableSocket[iCnt] != g_ListenSock) {

						NetProcRecv(pTableSocket[iCnt]);
					}
				}
			}
		}
	}
}

// 네트워크 송수신 처리
void NetIOProc(void) {

	st_SESSION *pSession;

	SOCKET	UserTableSOCKET[FD_SETSIZE] = { INVALID_SOCKET, };
	int			iSocketCount = 0;

	FD_SET ReadSet;
	FD_SET WriteSet;

	FD_ZERO(&ReadSet);
	FD_ZERO(&WriteSet);

	FD_SET(g_ListenSock, &ReadSet);
	UserTableSOCKET[iSocketCount] = g_ListenSock;

	iSocketCount++;

	map<SOCKET, SESSION *>::iterator SessionIter = g_SessionMap.begin();
	map<SOCKET, SESSION *>::iterator SessionIterEnd = g_SessionMap.end();
	for (; SessionIter != SessionIterEnd; ) {

		pSession = SessionIter->second;

		//해당 클라 ReadSet 등록
		UserTableSOCKET[iSocketCount] = pSession->Socket;
		
		FD_SET(pSession->Socket, &ReadSet);

		// 해당 클라 SendQ에 데이터 있다면 WriteSet 등록
		if (pSession->SendQ.GetUseSize() > 0)
			FD_SET(pSession->Socket, &WriteSet);

		iSocketCount++;
		SessionIter++;

		// Select 최대치 도달시, Select 호출 후 정리
		if (FD_SETSIZE <= iSocketCount) {

			NetSelectSocket(UserTableSOCKET, &ReadSet, &WriteSet);

			FD_ZERO(&ReadSet);
			FD_ZERO(&WriteSet);

			//memset(UserTableSOCKET, INVALID_SOCKET, sizeof(SOCKET) * FD_SETSIZE);
			memset(UserTableSOCKET, 0, sizeof(SOCKET) * FD_SETSIZE);

			FD_SET(g_ListenSock, &ReadSet);
			UserTableSOCKET[0] = g_ListenSock;
			iSocketCount = 1;
		}
	}

	// 64개가 안됐지만 0개 이상일때 Select 호출 후 정리
	if (iSocketCount > 0) {
		NetSelectSocket(UserTableSOCKET, &ReadSet, &WriteSet);
	}
}

#endif


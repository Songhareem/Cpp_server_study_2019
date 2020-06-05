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
// ����ü ����
//============================

// ���� ���� ��ǥ ����ü
typedef struct st_SECTOR_POS {

	int iX = -1;
	int iY = -1;
}SECTOR_POS;

// Ư�� ��ġ �ֺ� 9���� ���� ���� ����ü
typedef struct st_SECTOR_AROUND {

	int				iCount;	// ����ϴ� ���� ����
	st_SECTOR_POS	Around[9];
}SECTOR_AROUND;

// �������� ����ü
typedef struct st_SESSION {

	SOCKET		Socket;
	DWORD		dwSessionID;
	CRingBuffer	SendQ;
	CRingBuffer	RecvQ;
	DWORD		dwTrafficTick;
	DWORD		dwTrafficCount;
	DWORD		dwTrafficPerSecTick;

}SESSION;

// ĳ���� ���� ����ü
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
// �������� ����
//============================

//----------------------------------------
// �������� ����
//----------------------------------------

// ĳ���� map
map<DWORD, CHARACTER *>g_CharacterMap;

// ����� ĳ���� ���� list
list<st_CHARACTER *> g_Sector[dfSECTOR_MAX_Y][dfSECTOR_MAX_X];

//----------------------------------------
// ����� ����
//----------------------------------------

// �� ���� ���� ���� ���� ���� ���� ����
bool g_bShutdown = false;

// �� ���� ���� Ű���� ��� �� ��� ����
bool g_bControlMode = false;

// FPS ǥ��
INT64 g_TotalFps = 0;
DWORD g_MaxFps = 0;
DWORD g_MinFps = 0xffffffff;

//----------------------------------------
// ��Ʈ��ũ�� ����
//----------------------------------------

// ���� ����
SOCKET g_ListenSock;

// ���� Addr
SOCKADDR_IN g_ServerAddr;

// WSA
WSADATA g_wsa;

// ���� ���� ID
int g_iSessionID = 0;

// ���� ����ü ���� map
map<SOCKET, SESSION *>g_SessionMap;

//============================
// �Լ� ����
//============================

//----------------------------
// �Լ� ��� ����
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
// ����� �Լ�
//----------------------------

// ���� �� ���� ������, DB ������ �ε�
void LoadData(void) {

	// �α� ���� �ʱ�ȭ
	CLogMaker::GetInstance()->MakeLogFile("Log");
}

// Ű���� �Է��� ���� ������ ����
void ServerControl(void) {

	// D : ��Ʈ�� Lock / A : ��Ʈ�� Unlock / S : ���� ���� / L : �α� �����
	
	// A : ��Ʈ�� ��� Ȱ��ȭ
	if (GetAsyncKeyState(0x41) & 0x0001) {

		wprintf(L"Activate Console Mode\n");
		wprintf(L"S : Shutdown Server\n");
		g_bControlMode = true;
	}

	// D : ��Ʈ�� ��� ��Ȱ��ȭ
	if (GetAsyncKeyState(0x44) & 0x0001) {
		wprintf(L"Deactivate Console Mode\n");
		g_bControlMode = false;
	}

	// ��Ʈ�� ��尡 Ȱ��ȭ�Ǿ� ���� �ʴٸ� ����
	if (g_bControlMode == false)
		return;

	// �α� ����� (��� ��ȯ�� �Ǿ���)
	//if (L'L' == ControlKey || L'l' == ControlKey)

	// S : ���� ����
	if (GetAsyncKeyState(0x53) & 0x0001)
		g_bShutdown = true;
}

// ���� ����͸� ������ ǥ�� �� ����, ���� �ϴ� ��� ���
void Monitoring() {

	static DWORD dwOldTime = NULL;
	static DWORD dwNowTime = 0;

	static int Loop = 1;

	if (dwOldTime == NULL) {

		dwOldTime = timeGetTime();

		return;
	}

	dwNowTime = timeGetTime();

	// 1�ʸ���
	if (dwNowTime - dwOldTime >= 1000) {

		DWORD AvrFps = (DWORD)g_TotalFps / 1000;

		printf("================================================================\n");
		printf("|| Loop : %d | AvrFPS : %d | MaxFPS : %d | Min FPS : %d ||\n", Loop, AvrFps, g_MaxFps, g_MinFps);
		printf("================================================================\n");

		// Fps ������ �ʱ�ȭ
		g_TotalFps = 0;
		g_MaxFps = 0;
		g_MinFps = 0xffffffff;

		dwOldTime = dwNowTime;

		Loop = 1;
	}

	Loop++;
}

//--------------------------------------------------------
// ������ �Լ���
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

// ĳ���� ������ǥ(shX, shY)�� ������ġ ����Ͽ� �ش� ���Ϳ� �ֱ�
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

// ĳ������ ������ǥ(shX, shY)�� ������ġ ����Ͽ� �ش� ���Ϳ��� ����
void SectorRemoveCharacter(st_CHARACTER *pCharacter) {

	// ĳ���Ϳ� ���������� ���ٸ� �����ִ� ��Ȳ
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
		// �������� ���� ����?
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

// ���� ��ġ�� ���Ϳ��� ���� ��, ���� ��ǥ�� ���ο� �ش� ���Ϳ� �ֱ�
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

// Ư�� ���� ��ǥ ���� ����� ���� ���
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

// ���Ϳ��� ���͸� �̵� ������, ���� ����ǿ��� ���� ����, ���� �߰��� ������ ���� ����
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
			
			// �ű��ϰ� ¥����
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

// ���Ͱ� ����� ��� Ŭ�󿡰� ���� ���� ����
void CharacterSectorUpdatePacket(st_CHARACTER *pCharacter) {

	st_SECTOR_AROUND RemoveSector;
	st_SECTOR_AROUND AddSector;

	st_CHARACTER *pExistCharacter;

	list<st_CHARACTER *> *pSectorList = NULL;
		
	CPacket Packet;
	int iCnt;

	GetUpdateSectorAround(pCharacter, &RemoveSector, &AddSector);

	// 1. RemoveSector�� ĳ���� ���� ��Ŷ������
	mpDeleteCharacter(&Packet, pCharacter->dwSessionID);

	for (iCnt = 0; iCnt < RemoveSector.iCount; iCnt++) {

		// Ư�� ���� �������� �޽����� �����ϴ� �Լ�
		SendPacket_SectorOne(RemoveSector.Around[iCnt].iX, 
			RemoveSector.Around[iCnt].iY, 
			&Packet, NULL);
	}

	// 2. ���� �����̴� �༮����, RemoveSector�� ĳ���͵� ���� ��Ŷ ������
	for (iCnt = 0; iCnt < RemoveSector.iCount; iCnt++) {

		pSectorList = &g_Sector[RemoveSector.Around[iCnt].iY][RemoveSector.Around[iCnt].iX];

		list<st_CHARACTER *>::iterator ListIter;
		list<st_CHARACTER *>::iterator ListIterEnd = pSectorList->end();
		for (ListIter = pSectorList->begin(); ListIter != ListIterEnd; ++ListIter) {

			mpDeleteCharacter(&Packet, (*ListIter)->dwSessionID);

			// Ư�� Ŭ���̾�Ʈ �� ���� �޽��� ����
			SendPacket_Unicast(pCharacter->pSession, &Packet);
		}
	}

	// 3. AddSectors �� ���� ��Ŷ ������
	mpCreateOtherCharacter(&Packet, pCharacter->dwSessionID,
		pCharacter->byDirection,
		pCharacter->shX,
		pCharacter->shY,
		pCharacter->chHP);

	for (iCnt = 0; iCnt < AddSector.iCount; iCnt++) {

		SendPacket_SectorOne(AddSector.Around[iCnt].iX, AddSector.Around[iCnt].iY, &Packet, NULL);
	}

	// 3-1. AddSector�� ĳ���� �̵� ��Ŷ ������
	mpMoveStart(&Packet, pCharacter->dwSessionID,
		pCharacter->byMoveDirection,
		pCharacter->shX,
		pCharacter->shY);

	for (iCnt = 0; iCnt < AddSector.iCount; iCnt++) {

		SendPacket_SectorOne(AddSector.Around[iCnt].iX, AddSector.Around[iCnt].iY, &Packet, NULL);
	}

	// 4. �̵��� �༮���� AddSector�� �ִ� ĳ���� ���� ��Ŷ ������
	for (iCnt = 0; iCnt < AddSector.iCount; iCnt++) {

		// ����� ���͸� ���鼭 ���͸���Ʈ ����
		pSectorList = &g_Sector[AddSector.Around[iCnt].iY][AddSector.Around[iCnt].iX];

		list<st_CHARACTER *>::iterator ListIter;
		list<st_CHARACTER *>::iterator ListIterEnd = pSectorList->end();
		// �ش� ���͸��� ��ϵ� ĳ���͵��� �̾Ƽ� ���� ��Ŷ ����� ����
		for (ListIter = pSectorList->begin(); ListIter != ListIterEnd; ++ListIter) {

			pExistCharacter = *ListIter;

			// ���� �ƴ� ��쿡��
			if (pExistCharacter != pCharacter) {

				mpCreateOtherCharacter(&Packet,
					pExistCharacter->dwSessionID,
					pExistCharacter->byDirection,
					pExistCharacter->shX,
					pExistCharacter->shY,
					pExistCharacter->chHP);

				SendPacket_Unicast(pCharacter->pSession, &Packet);

				// �� Add ĳ������ �׼ǿ� ���� ��Ŷ ���� ����
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

// Frame()�Լ�, true : �Ʒ��� ����, false : ���� ����
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

	// �Ѿ �ð��� 20�� �Ѿ��ٸ�
	if (dwRemainTime >= dfFRAME_SLEEP_TIME) {

		dwRemainTime += dwTime;
		dwRemainTime -= dfFRAME_SLEEP_TIME;

		iResult = true;
	}

	// �ð��� 20 ����
	else if (dwTime < dfFRAME_SLEEP_TIME) {

		dwRemainTime += dwTime;

		iResult = false;
	}

	// �ð��� 20ms �̻�
	else if (dwTime >= dfFRAME_SLEEP_TIME) {

		dwRemainTime += dwTime - dfFRAME_SLEEP_TIME;

		iResult = true;
	}

	dwOldTime = timeGetTime();
	return iResult;
}

// Update �Լ�
void Update() {

	st_CHARACTER *pCharacter = NULL;

	map<DWORD, st_CHARACTER *>::iterator CharacterIter = g_CharacterMap.begin();
	map<DWORD, st_CHARACTER *>::iterator CharacterIterEnd = g_CharacterMap.end();

	for (; CharacterIter != CharacterIterEnd;) {

		pCharacter = CharacterIter->second;
		++CharacterIter;
		// �������� �׽�Ʈ�� ���� �ּ�
		//if (pCharacter->chHP <= 0) {
		//	// ���ó��
		//	printf("SessionID : %d HP = 0\n", pCharacter->dwSessionID);
		//	Disconnect(pCharacter->pSession->Socket);
		//}
		
		// ���� �ð����� ���� ������ ����ó��
		/*DWORD dwCurrTick = timeGetTime();
		if (dwCurrTick - pCharacter->pSession->dwTrafficTick > dfNETWORK_PACKET_RECV_TIMEOUT) {

			printf("Timeout SessionID: %d\n", pCharacter->dwSessionID);
			Disconnect(pCharacter->pSession->Socket);
			continue;
		}*/

		// ���� ���ۿ� ���� ó��
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

			// �̵��� ��� ���� ������Ʈ�� ��
			if (SectorUpdateCharacter(pCharacter)) {

				// ���Ͱ� ����� ���� Ŭ�󿡰� ���� ������ ������
				CharacterSectorUpdatePacket(pCharacter);
			}
		}
	}
}

//--------------------------------------------------------
// ��Ʈ��ũ �Լ���
//--------------------------------------------------------

// SendUnicast ( �� ���ǿ� ���� )
void SendPacket_Unicast(st_SESSION *pSession, CPacket *pPacket) {

	// Session�� NULL
	if (pSession == NULL) {
		// Error �α�ó��?
		return;
	}

	// ��Ŷ�� ������ ���� ��
	if (pPacket->GetPacketSize() <= 0) {

		return;
	}

	pSession->SendQ.Enqueue((char *)pPacket->GetReadPtr(), pPacket->GetPacketSize());
}

// SendBroadcast (��� ���ǿ� ����)
void SendPacket_Broadcast(CPacket *pPacket) {

	st_SESSION *pSession = NULL;

	map<SOCKET, st_SESSION *>::iterator SessionIter;
	map<SOCKET, st_SESSION *>::iterator SessionIterEnd = g_SessionMap.end();

	for (SessionIter = g_SessionMap.begin(); SessionIter != SessionIterEnd; ++SessionIter) {

		pSession = SessionIter->second;
		SendPacket_Unicast(pSession, pPacket);
	}
}

// SendSectorOne ( �� ���Ϳ� �ְ� ExceptSession�� ������ ��� ���ǿ� ������ )
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

// Socket���� Session ã��
st_SESSION *FindSession(SOCKET Sock) {

	return g_SessionMap.find(Sock)->second;
}

// �� Session ����
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

// SessionID�� Chracter ã��
st_CHARACTER *FindCharacter(DWORD dwSessionID) {

	return g_CharacterMap.find(dwSessionID)->second;
}

// Session�� �־� ���ο� Character ����
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
	
	// ���ͺκ�
	SectorAddCharacter(pCharacter);

	// ĳ���� �ʿ� �߰�
	g_CharacterMap.insert(make_pair(pSession->dwSessionID, pCharacter));
	
	return pCharacter;
}

// ���� �� ĳ���� ����� ( �� ã�� �������ϳ�? pSession �־ �ٷ� �ذ� �Ұ�?)
void Disconnect(SOCKET Sock) {

	// ĳ���� �� ���� �����
	st_SESSION *pSession = NULL;
	st_CHARACTER *pCharacter = NULL;
	st_SECTOR_AROUND SectorAround;

	pSession = g_SessionMap.find(Sock)->second;
	g_SessionMap.erase(Sock);
	
	pCharacter = g_CharacterMap.find(pSession->dwSessionID)->second;
	g_CharacterMap.erase(pSession->dwSessionID);

	// ���� ���Ϳ� �ش� ĳ���� ���� ����
	// 9�� ���Ϳ� �ִ� ��� �������� �޽��� �۽�
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

	// �ش� ���� �����
	SectorRemoveCharacter(pCharacter);

	//CLogMaker::GetInstance()->Log(dfLOG_LEVEL_DEBUG, 
	//	"Disconnect SessionID : %d\n", pSession->dwSessionID);
	
	// closesocket
	closesocket(pSession->Socket);

	// �޸� ����
	delete pSession;
	delete pCharacter;
}

// ��Ʈ��ũ �ʱ�ȭ �� listen
bool NetStartUp(void) {

	// ���� ����
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

// ��Ʈ��ũ ����
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
		// ū������
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

	// ���� ��ǥ ����( shX, shY �� �̹� ���巹Ŀ�� �۾� ��)
	pCharacter->shX = shX;
	pCharacter->shY = shY;

	if (SectorUpdateCharacter(pCharacter)) {

		CharacterSectorUpdatePacket(pCharacter);
	}

	// ���巹Ŀ���� ���� ���� ����
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
		// ū������
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

	// ID �� ĳ���� �˻�
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

	// ĳ���� ���� ����
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

	// ���� ��ǥ ����( shX, shY �� �̹� ���巹Ŀ�� �۾� ��)
	pCharacter->shX = shX;
	pCharacter->shY = shY;

	if (SectorUpdateCharacter(pCharacter)) {

		CharacterSectorUpdatePacket(pCharacter);
	}

	// ���巹Ŀ���� ���� ���� ����
	pCharacter->dwActionTick = timeGetTime();
	pCharacter->shActionX = pCharacter->shX;
	pCharacter->shActionY = pCharacter->shY;

	// mpMoveStop
	mpMoveStop(pPacket, pSession->dwSessionID, pCharacter->byDirection, pCharacter->shX, pCharacter->shY);
	
	SendPacket_Around(pSession, pPacket);
	//SendPacket_Broadcast(pPacket);

	return true;
}

// �浹 üũ, �´� ���� �ִٸ� �´� �� ptr ����, ���ٸ� NULL ����
st_CHARACTER *Collision(st_CHARACTER *pAttacker, int iHitRangeX, int iHitRangeY) {

	// ������ ĳ���Ͱ� �ִ��� ����üũ
	if (pAttacker == NULL) {

		printf("Attacker Not Found! Error\n");
		return NULL;
	}

	// �� ���� X Y �ȼ� ���� �˱�
	int iSectorX = pAttacker->CurSector.iX;
	int iSectorY = pAttacker->CurSector.iY;

	int iSectorStartX = iSectorX * dfSECTOR_PIXEL_WIDTH;
	int iSectorEndX = iSectorStartX + dfSECTOR_PIXEL_WIDTH;
	int iSectorStartY = iSectorY * dfSECTOR_PIXEL_HEIGHT;
	int iSectorEndY = iSectorStartY + dfSECTOR_PIXEL_HEIGHT;

	// ������ ����, ���� �ȼ� ���� �˱�
	short shRangeXStart = 0;
	short shRangeXEnd = 0;
	short shRangeYStart = 0;
	short shRangeYEnd = 0;

	// �˻��ؾ��� ���� ã������ �浹 üũ BYTE
	BYTE byCollisionFlag = 0x00;

	// �˻��� ���� ��ǥ ���� ������ Sector�迭 �� �ʱ�ȭ 
	// (���Ͱ� ������ ũ���� �� �ִ� 4��, ���Ͱ� �ʹ� ���� �� �ִ� 9�� Sector �˻�)
	st_SECTOR_POS stSectorCollision[9] = { {-1, -1}, };

	// stSectorCollision�� Index
	int iIndex = 0;

	// �� ĳ������ ���� ���⿡ ���� �˻��ؾߵ� ���� ���� (LL)
	if (pAttacker->byDirection == dfPACKET_MOVE_DIR_LL) {

		// pAttacker�� ���� ����
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

	// �� ĳ������ ���� ���⿡ ���� �˻��ؾߵ� ���� ���� (RR)
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

	// 8�� ���� ��, üũ�� �� ���� ����
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

	// byCollisionFlag ���� ���� üũ�� ���� ��ǥ �迭�� �ֱ�
	// �ڽ��� ���� ���� �ֱ� (�˻� 0����)
	stSectorCollision[iIndex].iX = iSectorX;
	stSectorCollision[iIndex].iY = iSectorY;
	iIndex++;

	// �¿� POS �ֱ� (�˻� 1����)
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

	// ���� POS�ֱ� (�˻� 2����)
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

	// �밢�� POS�ֱ� (�˻� 3����)
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
	// stSectorCollision�� ���� Pos ���� iter ���鼭 pDamaged ����
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
		// ū������
		printf("# Attack_1 # Session is NULL!!!\n");
	}

	BYTE byDirection = 0;
	short shX = 0;
	short shY = 0;

	*pPacket >> byDirection;
	*pPacket >> shX;
	*pPacket >> shY;

	// Attacker(Character) ã�Ƽ� �浹ó��
	st_CHARACTER *pCharacter = FindCharacter(pSession->dwSessionID);
	st_CHARACTER *pDamaged = NULL;

	// DeadReckoning ó��
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

	// ĳ���� ���� ����
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

	// ���� ��ǥ ����( shX, shY �� �̹� ���巹Ŀ�� �۾� ��)
	pCharacter->shX = shX;
	pCharacter->shY = shY;

	// ���� ��� �ִ� 9�� ���Ϳ� �ݿ� ( �ڽ� ���� )
	mpAttack1(pPacket, 
		pSession->dwSessionID, 
		pCharacter->byDirection, 
		pCharacter->shX, pCharacter->shY);

	SendPacket_Around(pSession, pPacket);

	// ������ ������ �ִ� 9�� ���Ϳ� �ݿ� ( �ڽ� ���� )
	pDamaged = Collision(pCharacter, dfATTACK1_RANGE_X, dfATTACK1_RANGE_Y);

	if (pDamaged == NULL) {

		//printf("ID :%d - Attack1 Miss\n", pCharacter->dwSessionID);
		return true;
	}

	// hp ���̱�
	pDamaged->chHP -= dfATTACK1_DAMAGE * 2;

	// DamagePacket
	mpDamage(pPacket, pSession->dwSessionID, pDamaged->dwSessionID, pDamaged->chHP);
	
	SendPacket_Around(pSession, pPacket, true);

	return true;
}

// NetProcAttack2
bool NetPacketProc_Attack2(st_SESSION *pSession, CPacket *pPacket) {

	if (pSession == NULL) {
		// ū������
		printf("# Attack_2 # Session is NULL!!!\n");
	}

	BYTE byDirection = 0;
	short shX = 0;
	short shY = 0;

	*pPacket >> byDirection;
	*pPacket >> shX;
	*pPacket >> shY;

	// Attacker(Character) ã�Ƽ� �浹ó��
	st_CHARACTER *pCharacter = FindCharacter(pSession->dwSessionID);
	st_CHARACTER *pDamaged = NULL;

	// DeadReckoning ó��
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

	// ĳ���� ���� ����
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

	// ���� ��ǥ ����( shX, shY �� �̹� ���巹Ŀ�� �۾� ��)
	pCharacter->shX = shX;
	pCharacter->shY = shY;

	// ���� ��� �ִ� 9�� ���Ϳ� �ݿ� ( �ڽ� ���� )
	mpAttack2(pPacket,
		pSession->dwSessionID,
		pCharacter->byDirection,
		pCharacter->shX, pCharacter->shY);

	SendPacket_Around(pSession, pPacket);

	// ������ ������ �ִ� 9�� ���Ϳ� �ݿ� ( �ڽ� ���� )
	pDamaged = Collision(pCharacter, dfATTACK2_RANGE_X, dfATTACK2_RANGE_Y);

	if (pDamaged == NULL) {

		//printf("ID :%d - Attack2 Miss\n", pCharacter->dwSessionID);
		return true;
	}

	// hp ���̱�
	pDamaged->chHP -= dfATTACK2_DAMAGE * 2;

	// DamagePacket
	mpDamage(pPacket, pSession->dwSessionID, pDamaged->dwSessionID, pDamaged->chHP);

	SendPacket_Around(pDamaged->pSession, pPacket, true);

	return true;
}

// NetProcAttack3
bool NetPacketProc_Attack3(st_SESSION *pSession, CPacket *pPacket) {
	
	if (pSession == NULL) {
		// ū������
		printf("# Attack_3 # Session is NULL!!!\n");
	}

	BYTE byDirection = 0;
	short shX = 0;
	short shY = 0;

	*pPacket >> byDirection;
	*pPacket >> shX;
	*pPacket >> shY;

	// Attacker(Character) ã�Ƽ� �浹ó��
	st_CHARACTER *pCharacter = FindCharacter(pSession->dwSessionID);
	st_CHARACTER *pDamaged = NULL;

	// DeadReckoning ó��
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

	// ĳ���� ���� ����
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

	// ���� ��ǥ ����( shX, shY �� �̹� ���巹Ŀ�� �۾� ��)
	pCharacter->shX = shX;
	pCharacter->shY = shY;

	// ���� ��� �ִ� 9�� ���Ϳ� �ݿ� ( �ڽ� ���� )
	mpAttack3(pPacket,
		pSession->dwSessionID,
		pCharacter->byDirection,
		pCharacter->shX, pCharacter->shY);

	SendPacket_Around(pSession, pPacket);

	// ������ ������ �ִ� 9�� ���Ϳ� �ݿ� ( �ڽ� ���� )
	pDamaged = Collision(pCharacter, dfATTACK3_RANGE_X, dfATTACK3_RANGE_Y);

	if (pDamaged == NULL) {

		//printf("ID :%d - Attack3 Miss\n", pCharacter->dwSessionID);
		return true;
	}

	// hp ���̱�
	pDamaged->chHP -= dfATTACK3_DAMAGE * 2;

	// DamagePacket
	mpDamage(pPacket, pSession->dwSessionID, pDamaged->dwSessionID, pDamaged->chHP);

	SendPacket_Around(pSession, pPacket, true);

	return true;
}

bool NetPacketProc_Echo(st_SESSION *pSession, CPacket *pPacket) {

	if (pSession == NULL) {
		// ū������
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

// ��Ʈ��ũ ���� ��Ŷ ó��
int ComplateRecvPacket(st_SESSION *pSession) {

	st_PACKET_HEADER	stHeader;
	
	int		iRecvQSize;

	iRecvQSize = pSession->RecvQ.GetUseSize();

	if (iRecvQSize < sizeof(stHeader))
		return 1;

	pSession->RecvQ.Peek((char *)&stHeader, sizeof(st_PACKET_HEADER));
	if (stHeader.byCode != dfNETWORK_PACKET_CODE)
		return -1;

	// +1 = EndCode ũ��
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

// ��Ʈ��ũ Send ó��
bool NetProcSend(SOCKET Socket) {

	// Ŭ�� ���� ����� return false;

	st_SESSION *pSession;
	int iBuffSize;
	int iResult;

	pSession = FindSession(Socket);

	if (pSession == NULL) {

		//CLogMaker::GetInstance()->Log(dfLOG_LEVEL_ERROR, "NetProcSend Error / Session NULL");
		printf("NetProcSend Error / Session NULL\n");
		return false;
	}

	// SendQ�� ���� á�ٸ� �̻��� ��Ȳ Disconnect
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

// ��Ʈ��ũ Accept ó��
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

	// �� ���� ����
	pSession = CreateSession(ClientSock);

	// �� ĳ���� ����
	pCharacter = CreateCharacter(pSession);

	int iSectorX = pCharacter->CurSector.iX;
	int iSectorY = pCharacter->CurSector.iY;
	GetSectorAround(iSectorX, iSectorY, &SectorAround);

	// 1. �� ���� ���� ����
	// 2. ���� ���� �÷��̾���� ���� ���� ����
	// 3. ���� ���� �÷��̾�鿡�� �� ���� ���� (Other)
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

	// ���� ���� (���� ���� ��)

	// ������, �ֺ� ���� �� �� ���� ���� �ٸ� ĳ���� ���� �޽��� �۽�
	// �� ĳ���� ��Ŷ ������
	//mpCreateCharacter(&Packet,
	//	pCharacter->dwSessionID,
	//	pCharacter->byDirection,
	//	pCharacter->shX,
	//	pCharacter->shY,
	//	pCharacter->chHP);
	//
	//// ���� ��������� ĳ���͸� ��� Ŭ�󿡰� ������ (���߿� ����?)
	//pSession->SendQ.Enqueue(Packet.GetReadPtr(), Packet.GetPacketSize());
	//int a = 0;
	//
	// ĳ���� ������(������ broadcast ���߿��� Sector)
	//map<DWORD, st_CHARACTER *>::iterator iter = g_CharacterMap.begin();
	//map<DWORD, st_CHARACTER *>::iterator iterEnd = g_CharacterMap.end();
	//
	//st_CHARACTER *pCharacterNow;
	//for (; iter != iterEnd; ++iter) {
	//
	//	pCharacterNow = iter->second;
	//	// ���� ������ ĳ���Ͱ� �ƴ϶��
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

// ��Ʈ��ũ Recv ó��
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

	// RecvQ�� ������� ���ٸ�, ������ Ŭ���̹Ƿ� ����
	int iFreeSize = pSession->RecvQ.GetFreeSize();
	if (iFreeSize <= 0) {
		
		//CLogMaker::GetInstance()->Log(dfLOG_LEVEL_ERROR, "NetProcSend Error / 0 FREE Size");
		printf("NetProcSend Error / 0 FREE Size\n");
		Disconnect(pSession->Socket);
		return;
	}

	// ���������� ���� �޽��� Ÿ��
	pSession->dwTrafficTick = timeGetTime();

	// �ޱ��۾�
	iBuffSize = pSession->RecvQ.GetEnqSize();
	iResult = recv(pSession->Socket, pSession->RecvQ.GetRearBufferPtr(), iBuffSize, 0);
	if (iResult == SOCKET_ERROR || iResult == 0) {

		//CLogMaker::GetInstance()->Log(dfLOG_LEVEL_ERROR, "NetProcSend Error / recv Error");
		printf("NetProcRecv Error / recv Error\n");
		Disconnect(pSession->Socket);
		return;
	}

	// ���� �����Ͱ� ����
	if (iResult > 0) {

		// RecvQ�� �ݿ�
		pSession->RecvQ.MoveRear(iResult);

		// RecvQ�� �ִ� ��� ��Ŷ ó��
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

// ��Ʈ��ũ Select ó��
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
				
				// �ش�Ŭ�� ���� ������ ��찡 �����Ƿ� Ȯ���ϰ� ����
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

// ��Ʈ��ũ �ۼ��� ó��
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

		//�ش� Ŭ�� ReadSet ���
		UserTableSOCKET[iSocketCount] = pSession->Socket;
		
		FD_SET(pSession->Socket, &ReadSet);

		// �ش� Ŭ�� SendQ�� ������ �ִٸ� WriteSet ���
		if (pSession->SendQ.GetUseSize() > 0)
			FD_SET(pSession->Socket, &WriteSet);

		iSocketCount++;
		SessionIter++;

		// Select �ִ�ġ ���޽�, Select ȣ�� �� ����
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

	// 64���� �ȵ����� 0�� �̻��϶� Select ȣ�� �� ����
	if (iSocketCount > 0) {
		NetSelectSocket(UserTableSOCKET, &ReadSet, &WriteSet);
	}
}

#endif


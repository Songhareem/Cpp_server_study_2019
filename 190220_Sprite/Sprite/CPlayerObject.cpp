#include "stdafx.h"
#include "e_SPRITE.h"
#include "e_OBJECT_TYPE.h"
#include "CPlayerObject.h"
#include "CSpriteDib.h"
#include "PacketDefine.h"

extern CBaseObject *g_pPlayerObject;

CPlayerObject::CPlayerObject() {}

CPlayerObject::CPlayerObject(int iObjectID, int iObjectType, bool bIsPlayer, BYTE byDir, BYTE byHP) 
	: CBaseObject(iObjectID, iObjectType), m_byDirCur(byDir), m_byHP(byHP), m_bPlayerCharacter(bIsPlayer) ,
	  m_iActionCur(dfACTION_STAND), m_iActionOld(dfACTION_STAND)
{
}

CPlayerObject::~CPlayerObject() {}

void CPlayerObject::Run() {

	NextFrame();
	ActionProc();
}

void CPlayerObject::Render(BYTE *pDest, int iWidth, int iHeight, int iPitch) {

	// 그림자 출력
	CSpriteDib::GetInstance()->DrawAlphaSprite(static_cast<int>(e_SPRITE::eSHADOW), m_iCurX, m_iCurY, pDest, iWidth, iHeight, iPitch);

	//플레이어 객체 출력
	if(IsPlayer() == true)
		CSpriteDib::GetInstance()->DrawPlayerSprite(GetSprite(), m_iCurX, m_iCurY, pDest, iWidth, iHeight, iPitch);

	else
		CSpriteDib::GetInstance()->DrawSprite(GetSprite(), m_iCurX, m_iCurY, pDest, iWidth, iHeight, iPitch);

	// HP 출력
	CSpriteDib::GetInstance()->DrawSprite(static_cast<int>(e_SPRITE::eGUAGE_HP), m_iCurX - 34, m_iCurY + 8, pDest, iWidth, iHeight, iPitch, m_byHP);
}

void CPlayerObject::ActionProc() {

	switch (m_iActionCur) {

	case dfACTION_ATTACK1:
	case dfACTION_ATTACK2:
	case dfACTION_ATTACK3:
		if (IsEndFrame()) {

			// 기본자세로
			SetActionStand();
			m_iActionCur = dfACTION_STAND;
		}
		else
			break;

	default:
		InputActionProc();
		break;
	}
}

void CPlayerObject::SetAnimation() {

	if (m_iActionCur == m_iActionOld)
		return;

	// 애니메이션 설정
	switch (m_iActionCur) {
	case dfACTION_STAND:
		SetActionStand();
		break;

	case dfACTION_MOVE_LL:
	case dfACTION_MOVE_LU:
	case dfACTION_MOVE_LD:
		SetDirection(LEFT);
		if (m_iActionOld == dfACTION_MOVE_LL || m_iActionOld == dfACTION_MOVE_LU || m_iActionOld == dfACTION_MOVE_LD)
			break;
		SetActionMove();
		break;
	case dfACTION_MOVE_RR:
	case dfACTION_MOVE_RU:
	case dfACTION_MOVE_RD:
		SetDirection(RIGHT);
		if (m_iActionOld == dfACTION_MOVE_RR || m_iActionOld == dfACTION_MOVE_RU || m_iActionOld == dfACTION_MOVE_RD)
			break;
		SetActionMove();
		break;
	case dfACTION_MOVE_UU:
	case dfACTION_MOVE_DD:
		if (m_iActionOld == dfACTION_MOVE_UU || m_iActionOld == dfACTION_MOVE_DD)
			break;
		SetActionMove();
		break;
	case dfACTION_ATTACK1:
		SetActionAttack1();
		break;
	case dfACTION_ATTACK2:
		SetActionAttack2();
		break;
	case dfACTION_ATTACK3:
		SetActionAttack3();
		break;
	}

	CPlayerObject *pPlayer = (CPlayerObject *)g_pPlayerObject;
	if (m_iObjectID != pPlayer->GetObjectID())
		return;

	// 전 액션이 움직임이고 바뀐 액션이 공격이라면 STOP한번 전송
	if (m_iActionOld == dfACTION_MOVE_LL || m_iActionOld == dfACTION_MOVE_LU
		|| m_iActionOld == dfACTION_MOVE_UU || m_iActionOld == dfACTION_MOVE_RU
		|| m_iActionOld == dfACTION_MOVE_RR || m_iActionOld == dfACTION_MOVE_RD
		|| m_iActionOld == dfACTION_MOVE_DD || m_iActionOld == dfACTION_MOVE_LD) {
			if (m_iActionCur == dfACTION_ATTACK1 || m_iActionCur == dfACTION_ATTACK2 || m_iActionCur == dfACTION_ATTACK3) {
				HEADER Head;
			stPACKET_CS_MOVE Pack;
			CS_MoveStop(&Head, &Pack, pPlayer->GetDirection(), pPlayer->GetCurX(), pPlayer->GetCurY());
			SendPacket(&Head, (char *)&Pack, sizeof(Pack));
		}
	}

	switch (m_iActionCur) {
	
	case dfACTION_STAND:
	{
		HEADER Head;
		stPACKET_CS_MOVE Pack;
		CS_MoveStop(&Head, &Pack, pPlayer->GetDirection(), pPlayer->GetCurX(), pPlayer->GetCurY());
		SendPacket(&Head, (char *)&Pack, sizeof(Pack));
	}
	break;
	case dfACTION_MOVE_LL:
	case dfACTION_MOVE_LU:
	case dfACTION_MOVE_UU:
	case dfACTION_MOVE_RU:
	case dfACTION_MOVE_RR:
	case dfACTION_MOVE_RD:
	case dfACTION_MOVE_DD:
	case dfACTION_MOVE_LD:
	{
		HEADER Head;
		stPACKET_CS_MOVE Pack;
		CS_MoveStart(&Head, &Pack, m_iActionCur - 1, pPlayer->GetCurX(), pPlayer->GetCurY());
		SendPacket(&Head, (char *)&Pack, sizeof(Pack));
	}
	break;
	case dfACTION_ATTACK1:
	{
		HEADER Head;
		stPACKET_CS_ATTACK Pack;
		CS_Attack1(&Head, &Pack, pPlayer->GetDirection(), pPlayer->GetCurX(), pPlayer->GetCurY());
		SendPacket(&Head, (char *)&Pack, sizeof(Pack));
		}
		break;
	case dfACTION_ATTACK2:
	{
		HEADER Head;
		stPACKET_CS_ATTACK Pack;
		CS_Attack2(&Head, &Pack, pPlayer->GetDirection(), pPlayer->GetCurX(), pPlayer->GetCurY());
		SendPacket(&Head, (char *)&Pack, sizeof(Pack));
	}
	break;
	case dfACTION_ATTACK3:
	{
		HEADER Head;
		stPACKET_CS_ATTACK Pack;
		CS_Attack3(&Head, &Pack, pPlayer->GetDirection(), pPlayer->GetCurX(), pPlayer->GetCurY());
		SendPacket(&Head, (char *)&Pack, sizeof(Pack));
	}
	break;
		default:
			break;
	}
}

void CPlayerObject::InputActionProc() {

	//# 이동 픽셀
	//	X 축 - 3 Pixel
	//	Y 축 - 2 Pixel

	m_iActionCur = m_iActionInput;

	// 좌표설정
	if (dfRANGE_MOVE_LEFT >= m_iCurX && (
		m_iActionInput == dfACTION_MOVE_LL || 
		m_iActionInput == dfACTION_MOVE_LU || 
		m_iActionInput == dfACTION_MOVE_LD ))
	{
		m_iActionInput = dfACTION_STAND;
		m_iActionCur = dfACTION_STAND;
	}

	if (dfRANGE_MOVE_RIGHT <= m_iCurX && (
		m_iActionInput == dfACTION_MOVE_RR ||
		m_iActionInput == dfACTION_MOVE_RU ||
		m_iActionInput == dfACTION_MOVE_RD))
	{
		m_iActionInput = dfACTION_STAND;
		m_iActionCur = dfACTION_STAND;
	}

	if (dfRANGE_MOVE_TOP >= m_iCurY && (
		m_iActionInput == dfACTION_MOVE_UU ||
		m_iActionInput == dfACTION_MOVE_LU ||
		m_iActionInput == dfACTION_MOVE_RU))
	{
		m_iActionInput = dfACTION_STAND;
		m_iActionCur = dfACTION_STAND;
	}

	if (dfRANGE_MOVE_BOTTOM <= m_iCurY && (
		m_iActionInput == dfACTION_MOVE_DD ||
		m_iActionInput == dfACTION_MOVE_LD ||
		m_iActionInput == dfACTION_MOVE_RD))
	{
		m_iActionInput = dfACTION_STAND;
		m_iActionCur = dfACTION_STAND;
	}

	switch (m_iActionInput) {

	case dfACTION_MOVE_LL:
		m_iCurX -= 3;
		break;
	case dfACTION_MOVE_UU:
		m_iCurY -= 2;
		break;
	case dfACTION_MOVE_RR:
		m_iCurX += 3;
		break;
	case dfACTION_MOVE_DD:
		m_iCurY += 2;
		break;
	case dfACTION_MOVE_LU:
		m_iCurX -= 3;
		m_iCurY -= 2;
		break;
	case dfACTION_MOVE_RU:
		m_iCurX += 3;
		m_iCurY -= 2;
		break;
	case dfACTION_MOVE_RD:
		m_iCurX += 3;
		m_iCurY += 2;
		break;
	case dfACTION_MOVE_LD:
		m_iCurX -= 3;
		m_iCurY += 2;
		break;
	}

	SetAnimation();

	m_iActionOld = m_iActionCur;
}

bool CPlayerObject::IsPlayer() { return m_bPlayerCharacter; }

//Getter
BYTE CPlayerObject::GetDirection() { return m_byDirCur; }
BYTE CPlayerObject::GetHp() { return m_byHP; }

//Setter
void CPlayerObject::SetActionAttack1() { 
	
	BYTE Dir = GetDirection();

	if (Dir == LEFT)
		SetSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK1_L_01), static_cast<int>(e_SPRITE::ePLAYER_ATTACK1_L_MAX), dfDELAY_ATTACK1);

	else if(Dir == RIGHT)
		SetSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK1_R_01), static_cast<int>(e_SPRITE::ePLAYER_ATTACK1_R_MAX), dfDELAY_ATTACK1);

	m_bEndFrame = false;
}

void CPlayerObject::SetActionAttack2() {

	BYTE Dir = GetDirection();

	if (Dir == LEFT)
		SetSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK2_L_01), static_cast<int>(e_SPRITE::ePLAYER_ATTACK2_L_MAX), dfDELAY_ATTACK2);

	else if (Dir == RIGHT)
		SetSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK2_R_01), static_cast<int>(e_SPRITE::ePLAYER_ATTACK2_R_MAX), dfDELAY_ATTACK2);

	m_bEndFrame = false;
}

void CPlayerObject::SetActionAttack3() {

	BYTE Dir = GetDirection();

	if (Dir == LEFT)
		SetSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK3_L_01), static_cast<int>(e_SPRITE::ePLAYER_ATTACK3_L_MAX), dfDELAY_ATTACK3);

	else if (Dir == RIGHT)
		SetSprite(static_cast<int>(e_SPRITE::ePLAYER_ATTACK3_R_01), static_cast<int>(e_SPRITE::ePLAYER_ATTACK3_R_MAX), dfDELAY_ATTACK3);

	m_bEndFrame = false;
}

void CPlayerObject::SetActionMove() {

	BYTE Dir = GetDirection();

	if (Dir == LEFT)
		SetSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_L_01), static_cast<int>(e_SPRITE::ePLAYER_MOVE_L_MAX), dfDELAY_MOVE);

	else if (Dir == RIGHT)
		SetSprite(static_cast<int>(e_SPRITE::ePLAYER_MOVE_R_01), static_cast<int>(e_SPRITE::ePLAYER_MOVE_R_MAX), dfDELAY_MOVE);

	m_bEndFrame = false;
}

void CPlayerObject::SetActionStand() {
	
	BYTE Dir = GetDirection();

	if (Dir == LEFT)
		SetSprite(static_cast<int>(e_SPRITE::ePLAYER_STAND_L_01), static_cast<int>(e_SPRITE::ePLAYER_STAND_L_MAX), dfDELAY_STAND);

	else if (Dir == RIGHT)
		SetSprite(static_cast<int>(e_SPRITE::ePLAYER_STAND_R_01), static_cast<int>(e_SPRITE::ePLAYER_STAND_R_MAX), dfDELAY_STAND);

	m_bEndFrame = false;
}

void CPlayerObject::SetDirection(BYTE byDir) { m_byDirCur = byDir; }

void CPlayerObject::SetHP(BYTE byHP) { m_byHP = byHP; }
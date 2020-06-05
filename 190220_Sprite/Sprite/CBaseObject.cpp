#include "stdafx.h"
#include "CBaseObject.h"
#include "e_SPRITE.h"

CBaseObject::CBaseObject() {}
CBaseObject::CBaseObject(int iObjectID, int iObjectType) : m_iObjectID(iObjectID), m_iObjectType(iObjectType) {}
CBaseObject::~CBaseObject() {}

void CBaseObject::ActionInput(int ActionCmd) {

	m_iActionInput = ActionCmd;
}

bool CBaseObject::IsEndFrame() { return m_bEndFrame; }

void CBaseObject::NextFrame() {

	//int save;
	// 없는 스프라이트
	if (m_iSpriteStart < 0) {
		return;
	}

	if (++m_iDelayCount > m_iFrameDelay) {

		m_iDelayCount = 0;

		m_iSpriteNow++;

		if (m_iSpriteNow > m_iSpriteEnd) {

			m_iSpriteNow = m_iSpriteStart;
			m_bEndFrame = true;
		}
	}
}

//Getter
int CBaseObject::GetCurX() { return m_iCurX; }
int CBaseObject::GetCurY() { return m_iCurY; }
int CBaseObject::GetObjectID() { return m_iObjectID; }
int CBaseObject::GetObjectType() { return m_iObjectType; }
int CBaseObject::GetSprite() { return m_iSpriteNow; }
int CBaseObject::GetAction() { return m_iActionInput; }

//Setter
void CBaseObject::SetPosition(int iX, int iY) { 
	
	m_iCurX = iX; 
	m_iCurY = iY;
}

void CBaseObject::SetSprite(int iSpriteStart, int iSpriteEnd,int iFrameDelay) {

	m_iSpriteNow = m_iSpriteStart = iSpriteStart;

	m_iSpriteEnd = iSpriteEnd;

	m_iFrameDelay = iFrameDelay;

	m_iDelayCount = 0;

	m_bEndFrame = false;

}
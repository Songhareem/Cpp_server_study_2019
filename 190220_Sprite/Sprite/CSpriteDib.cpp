
#include "stdafx.h"
#include "CSpriteDib.h"

CSpriteDib::CSpriteDib(int iMaxSprite, DWORD dwColorKey)
{
	m_iMaxSprite = iMaxSprite;
	m_dwColorKey = dwColorKey;
	pSprite = NULL;

	pSprite = new SPRITE[m_iMaxSprite];
	memset(this->pSprite, 0, sizeof(SPRITE)*maxSprite);
}

CSpriteDib::~CSpriteDib()
{
	// 전체를 돌면서 모두 지우기
	int count;
	for (count = 0; count < maxSprite; count++)
	{
		ReleaseSprite(count);
	}
}

bool CSpriteDib::LoadDibSprite(int iSpriteIndex, const char *szFileName, int iCenterX, int iCenterY) {


}

void CSpriteDib::ReleaseSprite(int iSpriteIndex) {


}

void CSpriteDib::DrawSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *by_pDest,
	int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen = 100) {


}

void CSpriteDib::DrawPlayerSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *by_pDest,
	int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen = 100) {


}

void CSpriteDib::DrawAlphaSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *by_pDest,
	int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen = 100) {


}

void CSpriteDib::DrawInvisibleSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *by_pDest,
	int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen = 100) {


}

void CSpriteDib::DrawImage(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *by_pDest,
	int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen = 100) {


}

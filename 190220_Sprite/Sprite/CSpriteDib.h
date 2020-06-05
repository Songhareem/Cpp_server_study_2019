#pragma once

#ifndef __SPRITE_DIB__
#define __SPRITE_DIB__

class CSpriteDib
{	
public:
	typedef struct {

		BYTE *by_pImage;	// byte 형 스프라이트 이미지 포인터
		int	 iWidth;		// int 형 너비
		int  iHeight;		// int 형 높이 
		int  iPitch;		// int 형 피치

		int  iCenterX;		// int 형 중점X
		int  iCenterY;		// int 형 중점Y 
	
	} st_SPRITE;

	bool LoadDibSprite(int iSpriteIndex, const char *szFileName, int iCenterX, int iCenterY);
	void ReleaseSprite(int iSpriteIndex);

	static CSpriteDib *GetInstance() { return &g_SpriteDib; };

	void DrawSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *by_pDest, 
					int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen = 100);

	void DrawPlayerSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *by_pDest,
		int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen = 100);

	void DrawAlphaSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *by_pDest,
		int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen = 100);

	void DrawInvisibleSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *by_pDest,
		int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen = 100);

	void DrawImage(int iSpriteIndex, int iDrawX, int iDrawY, BYTE *by_pDest,
		int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen = 100);

private:

	CSpriteDib(int iMaxSprite = MAX_IMAGE, DWORD dwColorKey = COLOR_KEY);
	virtual ~CSpriteDib();

protected:

	int m_iMaxSprite;
	st_SPRITE *m_pSprite;
	DWORD m_dwColorKey;

private:

	static CSpriteDib g_SpriteDib;
};

#endif
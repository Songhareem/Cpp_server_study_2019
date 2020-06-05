#pragma once

#ifndef __SPRITE_DIB__
#define __SPRITE_DIB__

class CSpriteDib
{	
public:
	typedef struct {

		BYTE *by_pImage;	// byte �� ��������Ʈ �̹��� ������
		int	 iWidth;		// int �� �ʺ�
		int  iHeight;		// int �� ���� 
		int  iPitch;		// int �� ��ġ

		int  iCenterX;		// int �� ����X
		int  iCenterY;		// int �� ����Y 
	
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
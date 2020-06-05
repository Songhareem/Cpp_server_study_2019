#pragma once
class CBaseObject
{
public:
	CBaseObject();
	CBaseObject(int iObjectID, int iObjectType);
	virtual ~CBaseObject() = 0;
	
	virtual void Run() = 0;
	virtual void Render(BYTE *pDest, int iWidth, int iHeight, int iPitch) = 0;

	void ActionInput(int ActionCmd);
	bool IsEndFrame();
	void NextFrame();

	//Getter
	int GetCurX();
	int GetCurY();
	int GetObjectID();
	int GetObjectType();
	int GetSprite();
	int GetAction();

	//Setter
	void SetPosition(int iX, int iY);
	void SetSprite(int iSpriteStart, int iSpriteEnd, int iFrameDelay);

	//int m_iCurX;

protected:
	
	bool m_bEndFrame;
	
	int m_iActionInput; // Å¥ ¿ªÇÒ
	
	int m_iCurX;
	int m_iCurY;
	
	int m_iDelayCount;
	int m_iFrameDelay;
	
	int m_iObjectID;
	int m_iObjectType;
	
	int m_iSpriteStart;
	int m_iSpriteNow;
	int m_iSpriteEnd;
};


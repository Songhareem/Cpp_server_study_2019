#pragma once
#include "CBaseObject.h"

class CPlayerObject :
	public CBaseObject
{
public:
	
	CPlayerObject();
	CPlayerObject(int iObjectID, int iObjectType, bool bIsPlayer, BYTE byDir, BYTE byHP);
	virtual ~CPlayerObject();

	virtual void Run();
	virtual void Render(BYTE *pDest, int iWidth, int iHeight, int iPitch);

	// 일반 펑션 쓰는것부터
	void ActionProc();
	void InputActionProc();
	bool IsPlayer();
	
	//Getter
	BYTE GetDirection();
	BYTE GetHp();

	//Setter
	void SetAnimation();
	void SetActionAttack1();
	void SetActionAttack2();
	void SetActionAttack3();
	void SetActionMove();
	void SetActionStand();
	void SetDirection(BYTE byDir);
	void SetHP(BYTE byHP);

private:
	
	bool m_bPlayerCharacter;
	
	BYTE m_byHP;

	int m_iActionCur;
	int m_iActionOld;

	BYTE m_byDirCur;
	BYTE m_byDirOld;
};


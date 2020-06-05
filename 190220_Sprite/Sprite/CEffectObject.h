#pragma once
#include "CBaseObject.h"
class CEffectObject :
	public CBaseObject
{
public:
	
	CEffectObject();
	virtual ~CEffectObject();

	virtual void Run();
	virtual void Render(BYTE *pDest, int iWidth, int iHeight, int iPitch);

private:
	BOOL   m_bEffectStart;
	DWORD  m_dwAttackID;
};


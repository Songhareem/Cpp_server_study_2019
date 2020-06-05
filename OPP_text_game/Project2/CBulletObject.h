#pragma once

#ifndef __CBULLETOBJECT__
#define __CBULLETOBJECT__

#include "CBaseObject.h"

class CBulletObject :
	public CBaseObject
{
public:
	CBulletObject();
	CBulletObject(bool bDivision, int iX, int iY);
	virtual ~CBulletObject();
	virtual bool Run();
	virtual void Render();
	void Move();
	void Collision();

private:
	bool m_Division;
};

#endif
#pragma once

#ifndef __CENEMYOBJECT__
#define __CENEMYOBJECT__

#include "CBaseObject.h"
class CEnemyObject :
	public CBaseObject
{
public:
	CEnemyObject();
	CEnemyObject(int iX, int iY);
	virtual ~CEnemyObject();
	virtual bool Run();
	virtual void Render();
	void Move();
	void FireReady(int level);
	void Fire();
	void Damaged();

private:
	int m_Hp;
	int m_MoveDir;
	int m_MoveCnt;

	bool m_FireFlag;
};

#endif
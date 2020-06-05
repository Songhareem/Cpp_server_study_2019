#pragma once

#ifndef __CPLAYEROBJECT__
#define __CPLAYEROBJECT__

#include "CBaseObject.h"

class CPlayerObject : public CBaseObject
{
public:
	enum {
		UP = 0x26,
		DOWN = 0x28,
		LEFT = 0x25,
		RIGHT = 0x27,
		Z = 0x5A,
		Y = 0x59,
		N = 0x4E,

		MAX_QUEUE = 20
	};

	CPlayerObject();
	virtual ~CPlayerObject();
	virtual bool Run();
	virtual void Render();
	void Move();
	void Fire();
	void Damaged();
	bool Enqueue(int Key);
	bool Dequeue(int *Reader);
	int DistanceX(int iX);
	int getHp() { return m_Hp; };

private:
	int m_Hp;

	int m_MsgQueue[MAX_QUEUE];
	int m_QueueFront;
	int m_QueueRear;
	int m_QueueLen;
	
	bool m_FireFlag;
};

#endif
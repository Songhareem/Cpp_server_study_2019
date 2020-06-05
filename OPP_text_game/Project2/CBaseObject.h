#pragma once

#ifndef __CBASEOBJECT__
#define __CBASEOBJECT__

class CBaseObject {

public:
	enum {
		PLAYER,
		ENEMY,
		BULLET,
		UI
	};

	CBaseObject();
	CBaseObject(int objType, int inX, int inY, bool bVisual = true);
	virtual ~CBaseObject();
	virtual bool Run() = 0;
	virtual void Render() = 0;
	int GetType() { return m_objectType; };
	int GetVisual() { return m_visual; };
	bool IsMatch(int iX, int iY);

protected:

	int m_x;
	int m_y;
	int m_objectType;
	bool m_visual;
};

#endif
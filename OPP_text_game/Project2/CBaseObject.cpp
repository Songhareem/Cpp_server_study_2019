#include"pch.h"
#include"CBaseObject.h"

CBaseObject::CBaseObject() {}
CBaseObject::CBaseObject(int objType, int inX, int inY, bool bVisual) : m_objectType(objType), m_x(inX), m_y(inY), m_visual(bVisual) {}
CBaseObject::~CBaseObject() {}

bool CBaseObject::IsMatch(int iX, int iY) {

	if (m_x == iX && m_y == iY) return true;

	return false;
}

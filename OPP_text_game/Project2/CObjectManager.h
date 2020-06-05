#pragma once

#ifndef __COBJECTMANAGER__
#define __COBJECTMANAGER__

#include "CBaseObject.h"
#include "CPlayerObject.h"
//#include <list>
#include "List.h"

//using namespace std;

class CObjectManager
{
public:

	//CList<CBaseObject *> ObjectList;
	list<CBaseObject *> ObjectList;
	CPlayerObject *Player;

	void CreateObject(int ObjectType, int iX = 0, int iY = 0, bool bDivision = 0);
	void DestroyObject(int ObjectType, int iX = 0, int iY = 0);
	void NonVisualDestroy();
	void DestroyAll();
	bool Run();
	void Render();
	static CObjectManager *GetInstance() { return &ObjectMng; };

private:

	static CObjectManager ObjectMng;
	
	CObjectManager();
	~CObjectManager();
};

#endif
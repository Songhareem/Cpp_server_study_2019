#include "pch.h"
#include "CBaseObject.h"
#include "CPlayerObject.h"
#include "CEnemyObject.h"
#include "CBulletObject.h"
#include "CUIObject.h"
#include "CObjectManager.h"
#include "CSceneManager.h"

CObjectManager::CObjectManager()
{
	Player = nullptr;
}

CObjectManager::~CObjectManager()
{
}

void CObjectManager::CreateObject(int ObjectType, int iX, int iY, bool bDivision) {

	CBaseObject *tmp = nullptr;

	switch (ObjectType) {

	case CBaseObject::PLAYER:
		Player = new CPlayerObject();
		tmp = Player;
		break;
	case CBaseObject::ENEMY:
		tmp = new CEnemyObject(iX, iY);
		break;
	case CBaseObject::BULLET:
		tmp = new CBulletObject(bDivision, iX, iY);
		break;
	case CBaseObject::UI:
		tmp = new CUIObject();
		break;
	}

	ObjectList.push_back(tmp);
}

void CObjectManager::DestroyObject(int ObjectType, int iX, int iY) {

	CBaseObject *tmp = nullptr;
	
	int type;
	bool matched;

	//CList<CBaseObject *>::iterator iter;
	list<CBaseObject *>::iterator iter;

	for (iter = ObjectList.begin(); iter != ObjectList.end(); ++iter) {

		tmp = *iter;
		type = (*iter)->GetType();
		matched = (*iter)->IsMatch(iX, iY);
		if (type == ObjectType && matched == true) {

			delete tmp;
			iter = ObjectList.erase(iter);
		}
	}
}

void  CObjectManager::NonVisualDestroy() {

	CBaseObject *pBO;
	bool Visual;

	//CList<CBaseObject *>::iterator iter;
	list<CBaseObject *>::iterator iter;
	for (iter = ObjectList.begin(); iter != ObjectList.end();) {

		pBO = (*iter);
		Visual = pBO->GetVisual();
		if (Visual == false) {
		
			delete pBO;
			iter = ObjectList.erase(iter);
		}
		else ++iter;
	}
}

void CObjectManager::DestroyAll() {

	CBaseObject *pBO;
	list<CBaseObject *>::iterator iter;
	for (iter = ObjectList.begin(); iter != ObjectList.end(); ++iter) {
		
		pBO = *iter;
		delete pBO;
	}

	ObjectList.clear();
}


bool CObjectManager::Run() {
	
	CBaseObject *pBO;
	//CList<CBaseObject *>::iterator iter;
	list<CBaseObject *>::iterator iter;
	
	for (iter = ObjectList.begin(); iter != ObjectList.end(); ++iter) {

		pBO = *iter;
		pBO->Run();
	}

	return true;
}

void CObjectManager::Render() {

	CBaseObject *pBO;
	//CList<CBaseObject *>::iterator iter;
	list<CBaseObject *>::iterator iter;
	
	for (iter = ObjectList.begin(); iter != ObjectList.end(); ++iter) {

		pBO = *iter;
		pBO->Render();
	}
}
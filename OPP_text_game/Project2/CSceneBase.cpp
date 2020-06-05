
#include "pch.h"
#include "CSceneBase.h"
#include "CObjectManager.h"

CSceneBase::CSceneBase() {

	system("mode con: cols=38 lines=14");
}

CSceneBase::~CSceneBase() {

	CObjectManager::GetInstance()->ObjectList.clear();
}

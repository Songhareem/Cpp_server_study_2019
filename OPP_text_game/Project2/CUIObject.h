#pragma once

#ifndef __CUIOBJECT__
#define __CUIOBJECT__

#include "CBaseObject.h"
#include "CSceneBase.h"

class CUIObject :
	public CBaseObject
{
public:

	CUIObject();
	~CUIObject();
	virtual bool Run();
	virtual void Render();
	void TitleRun();
	void GameRun();
	void EndRun();
	void MainRender();
	void SubRender();

private:
	char m_Buff[CSceneBase::MAIN_HEIGHT][CSceneBase::MAIN_WIDTH];
};

#endif
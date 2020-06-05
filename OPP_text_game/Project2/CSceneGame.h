#pragma once

#ifndef __CSCENEGAME__
#define __CSCENEGAME__

#include "CSceneBase.h"

class CSceneGame :
	public CSceneBase
{
public:

	CSceneGame();
	virtual ~CSceneGame();
	void ObjectInit();
	virtual void Run();
	virtual void InputKey();
	void GameOver();
};

#endif
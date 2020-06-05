#pragma once

#ifndef __CSCENEEND__
#define __CSCENEEND__

#include "CSceneBase.h"
class CSceneEnd :
	public CSceneBase
{
public:
	enum {
		WIDTH = 38,
		HEIGHT = 14
	};

	CSceneEnd();
	virtual ~CSceneEnd();
	virtual void Run();
	virtual void InputKey();

};

#endif
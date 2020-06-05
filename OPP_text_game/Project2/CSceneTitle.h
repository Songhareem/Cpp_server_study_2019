#pragma once

#ifndef __CSCENETITLE__
#define __CSCENETITLE__

#include "CSceneBase.h"

class CSceneTitle :
	public CSceneBase
{
public:

	CSceneTitle();
	virtual ~CSceneTitle();
	virtual void Run();
	virtual void InputKey();

};

#endif
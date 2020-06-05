#pragma once

#ifndef __CSCENEBASE__
#define __CSCENEBASE__

class CSceneBase
{
public:

	enum {

		MAIN_WIDTH = 127,
		MAIN_HEIGHT = 32,
		SUB_WIDTH = 38,
		SUB_HEIGHT = 14
	};

	CSceneBase();
	virtual ~CSceneBase();
	virtual void Run()=0;
	virtual void InputKey()=0;

};

#endif
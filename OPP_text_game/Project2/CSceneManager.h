#pragma once

#ifndef __CSCENEMANAGER__
#define __CSCENEMANAGER__

#include"CSceneBase.h"

class CSceneManager
{
public:
	enum {
		TITLE,
		GAME,
		END,
		DONE
	};
	
	bool Run();
	void LoadScene(int sequence);
	int GetLevel();
	int GetScore();
	int GetSequence();
	bool GetNeedChange();
	void LevelUp(); 
	void ScoreUp(); 
	void LevelReset() { m_level = 1; };
	void ScoreReset() { m_score = 0; };
	static CSceneManager *GetInstance() { return &SceneMng; };
	
private:
	int m_level;
	int m_score;
	int m_sequenceNow;
	bool m_needChange;

	CSceneBase * _pScene;
	static CSceneManager SceneMng;
	
	CSceneManager();
	~CSceneManager();
};

#endif
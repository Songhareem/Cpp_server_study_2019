#include "pch.h"
#include "CSceneBase.h"
#include "CSceneTitle.h"
#include "CSceneGame.h"
#include "CSceneEnd.h"
#include "CObjectManager.h"
#include "CSceneManager.h"

CSceneManager::CSceneManager() : m_level(1), m_score(0), m_sequenceNow(TITLE), m_needChange(false), _pScene(new CSceneTitle)
{
	//init �⺻��ü ���� �� �ʱ�ȭ �ε� ���
	//std::cout << "ONCE" << std::endl;
}

CSceneManager::~CSceneManager()
{
	// �����Ҵ� ��� �� ����
	delete _pScene;
}

bool CSceneManager::Run() {

	_pScene->Run();
	if (m_needChange == true) {

		if (m_sequenceNow == DONE) return true;

		delete _pScene;

		switch (m_sequenceNow)
		{
		case TITLE:
			_pScene = new CSceneTitle;
			break;
		case GAME:
			_pScene = new CSceneGame;
			break;
		case END:
			_pScene = new CSceneEnd;
			break;
		default:
			break;
		}

		m_needChange = false;
	}

	return false;
}

void CSceneManager::LoadScene(int sequence) 
{
	m_sequenceNow = sequence;
	m_needChange = true;
}

int CSceneManager::GetLevel() { return m_level; };
int CSceneManager::GetScore() { return m_score; };
int CSceneManager::GetSequence() { return m_sequenceNow; };
bool CSceneManager::GetNeedChange() { return m_needChange; };
void CSceneManager::LevelUp() { m_level++; };
void CSceneManager::ScoreUp() { m_score += m_level*100; };
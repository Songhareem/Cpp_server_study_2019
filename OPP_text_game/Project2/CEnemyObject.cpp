#include "pch.h"
#include "CBaseObject.h"
#include "CEnemyObject.h"
#include "CObjectManager.h"
#include "CSceneManager.h"
#include "CScreenBuffer.h"

CEnemyObject::CEnemyObject()
{
	m_objectType = ENEMY;

	m_Hp = 1;
	m_MoveDir = 1;
	m_MoveCnt = 0;
	m_FireFlag = false;
}

CEnemyObject::CEnemyObject(int iX, int iY) : CBaseObject(CBaseObject::ENEMY, iX, iY) {

	m_Hp = 1;
	m_MoveDir = 1;
	m_MoveCnt = 0;
	m_FireFlag = false;
}

CEnemyObject::~CEnemyObject()
{
}

bool CEnemyObject::Run() {
	
	// 활성화 상태가 아니라면 처리에서 제외
	if (m_visual != true) return false;

	Move();

	FireReady(CSceneManager::GetInstance()->GetLevel());

	Fire();

	return true;
}

void CEnemyObject::Render() {

	if (m_visual != true) return;

	CScreenBuffer::GetInstance()->Render(ENEMY, m_y, m_x);
}

void CEnemyObject::Move() {

	int origin_x = this->m_x;
	int change_x = this->m_x;
	
	change_x = origin_x + m_MoveDir;
	m_MoveCnt += 1;

	if (m_MoveCnt > 10) {

		m_MoveDir = m_MoveDir * -1;
		m_MoveCnt = 0;
	}

	this->m_x = change_x;
}

void CEnemyObject::FireReady(int level) {

	int random = rand() % 5000 + 1;

	// init 100 / 5000 = 1/50
	if (random < level * 50) {

		m_FireFlag = true;
	}
}

void CEnemyObject::Fire() {

	if (m_FireFlag == true) {
		
		// 총알 생성자 호출
		CObjectManager::GetInstance()->CreateObject(CBaseObject::BULLET, m_x, m_y + 1, false);
		m_FireFlag = false;
	}
}

void CEnemyObject::Damaged() {

	if (m_visual != true) return;

	m_Hp -= 1;
	
	if (m_Hp == 0) m_visual = false;

	//scoreUp

	CSceneManager::GetInstance()->ScoreUp();
}
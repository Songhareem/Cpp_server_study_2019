#include "pch.h"
#include "CUIObject.h"
#include "CSceneTitle.h"
#include "CSceneGame.h"
#include "CSceneEnd.h"
#include "CObjectManager.h"
#include "CSceneManager.h"
#include "CScreenBuffer.h"


CUIObject::CUIObject()
{
	for (int y = 0; y < CSceneBase::MAIN_HEIGHT; y++) {
		for (int x = 0; x < CSceneBase::MAIN_WIDTH; x++) {
			m_Buff[y][x] = ' ';
		}
	}

	m_y = 0;
	m_x = 0;
	m_objectType = CBaseObject::UI;
}

CUIObject::~CUIObject()
{
}

bool CUIObject::Run() {
	
	int sequence = CSceneManager::GetInstance()->GetSequence();

	if (CSceneManager::GetInstance()->GetNeedChange() == true) return false;

	switch (sequence) {

	case CSceneManager::TITLE:
		TitleRun();
		break;
	case CSceneManager::GAME:
		GameRun();
		break;
	case CSceneManager::END:
		EndRun();
		break;
	}

	return true;
}

void CUIObject::Render() {

	int sequence = CSceneManager::GetInstance()->GetSequence();

	if (CSceneManager::GetInstance()->GetNeedChange() == true) return;

	switch (sequence) {

	case CSceneManager::TITLE:
	    SubRender();
		break;
	case CSceneManager::GAME:
		MainRender();
		break;
	case CSceneManager::END:
		SubRender();
		break;
	}
}

void CUIObject::TitleRun() {

	static int Cnt = 0;
	char Press[21] = "press Z key for play";
	
	char Title[CSceneBase::SUB_HEIGHT][CSceneBase::SUB_WIDTH] = { "=====================================",	// 0
																  "!                                   !",	// 1
																  "!        < SHOOTING GAME >          !",	//2
																  "!                                   !",	// 3
																  "!      Control                      !",	// 4
																  "!      Up    key : move up          !",	// 5
																  "!      Down  key : move down        !",	// 6
																  "!      Left  key : move left        !",	// 7
																  "!      Right key : move right       !",	// 8
																  "!      Z     key : shoot bullet     !",	// 9
																  "!                                   !",	// 10
																  "!                                   !",  // 11
																  "!                                   !",	// 12
																  "=====================================" };
	Cnt++;
	if (Cnt <= 5) {

		// NULL 제외
		for (int i = 0; Press[i] != NULL; i++) Title[11][7 + i] = Press[i];
	}

	if (Cnt >= 10) Cnt = 0;

	for (int y = 0; y < CSceneBase::SUB_HEIGHT; y++) {
		for (int x = 0; x < CSceneBase::SUB_WIDTH; x++) {
			m_Buff[y][x] = Title[y][x];
		}
	}
}

void CUIObject::GameRun() {

	char Stage[10] = { ' ', };
	char Score[20] = { ' ', };
	char PlayerHp[15] = { ' ', };

	sprintf_s(Stage, "Stage : %d", CSceneManager::GetInstance()->GetLevel());
	sprintf_s(Score, "Score : %d", CSceneManager::GetInstance()->GetScore());
	sprintf_s(PlayerHp, "Player HP : %d", CObjectManager::GetInstance()->Player->getHp());

	for (int i = 0; i < 10; i++) {
		if (Stage[i] == NULL) break;
		m_Buff[0][3 + i] = Stage[i];
	}
	for (int i = 0; i < 20; i++) {
		if (Score[i] == NULL) break;
		m_Buff[0][17 + i] = Score[i];
	}
	for (int i = 0; i < 15; i++) {
		if (PlayerHp[i] == NULL) break;
		m_Buff[CSceneBase::MAIN_HEIGHT - 1][3 + i] = PlayerHp[i];
	}
}

void CUIObject::EndRun() {

	static int Cnt = 0;
	char LastLevel[21];
	char Score[21];
	char Press[21] = "One more play? [Y/N]";
	
	char End[CSceneBase::SUB_HEIGHT][CSceneBase::SUB_WIDTH] = { "=====================================",	// 0
																"!                                   !",	// 1
																"!           < GAME OVER >           !",	// 2
																"!                                   !",	// 3
																"!                                   !",	// 4
																"!                                   !",	// 5
																"!                                   !",	// 6
																"!                                   !",	// 7
																"!                                   !",	// 8
																"!                                   !",	// 9
																"!                                   !",	// 10
																"!                                   !",    // 11
																"!                                   !",	// 12
																"=====================================" };	// 13
	
	sprintf_s(LastLevel,"Last level : %d", CSceneManager::GetInstance()->GetLevel());
	sprintf_s(Score, "Score      : %d", CSceneManager::GetInstance()->GetScore());
	
	// NULL 제외
	for (int i = 0; LastLevel[i] != NULL; i++)  End[5][8 + i] = LastLevel[i];
	for (int i = 0; Score[i] != NULL; i++)		End[7][8 + i] = Score[i];

	Cnt++;
	if (Cnt <= 5) {

		for (int i = 0; Press[i] != NULL; i++) End[10][8 + i] = Press[i];
	}

	if (Cnt >= 10) Cnt = 0;

	for (int y = 0; y < CSceneBase::SUB_HEIGHT; y++) {
		for (int x = 0; x < CSceneBase::SUB_WIDTH; x++) {
			m_Buff[y][x] = End[y][x];
		}
	}
}

void CUIObject::MainRender() {

	bool NonData = false;

	for (int y = 0; y < CSceneBase::MAIN_HEIGHT; y++) {

		for (int x = 0; x < CSceneBase::MAIN_WIDTH; x++) {

			if (m_Buff[y][x] == ' ') continue;

			CScreenBuffer::GetInstance()->Render(CBaseObject::UI, y, x, NonData, m_Buff[y][x]);
		}
	}
}

void CUIObject::SubRender() {

	bool NonData = false;

	for (int y = 0; y < CSceneBase::SUB_HEIGHT; y++) {

		for (int x = 0; x < CSceneBase::SUB_WIDTH; x++) {

			if (m_Buff[y][x] == ' ') continue;

			CScreenBuffer::GetInstance()->Render(CBaseObject::UI, y, x, NonData, m_Buff[y][x]);
		}
	}
}

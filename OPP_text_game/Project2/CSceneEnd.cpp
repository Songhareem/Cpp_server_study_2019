#include "pch.h"
#include "CSceneBase.h"
#include "CSceneEnd.h"
#include "CObjectManager.h"
#include "CSceneManager.h"
#include "CScreenBuffer.h"

CSceneEnd::CSceneEnd()
{
	// init 기본객체 생성 및 초기화 로딩 등등
	// 콘솔창 크기 변경
	system("mode con: cols=38 lines=14");
	// 게임 시작 시간 체크
	// Object 할당
	CObjectManager::GetInstance()->CreateObject(CBaseObject::UI);
}


CSceneEnd::~CSceneEnd()
{
	// 동적할당 멤버 다 삭제
	CObjectManager::GetInstance()->DestroyAll();
	
	// 게임 레벨 및 점수 리셋
	CSceneManager::GetInstance()->LevelReset();
	CSceneManager::GetInstance()->ScoreReset();
}

void CSceneEnd::Run() 
{
	//*/
	// 버퍼 비우기
	CScreenBuffer::GetInstance()->Clear();
	// 키보드처리
	InputKey();

	// Object 순회 액션
	CObjectManager::GetInstance()->Run();

	// Obhect 순회 랜더
	CObjectManager::GetInstance()->Render();

	// 플립
	CScreenBuffer::GetInstance()->Flip(CSceneBase::SUB_HEIGHT);
	//*/
}

void CSceneEnd::InputKey()
{
	//  Y버튼
	if (GetKeyState(CPlayerObject::Y) & 0x8000) {

		// 게임 화면으로 전환
		CSceneManager::GetInstance()->LoadScene(CSceneManager::TITLE);
	}

	else if (GetKeyState(CPlayerObject::N) & 0x8000) {

		// 게임 종료
		CSceneManager::GetInstance()->LoadScene(CSceneManager::DONE);
	}
}

#include "pch.h"
#include <Windows.h>
#include "CBaseObject.h"
#include "CSceneBase.h"
#include "CSceneTitle.h"
#include "CObjectManager.h"
#include "CSceneManager.h"
#include "CScreenBuffer.h"

using namespace std;

CSceneTitle::CSceneTitle()
{
	// init 기본객체 생성 및 초기화 로딩 등등
	// 콘솔창 크기 변경
	system("mode con: cols=80 lines=23");
	// 게임 시작 시간 체크
	// Object 할당
	CObjectManager::GetInstance()->CreateObject(CBaseObject::UI);
}

CSceneTitle::~CSceneTitle()
{
	cout << "Title destroy" << endl;
	CObjectManager::GetInstance()->DestroyAll();
}

void CSceneTitle::Run() 
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

void CSceneTitle::InputKey() {

	// Z 버튼
	if (GetKeyState(CPlayerObject::Z) & 0x8000) {
	
		CSceneManager::GetInstance()->LoadScene(CSceneManager::GAME);
	}
}

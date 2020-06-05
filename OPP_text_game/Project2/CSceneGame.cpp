#include "pch.h"
#include <Windows.h>
#include "CSceneBase.h"
#include "CSceneGame.h"
#include "CObjectManager.h"
#include "CSceneManager.h"
#include "CScreenBuffer.h"

CSceneGame::CSceneGame()
{
	// init �⺻��ü ���� �� �ʱ�ȭ �ε� ���
	// �ܼ�â �ʱ�ȭ
	system("mode con: cols=127 lines=32");
	
	ObjectInit();
}

CSceneGame::~CSceneGame()
{
	// �����Ҵ� ��� �� ����
	CObjectManager::GetInstance()->DestroyAll();
}

void CSceneGame::ObjectInit() {

	// Game�� �ʿ��� Object ����
	CObjectManager::GetInstance()->CreateObject(CBaseObject::PLAYER);
	
	for (int x = 8; x < MAIN_WIDTH - 15; x += 16) 
		CObjectManager::GetInstance()->CreateObject(CBaseObject::ENEMY, x, 4);
	
	for (int x = 16; x < MAIN_WIDTH - 15; x += 16) 
		CObjectManager::GetInstance()->CreateObject(CBaseObject::ENEMY, x, 7);
	
	for (int x = 8; x < MAIN_WIDTH - 15; x += 16) 
		CObjectManager::GetInstance()->CreateObject(CBaseObject::ENEMY, x, 10);
	
	CObjectManager::GetInstance()->CreateObject(CBaseObject::UI);
}

void CSceneGame::Run() 
{
	// ���� Ŭ����
	CScreenBuffer::GetInstance()->Clear();
	// Ű����ó��
	InputKey();
	// Object ��ȸ �׼�
	CObjectManager::GetInstance()->Run();
	// Obhect ��ȸ ����
	CObjectManager::GetInstance()->Render();
	// �ø�
	CScreenBuffer::GetInstance()->Flip();
	// ���ʿ� ��� ����, ���� ���� ����
	CObjectManager::GetInstance()->NonVisualDestroy();
	GameOver();
}

void CSceneGame::InputKey()
{
	if (CObjectManager::GetInstance()->Player == nullptr) return;

	if (GetKeyState(CPlayerObject::UP) & 0x8000) {

		CObjectManager::GetInstance()->Player->Enqueue(CPlayerObject::UP);
	}
	if (GetKeyState(CPlayerObject::DOWN) & 0x8000) {

		CObjectManager::GetInstance()->Player->Enqueue(CPlayerObject::DOWN);
	}
	if (GetKeyState(CPlayerObject::LEFT) & 0x8000) {

		CObjectManager::GetInstance()->Player->Enqueue(CPlayerObject::LEFT);
	}
	if (GetKeyState(CPlayerObject::RIGHT) & 0x8000) {

		CObjectManager::GetInstance()->Player->Enqueue(CPlayerObject::RIGHT);
	}
	if (GetKeyState(CPlayerObject::Z) & 0x8000) {

		CObjectManager::GetInstance()->Player->Enqueue(CPlayerObject::Z);
	}
}

void CSceneGame::GameOver() {

	//CList<CBaseObject *>::iterator iter;
	list<CBaseObject *>::iterator iter = CObjectManager::GetInstance()->ObjectList.begin();

	int CntEnemy = 0;
	int CntPlayer = 0;
	int Type;
	
	CBaseObject *pBO = nullptr;
	for (; iter != CObjectManager::GetInstance()->ObjectList.end(); ++iter) {

		pBO = (*iter);
		Type = pBO->GetType();
		if (Type == CBaseObject::ENEMY)  CntEnemy++;
		if (Type == CBaseObject::PLAYER) CntPlayer++;
	}

	if (CntPlayer == 0) {

		CSceneManager::GetInstance()->LoadScene(CSceneManager::END);
	}

	if (CntEnemy == 0) {

		CObjectManager::GetInstance()->DestroyAll();
		CSceneManager::GetInstance()->LevelUp();
		ObjectInit();
	}
}
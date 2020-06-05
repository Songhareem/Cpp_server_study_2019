#include "pch.h"
#include "CSceneBase.h"
#include "CSceneEnd.h"
#include "CObjectManager.h"
#include "CSceneManager.h"
#include "CScreenBuffer.h"

CSceneEnd::CSceneEnd()
{
	// init �⺻��ü ���� �� �ʱ�ȭ �ε� ���
	// �ܼ�â ũ�� ����
	system("mode con: cols=38 lines=14");
	// ���� ���� �ð� üũ
	// Object �Ҵ�
	CObjectManager::GetInstance()->CreateObject(CBaseObject::UI);
}


CSceneEnd::~CSceneEnd()
{
	// �����Ҵ� ��� �� ����
	CObjectManager::GetInstance()->DestroyAll();
	
	// ���� ���� �� ���� ����
	CSceneManager::GetInstance()->LevelReset();
	CSceneManager::GetInstance()->ScoreReset();
}

void CSceneEnd::Run() 
{
	//*/
	// ���� ����
	CScreenBuffer::GetInstance()->Clear();
	// Ű����ó��
	InputKey();

	// Object ��ȸ �׼�
	CObjectManager::GetInstance()->Run();

	// Obhect ��ȸ ����
	CObjectManager::GetInstance()->Render();

	// �ø�
	CScreenBuffer::GetInstance()->Flip(CSceneBase::SUB_HEIGHT);
	//*/
}

void CSceneEnd::InputKey()
{
	//  Y��ư
	if (GetKeyState(CPlayerObject::Y) & 0x8000) {

		// ���� ȭ������ ��ȯ
		CSceneManager::GetInstance()->LoadScene(CSceneManager::TITLE);
	}

	else if (GetKeyState(CPlayerObject::N) & 0x8000) {

		// ���� ����
		CSceneManager::GetInstance()->LoadScene(CSceneManager::DONE);
	}
}

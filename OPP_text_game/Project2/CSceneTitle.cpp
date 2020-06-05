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
	// init �⺻��ü ���� �� �ʱ�ȭ �ε� ���
	// �ܼ�â ũ�� ����
	system("mode con: cols=80 lines=23");
	// ���� ���� �ð� üũ
	// Object �Ҵ�
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

void CSceneTitle::InputKey() {

	// Z ��ư
	if (GetKeyState(CPlayerObject::Z) & 0x8000) {
	
		CSceneManager::GetInstance()->LoadScene(CSceneManager::GAME);
	}
}

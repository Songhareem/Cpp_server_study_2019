#include "pch.h"
#include "CBaseObject.h"
#include "CEnemyObject.h"
#include "CBulletObject.h"
#include "CObjectManager.h"
#include "CScreenBuffer.h"

CBulletObject::CBulletObject()
{
}

CBulletObject::CBulletObject(bool bDivision, int iX, int iY) : CBaseObject(CBaseObject::BULLET, iX, iY) {

	m_Division = bDivision;
}

CBulletObject::~CBulletObject()
{
}

bool CBulletObject::Run() {
	
	// Ȱ��ȭ ���°� �ƴ϶�� ó������ ����
	if (m_visual != true) return false;

	Move();

	Collision();

	return true;
}

void CBulletObject::Render() {

	// Ȱ��ȭ ���°� �ƴ϶�� ó������ ����
	if (m_visual != true) return;

	if (m_Division == true) {
		CScreenBuffer::GetInstance()->Render(BULLET, m_y, m_x, true);
	}
	else if (m_Division == false){
		CScreenBuffer::GetInstance()->Render(BULLET, m_y, m_x, false);
	}
}

void CBulletObject::Move() {

	short origin_x = this->m_x;
	short origin_y = this->m_y;
	short change_x = this->m_x;
	short change_y = this->m_y;

	int dis_x = CObjectManager::GetInstance()->Player->DistanceX(origin_x);

	// �Ѿ� �̵�
	switch (m_Division) {

	case FALSE:
		if		(dis_x == 2) printf("Distance ERROR (Player Bullet)\n");
		else if (dis_x != 0) {

			if		(dis_x ==  1) change_x += 1;
			else if (dis_x == -1) change_x -= 1;
		}

		change_y += 1;
		break;

	case TRUE:
		change_y -= 1;
		break;
	}

	// �ٲ� x,y��ǥ �ݿ�
	this->m_x = change_x;
	this->m_y = change_y;
}

void CBulletObject::Collision() {
	
	//*/
	if (this->m_y <= 0 || this->m_y >= CScreenBuffer::HEIGHT - 1 || this->m_x <= 0 || this->m_x >= CScreenBuffer::WIDTH - 3) {

		// �Ѿ� ���� or ���� f��ag on
		m_visual = false;
		return;
	}
	
	//CList<CBaseObject *>::iterator iter = pOM->ObjectList.begin();
	list<CBaseObject *>::iterator iter = CObjectManager::GetInstance()->ObjectList.begin();
	CEnemyObject *pEO = nullptr;
	
	// �Ǿ� �Ѿ˿� ���� �浹üũ
	switch (m_Division) {

	case TRUE:	// �Ѿ��� �Ʊ��� ��
		
		// ������Ʈ ����Ʈ ��ȸ �� ���� ��ġ�� �Ѿ��� ��ġ�� ���ٸ�,
		for (; iter != CObjectManager::GetInstance()->ObjectList.end(); ++iter) {
			//*/
			pEO = (CEnemyObject *)(*iter);
			int Type = pEO->GetType();
			if (Type == CBaseObject::ENEMY && pEO->IsMatch(m_x,m_y) == true) {
				
				// �� ��ü ������
				pEO->Damaged();
				
				// �Ѿ� ��ü ���� or ���� flag on
				m_visual = false;
				break;
			}
			//*/
		}
		break;

	case FALSE:	// �Ѿ��� ������ ��
		// ������Ʈ ����Ʈ ��ȸ �� �÷��̾��� ��ġ�� �Ѿ��� ��ġ�� ���ٸ�,
		//*/
		if (CObjectManager::GetInstance()->Player != nullptr && CObjectManager::GetInstance()->Player->IsMatch(m_x, m_y) == true) {

			// �� ��ü ������
			CObjectManager::GetInstance()->Player->Damaged();

			// �Ѿ� ��ü ���� or ���� flag on
			m_visual = false;
		}
		//*/
		break;
	}
	//*/
}
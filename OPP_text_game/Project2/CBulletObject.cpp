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
	
	// 활성화 상태가 아니라면 처리에서 제외
	if (m_visual != true) return false;

	Move();

	Collision();

	return true;
}

void CBulletObject::Render() {

	// 활성화 상태가 아니라면 처리에서 제외
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

	// 총알 이동
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

	// 바뀐 x,y좌표 반영
	this->m_x = change_x;
	this->m_y = change_y;
}

void CBulletObject::Collision() {
	
	//*/
	if (this->m_y <= 0 || this->m_y >= CScreenBuffer::HEIGHT - 1 || this->m_x <= 0 || this->m_x >= CScreenBuffer::WIDTH - 3) {

		// 총알 삭제 or 삭제 fㅣag on
		m_visual = false;
		return;
	}
	
	//CList<CBaseObject *>::iterator iter = pOM->ObjectList.begin();
	list<CBaseObject *>::iterator iter = CObjectManager::GetInstance()->ObjectList.begin();
	CEnemyObject *pEO = nullptr;
	
	// 피아 총알에 따른 충돌체크
	switch (m_Division) {

	case TRUE:	// 총알이 아군의 것
		
		// 오브젝트 리스트 순회 중 적의 위치와 총알의 위치가 같다면,
		for (; iter != CObjectManager::GetInstance()->ObjectList.end(); ++iter) {
			//*/
			pEO = (CEnemyObject *)(*iter);
			int Type = pEO->GetType();
			if (Type == CBaseObject::ENEMY && pEO->IsMatch(m_x,m_y) == true) {
				
				// 적 객체 데미지
				pEO->Damaged();
				
				// 총알 객체 삭제 or 삭제 flag on
				m_visual = false;
				break;
			}
			//*/
		}
		break;

	case FALSE:	// 총알이 적군의 것
		// 오브젝트 리스트 순회 중 플레이어의 위치와 총알의 위치가 같다면,
		//*/
		if (CObjectManager::GetInstance()->Player != nullptr && CObjectManager::GetInstance()->Player->IsMatch(m_x, m_y) == true) {

			// 적 객체 데미지
			CObjectManager::GetInstance()->Player->Damaged();

			// 총알 객체 삭제 or 삭제 flag on
			m_visual = false;
		}
		//*/
		break;
	}
	//*/
}
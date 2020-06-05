#include "pch.h"
#include "CBaseObject.h"
#include "CPlayerObject.h"
#include "CSceneBase.h"
#include "CObjectManager.h"
#include "CScreenBuffer.h"


CPlayerObject::CPlayerObject() : CBaseObject(CBaseObject::PLAYER, CScreenBuffer::WIDTH / 2, m_y = CScreenBuffer::HEIGHT / 5 * 4)
{
	m_Hp = 3;
	
	m_FireFlag = false;

	m_QueueFront=1;
	m_QueueRear=1;
	m_QueueLen=0;
}

CPlayerObject::~CPlayerObject()
{

}

bool CPlayerObject::Run() {

	// Ȱ��ȭ ���°� �ƴ϶�� ó������ ����
	if (m_visual != true) return false;

	// Ű���� �Է��� ���Ҵٸ� Move
	Move();

	// �߻� �÷��װ� on �̶�� Fire
	Fire();

	return true;
}

void CPlayerObject::Render() {

	// Ȱ��ȭ ���°� �ƴ϶�� ó������ ����
	if (m_visual != true) return;

	CScreenBuffer::GetInstance()->Render(PLAYER, m_y, m_x);
}

void CPlayerObject::Move() {

	int origin_x = this->m_x;
	int origin_y = this->m_y;
	int change_x = this->m_x;
	int change_y = this->m_y;

	int QueueReader;

	while (m_QueueLen > 0) {

		Dequeue(&QueueReader);

		switch (QueueReader) {

		case UP:
			if (origin_y > 1) change_y--;
			break;
		case DOWN:
			if (origin_y < CScreenBuffer::HEIGHT - 2) change_y++;
			break;
		case LEFT:
			if (origin_x > 1) change_x -= 2;
			break;
		case RIGHT:
			if (origin_x < CScreenBuffer::WIDTH - 4) change_x += 2;
			break;
		case Z:
			m_FireFlag = true;
			break;
		}
	}
	
	this->m_x = change_x;
	this->m_y = change_y;
}

void CPlayerObject::Fire() {

	if (m_FireFlag == true) {

		CObjectManager::GetInstance()->CreateObject(CBaseObject::BULLET, m_x, m_y - 1, true);

		// �Ѿ� ������
		m_FireFlag = false;
	}
}

void CPlayerObject::Damaged() {

	if (m_visual != true) return;

	m_Hp -= 1;
	
	if (m_Hp == 0) m_visual = false;
}

bool CPlayerObject::Enqueue(int Key) {

	//std::cout << Key << std::endl;

	// ť�� ���� á���� üũ
	int check = m_QueueRear;

	if (++check >= MAX_QUEUE) check = 0;
	if (check == m_QueueFront) return false;

	// ������ ����
	m_MsgQueue[m_QueueRear] = Key;
	m_QueueRear++;
	if (m_QueueRear >= MAX_QUEUE) m_QueueRear = 0;

	// �޽��� ť++ ����
	m_QueueLen++;

	return true;
}

bool CPlayerObject::Dequeue(int *Reader) {

	// ť�� ������� üũ
	if (m_QueueFront == m_QueueRear) return false;

	// ������ �б�
	*Reader = m_MsgQueue[m_QueueFront];
	m_QueueFront++;
	if (m_QueueFront >= MAX_QUEUE) m_QueueFront = 0;

	// �޽��� ť-- ����
	m_QueueLen--;

	return true;
}

int CPlayerObject::DistanceX(int iX) {

	if		(m_x == iX) return 0;
	else if (m_x > iX)  return 1;
	else if (m_x < iX)  return -1;

	return 2;
}
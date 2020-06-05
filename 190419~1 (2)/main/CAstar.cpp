// ���ø� ���� ���� : https://www.ikpil.com/1020

#include "stdafx.h"
#include "CAstar.h"

//public
CAstar::CAstar()
{
	NodeInit();
}


CAstar::~CAstar()
{
}

void CAstar::ClearWall() {

	for (int Y = 0; Y < FILD_Y; Y++) {
		for (int X = 0; X < FILD_X; X++) {
			
			if (NodeFild[Y][X].iColor == GRAY) {

				NodeFild[Y][X].iColor = WHITE;
				NodeFild[Y][X].isWall = false;
			}
		}
	}
}

void CAstar::WallUp(int iX, int iY) {

	int NodeX = iX;
	int NodeY = iY;

	GetNodePos(&NodeX, &NodeY);

	// ����̾�� ĥ�� �� ����
	if (NodeFild[NodeY][NodeX].iColor == WHITE) {

		// ȸ������ ĥ�ϰ� �� �Ӽ� ��ȭ
		NodeFild[NodeY][NodeX].iColor = GRAY;
		NodeFild[NodeY][NodeX].isWall = true;
	}
}

void CAstar::WallDown(int iX, int iY) {

	int NodeX = iX;
	int NodeY = iY;

	GetNodePos(&NodeX, &NodeY);

	// ȸ���̾�� ĥ�� �� ����
	if (NodeFild[NodeY][NodeX].iColor == GRAY) {

		// ������� ĥ�ϰ� �� �Ӽ� ��ȭ
		NodeFild[NodeY][NodeX].iColor = WHITE;
		NodeFild[NodeY][NodeX].isWall = false;
	}
}

void CAstar::MoveStartNode(int iX, int iY) {

	int NodeX = iX;
	int NodeY = iY;
	
	GetNodePos(&NodeX, &NodeY);

	// ���� ������ ��� �� ����
	pStartNode->iColor = WHITE;
	
	// ��ŸƮ ������ �ű��
	pStartNode = &NodeFild[NodeY][NodeX];
	pStartNode->iColor = GREEN;
	pStartNode->isWall = false;

	// ��ŸƮ ������ H�� ����
	pStartNode->H = GetDistance(pStartNode->iX, pStartNode->iY, pEndNode->iX, pEndNode->iY);
}

void CAstar::MoveEndNode(int iX, int iY) {

	int NodeX = iX;
	int NodeY = iY;

	GetNodePos(&NodeX, &NodeY);

	// ���� ������ ��� �� ����
	pEndNode->iColor = WHITE;

	// ��ŸƮ ������ �ű��
	pEndNode = &NodeFild[NodeY][NodeX];
	pEndNode->iColor = RED;
	pEndNode->isWall = false;

	// ��ŸƮ ������ H�� ����
	pStartNode->H = GetDistance(pStartNode->iX, pStartNode->iY, pEndNode->iX, pEndNode->iY);
}

bool CAstar::GetNodeRect(int NodeX, int NodeY, int &LeftX, int &RightX, int &TopY, int &BottomY) {

	// �迭 ���� ���� X �� Y�� ���´ٸ� false ����
	if (NodeX >= FILD_X || NodeY >= FILD_Y)
		return false;

	LeftX = NodeFild[NodeY][NodeX].iLeftX;
	RightX = NodeFild[NodeY][NodeX].iRightX;
	TopY = NodeFild[NodeY][NodeX].iTopY;
	BottomY = NodeFild[NodeY][NodeX].iBottomY;

	return true;
}

void CAstar::GetNodePos(int *iX, int *iY) {

	int PosX = *iX / FILD_SIZE;
	int PosY = *iY / FILD_SIZE;

	*iX = PosX;
	*iY = PosY;
}

int CAstar::GetDistance(int strX, int strY, int endX, int endY) {

	int distance = abs(endX - strX) + abs(endY - strY);

	return distance;
}

void CAstar::NodeInit() {

	int X = 0;
	int Y = 0;

	// ��� ���(�ʵ�) �ʱⰪ ����
	for (Y = 0; Y < FILD_Y; Y++) {
		for (X = 0; X < FILD_X; X++) {
			NodeFild[Y][X].iX = X;
			NodeFild[Y][X].iY = Y;
			NodeFild[Y][X].iLeftX = 0 + (FILD_SIZE * X);
			NodeFild[Y][X].iRightX = NodeFild[Y][X].iLeftX + FILD_SIZE;
			NodeFild[Y][X].iTopY = 0 + (FILD_SIZE * Y);
			NodeFild[Y][X].iBottomY = NodeFild[Y][X].iTopY + FILD_SIZE;
			NodeFild[Y][X].iColor = WHITE;
			NodeFild[Y][X].G = 0;
			NodeFild[Y][X].H = 0;
			NodeFild[Y][X].F = 0;
			NodeFild[Y][X].isWall = false;
			NodeFild[Y][X].pParent = NULL;
		}
	}

	// �ʱ� ������ ���� �� ����
	X = FILD_X / 3;
	Y = FILD_Y / 2;

	pStartNode = &NodeFild[Y][X];
	pStartNode->iColor = GREEN;
	pStartNode->H = (X * 2) - X;

	// �ʱ� ������ ���� �� ����
	X *= 2;

	pEndNode = &NodeFild[Y][X];
	pEndNode->iColor = RED;

	//int breakpoint = 0;
}
// 템플릿 이해 돕기 : https://www.ikpil.com/1020

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

	// 흰색이어야 칠할 수 있음
	if (NodeFild[NodeY][NodeX].iColor == WHITE) {

		// 회색으로 칠하고 벽 속성 변화
		NodeFild[NodeY][NodeX].iColor = GRAY;
		NodeFild[NodeY][NodeX].isWall = true;
	}
}

void CAstar::WallDown(int iX, int iY) {

	int NodeX = iX;
	int NodeY = iY;

	GetNodePos(&NodeX, &NodeY);

	// 회색이어야 칠할 수 있음
	if (NodeFild[NodeY][NodeX].iColor == GRAY) {

		// 흰색으로 칠하고 벽 속성 변화
		NodeFild[NodeY][NodeX].iColor = WHITE;
		NodeFild[NodeY][NodeX].isWall = false;
	}
}

void CAstar::MoveStartNode(int iX, int iY) {

	int NodeX = iX;
	int NodeY = iY;
	
	GetNodePos(&NodeX, &NodeY);

	// 기존 시작점 노드 색 변경
	pStartNode->iColor = WHITE;
	
	// 스타트 포인터 옮기기
	pStartNode = &NodeFild[NodeY][NodeX];
	pStartNode->iColor = GREEN;
	pStartNode->isWall = false;

	// 스타트 포인터 H값 변경
	pStartNode->H = GetDistance(pStartNode->iX, pStartNode->iY, pEndNode->iX, pEndNode->iY);
}

void CAstar::MoveEndNode(int iX, int iY) {

	int NodeX = iX;
	int NodeY = iY;

	GetNodePos(&NodeX, &NodeY);

	// 기존 시작점 노드 색 변경
	pEndNode->iColor = WHITE;

	// 스타트 포인터 옮기기
	pEndNode = &NodeFild[NodeY][NodeX];
	pEndNode->iColor = RED;
	pEndNode->isWall = false;

	// 스타트 포인터 H값 변경
	pStartNode->H = GetDistance(pStartNode->iX, pStartNode->iY, pEndNode->iX, pEndNode->iY);
}

bool CAstar::GetNodeRect(int NodeX, int NodeY, int &LeftX, int &RightX, int &TopY, int &BottomY) {

	// 배열 범위 밖의 X 나 Y가 들어온다면 false 리턴
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

	// 모든 노드(필드) 초기값 세팅
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

	// 초기 시작지 지정 및 세팅
	X = FILD_X / 3;
	Y = FILD_Y / 2;

	pStartNode = &NodeFild[Y][X];
	pStartNode->iColor = GREEN;
	pStartNode->H = (X * 2) - X;

	// 초기 종착지 지정 및 세팅
	X *= 2;

	pEndNode = &NodeFild[Y][X];
	pEndNode->iColor = RED;

	//int breakpoint = 0;
}
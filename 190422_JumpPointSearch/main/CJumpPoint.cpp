#include "stdafx.h"
#include "CJumpPoint.h"

CJumpPoint::CJumpPoint()
{
	NodeInit();

}


CJumpPoint::~CJumpPoint()
{
}

void CJumpPoint::NodeInit() {

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
	X = FILD_X / 5;
	Y = FILD_Y / 2;

	pStartNode = &NodeFild[Y][X];
	pStartNode->iColor = GREEN;
	pStartNode->H = (X * 2) - X;

	pEndNode = &NodeFild[Y][X * 4];
	pEndNode->iColor = RED;

	//int breakpoint = 0;
}

void CJumpPoint::ClearWall() {

	for (int Y = 0; Y < FILD_Y; Y++) {
		for (int X = 0; X < FILD_X; X++) {

			if (NodeFild[Y][X].iColor == GRAY) {

				NodeFild[Y][X].iColor = WHITE;
				NodeFild[Y][X].isWall = false;
			}
		}
	}
}

void CJumpPoint::WallUp(int iX, int iY) {

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

void CJumpPoint::WallDown(int iX, int iY) {

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

void CJumpPoint::MoveStartNode(int iX, int iY) {

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

void CJumpPoint::MoveEndNode(int iX, int iY) {

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

// 코너인지 아닌지 확인하는 함수
bool  CJumpPoint::isCorner(int iX, int iY, BYTE ParentDir) {

	switch (ParentDir) {

	case DIR_LL:
	{
		// 범위에 따른 예외처리
		if (iX - 1 < 0)
			break;

		bool TopWall = false;
		bool DownWall = false;

		// 범위 내에 있고, UU이 막히고 LU이 안막혔다면 코너
		if (iY - 1 >= 0) {
			if (NodeFild[iY - 1][iX].isWall == true && NodeFild[iY - 1][iX - 1].isWall == false)
				TopWall = true;
		}

		// 범위 내에 있고, DD가 막히고 DL이 안막혔다면 코너 
		if (iY + 1 < FILD_Y) {
			if (NodeFild[iY + 1][iX].isWall == true && NodeFild[iY + 1][iX - 1].isWall == false)
				DownWall = true;
		}

		if (TopWall == true || DownWall == true)
			return true;
	}
	break;
	case DIR_LU:
	{
		bool LeftDownWall = false;
		bool RightUpWall = false;

		// 범위 내에 있고, DD가 막히고 LD이 안막혔다면 코너
		if (iX - 1 >= 0 && iY + 1 < FILD_Y) {
			if (NodeFild[iY + 1][iX].isWall == true && NodeFild[iY + 1][iX - 1].isWall == false)
				LeftDownWall = true;
		}

		// 범위 내에 있고, RR이 막히고 UR이 안막혔다면 코너 
		if (iY - 1 >= 0 && iX + 1 < FILD_X) {
			if (NodeFild[iY][iX + 1].isWall == true && NodeFild[iY - 1][iX + 1].isWall == false)
				RightUpWall = true;
		}

		if (LeftDownWall == true || RightUpWall == true)
			return true;
	}
	break;
	case DIR_UU:
	{
		// 범위에 따른 예외처리
		if (iY - 1 < 0)
			break;

		bool LeftWall = false;
		bool RightWall = false;

		// 범위 내에 있고, LL이 막히고 LU이 안막혔다면 코너
		if (iX - 1 >= 0) {
			if (NodeFild[iY][iX - 1].isWall == true && NodeFild[iY - 1][iX - 1].isWall == false)
				LeftWall = true;
		}

		// 범위 내에 있고, RR이 막히고 UR이 안막혔다면 코너 
		if (iX + 1 < FILD_X) {
			if (NodeFild[iY][iX + 1].isWall == true && NodeFild[iY - 1][iX + 1].isWall == false)
				RightWall = true;
		}

		if (LeftWall == true || RightWall == true)
			return true;
	}
	break;
	case DIR_UR:
	{
		bool LeftUpWall = false;
		bool RightDownWall = false;

		// 범위 내에 있고, LL이 막히고 LU이 안막혔다면 코너
		if (iY - 1 >= 0 && iX - 1 >= 0) {
			if (NodeFild[iY][iX - 1].isWall == true && NodeFild[iY - 1][iX - 1].isWall == false)
				LeftUpWall = true;
		}

		// 범위 내에 있고, DD가 막히고 RD이 안막혔다면 코너 
		if (iY + 1 < FILD_Y && iX + 1 < FILD_X) {
			if (NodeFild[iY + 1][iX].isWall == true && NodeFild[iY + 1][iX + 1].isWall == false)
				RightDownWall = true;
		}

		if (LeftUpWall == true || RightDownWall == true)
			return true;
	}
	break;
	case DIR_RR:
	{
		// 범위에 따른 예외처리
		if (iX + 1 >= FILD_X)
			break;

		bool TopWall = false;
		bool DownWall = false;

		// 범위 내에 있고, UU이 막히고 UR이 안막혔다면 코너
		if (iY - 1 >= 0) {
			if (NodeFild[iY - 1][iX].isWall == true && NodeFild[iY - 1][iX + 1].isWall == false)
				TopWall = true;
		}

		// 범위 내에 있고, DD가 막히고 RD이 안막혔다면 코너 
		if (iY + 1 < FILD_Y) {
			if (NodeFild[iY + 1][iX].isWall == true && NodeFild[iY + 1][iX + 1].isWall == false)
				DownWall = true;
		}

		if (TopWall == true || DownWall == true)
			return true;
	}
	break;
	case DIR_RD:
	{
		bool LeftDownWall = false;
		bool RightUpWall = false;

		// 범위 내에 있고, LL이 막히고 DL이 안막혔다면 코너
		if (iY + 1 < FILD_Y && iX - 1 >= 0) {
			if (NodeFild[iY][iX - 1].isWall == true && NodeFild[iY + 1][iX - 1].isWall == false)
				LeftDownWall = true;
		}

		// 범위 내에 있고, UU가 막히고 UR이 안막혔다면 코너 
		if (iY - 1 >= 0 && iX + 1 < FILD_X) {
			if (NodeFild[iY - 1][iX].isWall == true && NodeFild[iY - 1][iX + 1].isWall == false)
				RightUpWall = true;
		}

		if (LeftDownWall == true || RightUpWall == true)
			return true;
	}
	break;
	case DIR_DD:
	{
		// 범위에 따른 예외처리
		if (iY + 1 >= FILD_Y)
			break;

		bool LeftWall = false;
		bool RightWall = false;

		// 범위 내에 있고, LL이 막히고 DL이 안막혔다면 코너
		if (iX - 1 >= 0) {
			if (NodeFild[iY][iX - 1].isWall == true && NodeFild[iY + 1][iX - 1].isWall == false)
				LeftWall = true;
		}

		// 범위 내에 있고, RR이 막히고 RD가 안막혔다면 코너 
		if (iX + 1 < FILD_X) {
			if (NodeFild[iY][iX + 1].isWall == true && NodeFild[iY + 1][iX + 1].isWall == false)
				RightWall = true;
		}

		if (LeftWall == true || RightWall == true)
			return true;
	}
	break;
	case DIR_DL:
	{
		bool LeftUpWall = false;
		bool RightDownWall = false;

		// 범위 내에 있고, RR이 막히고 RD이 안막혔다면 코너
		if (iY + 1 < FILD_Y && iX + 1 < FILD_X) {
			if (NodeFild[iY][iX + 1].isWall == true && NodeFild[iY + 1][iX + 1].isWall == false)
				LeftUpWall = true;
		}

		// 범위 내에 있고, UU이 막히고 LU이 안막혔다면 코너 
		if (iY - 1 >= 0 && iX - 1 >= 0) {
			if (NodeFild[iY - 1][iX].isWall == true && NodeFild[iY - 1][iX - 1].isWall == false)
				RightDownWall = true;
		}

		if (LeftUpWall == true || RightDownWall == true)
			return true;
	}
	break;
	default:
		break;
	}
	
	return false;
}

bool CJumpPoint::CheckNodeJump(int PosX, int PosY, BYTE ParentDir, DWORD SearchedColor) {

	int iX = PosX;
	int iY = PosY;
	bool retval;
	NODE *pNode = NULL;

	switch (ParentDir) {

	case DIR_LL:
	{

	}
	break;
	case DIR_RR:
	{
		for (;;) {
			// 범위 벗어나면 false;
			if (PosX >= FILD_X) {
				retval = false;
				break;
			}
			// 벽이라면 false
			if (GetIsWall(PosX, PosY) == true) {
				retval = false;
				break;
			}
			// 종점인지 체크, 맞다면 true
			pNode = GetNodePtr(PosX, PosY);
			if (pNode == pEndNode) {
				retval = true;
				break;
			}
			// 코너라면 true
			pNode->iColor = SearchedColor;
			if (isCorner(PosX, PosY, DIR_RR) == true){
				retval = true;
				break;
			}

			PosX += 1;
		}
	}
	break;
	case DIR_UU:
	{
		for (;;) {
			// 범위 벗어나면 false;
			if (PosY < 0) {
				retval = false;
				break;
			}
			// 벽이라면 false
			if (GetIsWall(PosX, PosY) == true) {
				retval = false;
				break;
			}
			// 종점인지 체크, 맞다면 true
			pNode = GetNodePtr(PosX, PosY);
			if (pNode == pEndNode) {
				retval = true;
				break;
			}
			// 코너라면 true
			pNode->iColor = SearchedColor;
			if (isCorner(PosX, PosY, DIR_UU) == true) {
				retval = true;
				break;
			}

			PosY -= 1;
		}
	}
	break;
	}

	return retval;
}

// bool 값이 의미가 있는지?
bool CJumpPoint::Jump(int PosX, int PosY, BYTE ParentDir, NODE *pParent, NODE **pFindNode, DWORD SearchedColor) {

	int iX = PosX;
	int iY = PosY;

	// 필드를 벗어난다면
	if (iX < 0 || iX >= FILD_X)
		return false;

	if (iY < 0 || iY >= FILD_Y)
		return false;

	// 현위치가 벽이라면
	NODE *pNowNode = GetNodePtr(iX, iY);
	if (pNowNode->isWall == true)
		return false;

	// 현위치가 목적지라면
	NODE *pEndNode = GetEndPtr();
	if (pNowNode == pEndNode) {
		
		pNowNode->byParentDir = ParentDir;
		pNowNode->pParent = pParent;

		*pFindNode = pNowNode;
		return true;
	}

	// 코너가 맞는지 확인 후, 찾아본 노드는 색 변경
	bool CornerCheck = isCorner(iX, iY, ParentDir);
	pNowNode->iColor = SearchedColor;
	if (CornerCheck == true) {
		// 코너가 맞을 때 해야하는 처리/ (Open/Close List 확인 후,) 노드의 속성 변경
		NODE *pStartNode = GetStartPtr();
		NODE *pEndNode = GetEndPtr();

		pNowNode->byParentDir = ParentDir;
		pNowNode->pParent = pParent;
		pNowNode->G = GetDistance(pNowNode->iX, pNowNode->iY, pStartNode->iX, pStartNode->iY);
		pNowNode->H = GetDistance(pNowNode->iX, pNowNode->iY, pEndNode->iX, pEndNode->iY);
		pNowNode->F = pNowNode->G + pNowNode->H;

		*pFindNode = pNowNode;
		return true;
	}

	switch (ParentDir) {

	case DIR_LL:
	{
		Jump(iX-1, iY, DIR_LL, pParent, pFindNode, SearchedColor);
	}
	break;
	case DIR_LU:
	{
		Jump(iX-1, iY, DIR_LL, pParent, pFindNode, SearchedColor);
		Jump(iX, iY - 1, DIR_UU, pParent, pFindNode, SearchedColor);
		if (*pFindNode == NULL)
			Jump(iX - 1, iY - 1, DIR_LU, pParent, pFindNode, SearchedColor);
	}
	break;
	case DIR_UU:
	{
		Jump(iX, iY-1, DIR_UU, pParent, pFindNode, SearchedColor);
	}
	break;
	case DIR_UR:
	{
		//Jump(iX + 1, iY, DIR_RR, pParent, pFindNode, SearchedColor);
		//Jump(iX, iY - 1, DIR_UU, pParent, pFindNode, SearchedColor);
		bool isRR = CheckNodeJump(iX + 1, iY, DIR_RR, SearchedColor);
		if (isRR == true) {
			*pFindNode = GetNodePtr(iX, iY);
			return true;
		}
		bool isUU = CheckNodeJump(iX, iY - 1, DIR_UU, SearchedColor);
		if (isUU == true) {
			*pFindNode = GetNodePtr(iX, iY);
			return true;
		}
		if (isRR == false && isUU == false)
			Jump(iX + 1, iY - 1, DIR_UR, pParent, pFindNode, SearchedColor);
	}
	break;
	case DIR_RR:
	{
		Jump(iX + 1, iY, DIR_RR, pParent, pFindNode, SearchedColor);
	}
	break;
	case DIR_RD:
	{
		Jump(iX + 1, iY, DIR_RR, pParent, pFindNode, SearchedColor);
		Jump(iX, iY + 1, DIR_DD, pParent, pFindNode, SearchedColor);
		if (*pFindNode == NULL)
			Jump(iX + 1, iY + 1, DIR_RD, pParent, pFindNode, SearchedColor);
	}
	break;
	case DIR_DD:
	{
		Jump(iX, iY + 1, DIR_DD, pParent, pFindNode, SearchedColor);
	}
	break;
	case DIR_DL:
	{
		Jump(iX - 1, iY, DIR_LL, pParent, pFindNode, SearchedColor);
		Jump(iX, iY + 1, DIR_DD, pParent, pFindNode, SearchedColor);
		if (*pFindNode == NULL)
			Jump(iX - 1, iY + 1, DIR_DL, pParent, pFindNode, SearchedColor);
	}
	break;
	default:
		break;
	}

	return false;
}

bool CJumpPoint::JumpLL(int PosX, int PosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor) {

	int iG = pParent->G + 1;
	

	// 여기 노드가 생겨야함? G값등 구해서 넣어야함
	for(;;) {
	
		if (PosX < 0)
			break;
		
		// 벽이라면 끝
		if (GetIsWall(PosX, PosY) == true)
			break;

		// 목적지 노드라면 끝
		NODE *pNowNode = GetNodePtr(PosX, PosY);
		if (pNowNode == pEndNode) {

			*pFindNode = pNowNode;
			return true;
		}

		pNowNode->iColor = SearchedColor;
		bool Corner = isCorner(PosX, PosY, DIR_LL);
		if (Corner == true) {
			pNowNode->byParentDir = DIR_LL;
			pNowNode->pParent = pParent;
			pNowNode->iX = PosX;
			pNowNode->iY = PosY;
			pNowNode->G = iG;
			pNowNode->H = GetDistance(PosX, PosY, pEndNode->iX, pEndNode->iY);
			pNowNode->F = pNowNode->G + pNowNode->H;

			*pFindNode = pNowNode;

			return true;
		}

		iG += 1;
		PosX -= 1;
	}

	return false;
}

bool CJumpPoint::JumpRR(int PosX, int PosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor) {

	int iG = pParent->G + 1;

	// 여기 노드가 생겨야함? G값등 구해서 넣어야함
	for (;;) {

		// 범위 벗어나면 끝
		if (PosX >= FILD_X)
			break;

		// 벽이라면 끝
		if (GetIsWall(PosX, PosY) == true)
			break;

		// 목적지 노드라면 끝
		NODE *pNowNode = GetNodePtr(PosX, PosY);
		if (pNowNode == pEndNode) {

			*pFindNode = pNowNode;
			return true;
		}

		pNowNode->iColor = SearchedColor;
		bool Corner = isCorner(PosX, PosY, DIR_RR);
		if (Corner == true) {
			pNowNode->byParentDir = DIR_RR;
			pNowNode->pParent = pParent;
			pNowNode->iX = PosX;
			pNowNode->iY = PosY;
			pNowNode->G = iG;
			pNowNode->H = GetDistance(PosX, PosY, pEndNode->iX, pEndNode->iY);
			pNowNode->F = pNowNode->G + pNowNode->H;

			*pFindNode = pNowNode;

			return true;
		}

		iG += 1;
		PosX += 1;
	}

	return false;
}

bool CJumpPoint::JumpUU(int PosX, int PosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor) {

	int iG = pParent->G + 1;


	// 여기 노드가 생겨야함? G값등 구해서 넣어야함
	for (;;) {

		if (PosY < 0)
			break;

		// 벽이라면 끝
		if (GetIsWall(PosX, PosY) == true)
			break;

		// 목적지 노드라면 끝
		NODE *pNowNode = GetNodePtr(PosX, PosY);
		if (pNowNode == pEndNode) {

			*pFindNode = pNowNode;
			return true;
		}

		pNowNode->iColor = SearchedColor;
		bool Corner = isCorner(PosX, PosY, DIR_UU);
		if (Corner == true) {
			pNowNode->byParentDir = DIR_UU;
			pNowNode->pParent = pParent;
			pNowNode->iX = PosX;
			pNowNode->iY = PosY;
			pNowNode->G = iG;
			pNowNode->H = GetDistance(PosX, PosY, pEndNode->iX, pEndNode->iY);
			pNowNode->F = pNowNode->G + pNowNode->H;

			*pFindNode = pNowNode;

			return true;
		}

		iG += 1;
		PosY -= 1;
	}

	return false;
}

bool CJumpPoint::JumpDD(int PosX, int PosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor) {

	int iG = pParent->G + 1;


	// 여기 노드가 생겨야함? G값등 구해서 넣어야함
	for (;;) {

		if (PosY >= FILD_Y)
			break;

		// 벽이라면 끝
		if (GetIsWall(PosX, PosY) == true)
			break;

		// 목적지 노드라면 끝
		NODE *pNowNode = GetNodePtr(PosX, PosY);
		if (pNowNode == pEndNode) {

			*pFindNode = pNowNode;
			return true;
		}

		pNowNode->iColor = SearchedColor;
		bool Corner = isCorner(PosX, PosY, DIR_DD);
		if (Corner == true) {
			pNowNode->byParentDir = DIR_DD;
			pNowNode->pParent = pParent;
			pNowNode->iX = PosX;
			pNowNode->iY = PosY;
			pNowNode->G = iG;
			pNowNode->H = GetDistance(PosX, PosY, pEndNode->iX, pEndNode->iY);
			pNowNode->F = pNowNode->G + pNowNode->H;

			*pFindNode = pNowNode;

			return true;
		}

		iG += 1;
		PosY += 1;
	}

	return false;
}



bool CJumpPoint::GetNodeRect(int NodeX, int NodeY, int &LeftX, int &RightX, int &TopY, int &BottomY) {

	// 배열 범위 밖의 X 나 Y가 들어온다면 false 리턴
	if (NodeX >= FILD_X || NodeY >= FILD_Y)
		return false;

	LeftX = NodeFild[NodeY][NodeX].iLeftX;
	RightX = NodeFild[NodeY][NodeX].iRightX;
	TopY = NodeFild[NodeY][NodeX].iTopY;
	BottomY = NodeFild[NodeY][NodeX].iBottomY;

	return true;
}

void CJumpPoint::GetNodePos(int *iX, int *iY) {

	int PosX = *iX / FILD_SIZE;
	int PosY = *iY / FILD_SIZE;

	*iX = PosX;
	*iY = PosY;
}

int CJumpPoint::GetDistance(int strX, int strY, int endX, int endY) {

	int distance = abs(endX - strX) + abs(endY - strY);

	return distance;
}

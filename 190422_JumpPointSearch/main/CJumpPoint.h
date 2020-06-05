#pragma once
#ifndef __JUMP_POINT__
#define __JUMP_POINT__

#define DIR_LL	0
#define DIR_LU  1
#define DIR_UU  2
#define DIR_UR	3
#define DIR_RR	4
#define DIR_RD	5
#define DIR_DD	6
#define DIR_DL	7

enum NODE_SIZE {

	FILD_Y = 32,
	FILD_X = 64,
	FILD_SIZE = 20
};

enum NODE_COLOR {

	WHITE = 1,
	BLACK,
	GRAY,
	RED,
	GREEN,
	BLUE,
	YELLOW
};

typedef struct st_NODE {

	int iX;
	int iY;
	int iLeftX;
	int iRightX;
	int iTopY;
	int iBottomY;
	int G;
	int H;
	int F;
	bool isWall;
	DWORD	iColor;
	BYTE	byParentDir;
	st_NODE *pParent;

} NODE;

class CJumpPoint
{
public:
	CJumpPoint();
	~CJumpPoint();

	// NodeFild 초기화 함수 (x,y,left/right_X,top/bottom_Y,h)
	void NodeInit();

	// 벽 전부 삭제
	void ClearWall();

	// 벽 생성
	void WallUp(int iX, int iY);

	// 벽 삭제
	void WallDown(int iX, int iY);

	// 시작지점 옮기기
	void MoveStartNode(int iX, int iY);

	// 종착지점 옮기기
	void MoveEndNode(int iX, int iY);

	// 코너가 맞는지 확인
	bool isCorner(int iX, int iY, BYTE ParentDir);

	bool CheckNodeJump(int PosX, int PosY, BYTE ParentDir, DWORD SearchedColor);

	// 부모의 방향에 따라 노드가 생겨야 할 자리 찾아주는 함수 (결과를 pFindNode로 반환, 생겨야할 노드 주소 반환)
	bool Jump(int PosX, int PosY, BYTE ParentDir, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);

	// 부모의 방향에 따라 노드가 생겨야 할 자리 찾아주는 함수 (결과를 pFindNode로 반환, 생겨야할 노드 주소 반환)
	bool JumpLL(int PosX, int PosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpRR(int PosX, int PosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpUU(int PosX, int PosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpDD(int PosX, int PosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpLU(int PosX, int PosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpUR(int PosX, int PosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpRD(int PosX, int PosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpDL(int PosX, int PosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);

	// Rect요소값 얻기
	bool GetNodeRect(int NodeX, int NodeY, int &LeftX, int &RightX, int &TopY, int &BottomY);

	// color값 얻기
	int GetColor(int NodeX, int NodeY) { return NodeFild[NodeY][NodeX].iColor; }
	
	// 방향에 따른 블록 체크 (isWall)
	bool GetIsWall(int NodeX, int NodeY) { return NodeFild[NodeY][NodeX].isWall; }

	// NodeFild[NodeY][NodeX]의 포인터 얻기
	NODE *GetNodePtr(int NodeX, int NodeY) { return &NodeFild[NodeY][NodeX]; }

	// pStartNode 포인터 얻기
	NODE *GetStartPtr() { return pStartNode; }

	// pEndNode 포인터 얻기
	NODE *GetEndPtr() { return pEndNode; }

	// 마우스 x,y 값으로 해당 노드 찾는 함수
	void GetNodePos(int *iX, int *iY);

	// 노드간 거리값 얻는 함수
	int GetDistance(int strX, int strY, int endX, int endY);

	// 노드 색 설정
	void SetColor(int NodeX, int NodeY, int Color) { NodeFild[NodeY][NodeX].iColor = Color; }

public:
	NODE NodeFild[FILD_Y][FILD_X]; // 0 ~ 49, 0 ~ 99
	NODE *pEndNode;
	NODE *pStartNode;
};


#endif
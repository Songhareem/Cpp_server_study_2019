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

	// NodeFild �ʱ�ȭ �Լ� (x,y,left/right_X,top/bottom_Y,h)
	void NodeInit();

	// �� ���� ����
	void ClearWall();

	// �� ����
	void WallUp(int iX, int iY);

	// �� ����
	void WallDown(int iX, int iY);

	// �������� �ű��
	void MoveStartNode(int iX, int iY);

	// �������� �ű��
	void MoveEndNode(int iX, int iY);

	// �ڳʰ� �´��� Ȯ��
	bool isCorner(int iX, int iY, BYTE ParentDir);

	bool CheckNodeJump(int PosX, int PosY, BYTE ParentDir, DWORD SearchedColor);

	// �θ��� ���⿡ ���� ��尡 ���ܾ� �� �ڸ� ã���ִ� �Լ� (����� pFindNode�� ��ȯ, ���ܾ��� ��� �ּ� ��ȯ)
	bool Jump(int PosX, int PosY, BYTE ParentDir, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);

	// �θ��� ���⿡ ���� ��尡 ���ܾ� �� �ڸ� ã���ִ� �Լ� (����� pFindNode�� ��ȯ, ���ܾ��� ��� �ּ� ��ȯ)
	bool JumpLL(int PosX, int PosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpRR(int PosX, int PosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpUU(int PosX, int PosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpDD(int PosX, int PosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpLU(int PosX, int PosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpUR(int PosX, int PosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpRD(int PosX, int PosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpDL(int PosX, int PosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);

	// Rect��Ұ� ���
	bool GetNodeRect(int NodeX, int NodeY, int &LeftX, int &RightX, int &TopY, int &BottomY);

	// color�� ���
	int GetColor(int NodeX, int NodeY) { return NodeFild[NodeY][NodeX].iColor; }
	
	// ���⿡ ���� ��� üũ (isWall)
	bool GetIsWall(int NodeX, int NodeY) { return NodeFild[NodeY][NodeX].isWall; }

	// NodeFild[NodeY][NodeX]�� ������ ���
	NODE *GetNodePtr(int NodeX, int NodeY) { return &NodeFild[NodeY][NodeX]; }

	// pStartNode ������ ���
	NODE *GetStartPtr() { return pStartNode; }

	// pEndNode ������ ���
	NODE *GetEndPtr() { return pEndNode; }

	// ���콺 x,y ������ �ش� ��� ã�� �Լ�
	void GetNodePos(int *iX, int *iY);

	// ��尣 �Ÿ��� ��� �Լ�
	int GetDistance(int strX, int strY, int endX, int endY);

	// ��� �� ����
	void SetColor(int NodeX, int NodeY, int Color) { NodeFild[NodeY][NodeX].iColor = Color; }

public:
	NODE NodeFild[FILD_Y][FILD_X]; // 0 ~ 49, 0 ~ 99
	NODE *pEndNode;
	NODE *pStartNode;
};


#endif
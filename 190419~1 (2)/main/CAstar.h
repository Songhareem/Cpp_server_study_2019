#pragma once
#ifndef __ASTAR__
#define __ASTAR__

#include <list>
#include <algorithm>

using namespace std;

enum NODE_SIZE {

	FILD_Y = 20,
	FILD_X = 40,
	FILD_SIZE = 30
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
	int iColor;
	bool isWall;
	st_NODE *pParent;

} NODE;

class CAstar
{

public:
	CAstar();
	~CAstar();

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

	// Rect��Ұ� ���
	bool GetNodeRect(int NodeX, int NodeY, int &LeftX, int &RightX, int &TopY, int &BottomY);
	
	// color�� ���
	int GetColor(int NodeX, int NodeY) { return NodeFild[NodeY][NodeX].iColor; }

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
	//void SetNodeColor(int NodeX, int NodeY, int Color) { NodeFild[NodeY][NodeX].iColor = Color; }

	// ��� �� �Ӽ� ����
	//void SetNodeWall(int NodeX, int NodeY, bool Wall) { NodeFild[NodeY][NodeX].isWall = Wall; }

	// NodeFild �ʱ�ȭ �Լ� (x,y,left/right_X,top/bottom_Y,h)
	void NodeInit();

public:
	NODE NodeFild[FILD_Y][FILD_X]; // 0 ~ 49, 0 ~ 99
	NODE *pEndNode;
	NODE *pStartNode;
};

#endif
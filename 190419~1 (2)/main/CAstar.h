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

	// Rect요소값 얻기
	bool GetNodeRect(int NodeX, int NodeY, int &LeftX, int &RightX, int &TopY, int &BottomY);
	
	// color값 얻기
	int GetColor(int NodeX, int NodeY) { return NodeFild[NodeY][NodeX].iColor; }

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

	// 노드 색 변경
	//void SetNodeColor(int NodeX, int NodeY, int Color) { NodeFild[NodeY][NodeX].iColor = Color; }

	// 노드 벽 속성 변경
	//void SetNodeWall(int NodeX, int NodeY, bool Wall) { NodeFild[NodeY][NodeX].isWall = Wall; }

	// NodeFild 초기화 함수 (x,y,left/right_X,top/bottom_Y,h)
	void NodeInit();

public:
	NODE NodeFild[FILD_Y][FILD_X]; // 0 ~ 49, 0 ~ 99
	NODE *pEndNode;
	NODE *pStartNode;
};

#endif
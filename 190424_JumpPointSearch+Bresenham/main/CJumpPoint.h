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
	st_NODE *pNext;

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

	bool JumpCornerSearch(int PosX, int PosY, BYTE SearchDir, DWORD SearchedColor);

	// 부모의 방향에 따라 노드가 생겨야 할 자리 찾아주는 함수 (결과를 pFindNode로 반환, 생겨야할 노드 주소 반환)
	//bool Jump(int PosX, int PosY, BYTE ParentDir, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);

	// 부모의 방향에 따라 노드가 생겨야 할 자리 찾아주는 함수 (결과를 pFindNode로 반환, 생겨야할 노드 주소 반환)
	bool JumpLL(int iPosX, int iPosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpRR(int iPosX, int iPosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpUU(int iPosX, int iPosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpDD(int iPosX, int iPosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpLU(int iPosX, int iPosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpUR(int iPosX, int iPosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpRD(int iPosX, int iPosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpDL(int iPosX, int iPosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);

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

//#ifndef __CLASS_PROCADEMY_JUMP_POINT_SEARCH__
//#define __CLASS_PROCADEMY_JUMP_POINT_SEARCH__
//
//using namespace std;
//
//#define JPS_FIXED
//
//namespace procademy
//{
//	class CJumpPointSearch
//	{
//	public:
//
//		//-------------------------------------------
//		// 속성, 설정치
//		//-------------------------------------------
//		enum
//		{
//			OBSTACLES_NONE = 0,
//			OBSTACLES_FIXED,			// 수정 불가능한 고정 장애물
//			OBSTACLES_DYNAMIC,			// 수정 가능한 동적 장애물 (캐릭터 등 움직이는 장애물용)
//
//			MAX_WIDTH = 600,			// 최대 맵 사이즈 
//			MAX_HEIGHT = 200,
//		};
//
//		//-------------------------------------------
//		// 길찾기 결과를 이 구조체로 알려줌.
//		//-------------------------------------------
//		struct stPATH_POINT
//		{
//			int iX;
//			int iY;
//		};
//
//
//		//------------------------------------------------------
//		// 길찾기중 타일 하나하나에 대한 노드
//		//------------------------------------------------------
//		struct stNODE
//		{
//			int iX;
//			int iY;
//
//			int iG;
//			int iH;
//			int iF;
//
//			stNODE *pParent;
//			stNODE *pNext;		// 이는 마지막에 길을 다 찾아서 완성 후 사용.
//								// 출발점부터 길을 확인하기 위해
//								// 도착점 부터 출발점으로 Parent 를 타고가며 Next 를 연결한다.
//		};
//
//	public:
//
//		CJumpPointSearch(int iWidth, int iHeight, int iMaxOpen = 65535, int iMaxJump = 65535);	// MaxOpen 는 길을 몾찾을때 또는 너무 어려운 길을 찾을때
//																						// 적정선에서 포기하도록 하는 수치.
//																						// MaxOpen 횟수만큼 Open 노드를 탐지함.
//																						// MaxJump 는 Jump 탐색의 최대횟수.
//																						// 완벽한 길찾이가 아닌 일정 영역 기준으로 찾는경우 지정.
//		~CJumpPointSearch();
//
//		//////////////////////////////////////////////////////////////////////////////////
//		// 길을 찾아주는 함수.
//		//
//		// JPS 이므로 꺽어지는 부분마다의 위치를 찾아서 PATH_POINT 배열에 담아줌.
//		//
//		// Parameters: StartX, StartY, EndX, EndY, 직신이동 플래그. 경로를 얻을 PATH_POINT 배열, 배열개수
//		// Return: 경로 개수. (0 이면 못찾음)
//		//////////////////////////////////////////////////////////////////////////////////
//		int		PathFind(int iStartX, int iStartY, int iDestX, int iDestY, bool bStraightRevison, stPATH_POINT *pPath, int iPathMax);
//
//
//		//////////////////////////////////////////////////////////////////////////////////
//		// Dynamic 속성의 체크 여부를 설정.
//		//////////////////////////////////////////////////////////////////////////////////
//		void	CheckDynamicMode(bool bFlag)
//		{
//			_bCheckDynamic = bFlag;
//		}
//
//		//////////////////////////////////////////////////////////////////////////////////
//		// Fixed 속성을 외부에서 지정함. 이는 지울 수 없음.
//		//
//		// X, Y 좌표에 고정 장애물 지정. 
//		//////////////////////////////////////////////////////////////////////////////////
//		void	SetFixedObstacles(int iX, int iY, bool bClear = false)
//		{
//			if (iX < 0 || iX >= _iWidth || iY < 0 || iY >= _iHeight)
//				return;
//
//			_Map[iY][iX] = OBSTACLES_FIXED;
//
//			if (bClear)
//				_Map[iY][iX] = OBSTACLES_NONE;
//
//		}
//
//
//		//////////////////////////////////////////////////////////////////////////////////
//		// Dynamic 속성을 외부에서 지정함.
//		//
//		// X, Y 좌표에 다이나믹 속성을 넣거나 뺀다.  
//		// 고정속성(FIXED) 의 위치에는 다이나믹 속성을 넣고 뺼 수 없음.
//		//////////////////////////////////////////////////////////////////////////////////
//		void	SetDynamicObstacles(int iX, int iY, bool Obstacles)
//		{
//			if (iX < 0 || iX >= _iWidth || iY < 0 || iY >= _iHeight)
//				return;
//
//			if (_Map[iY][iX] == OBSTACLES_FIXED)
//				return;
//
//			if (Obstacles)
//				_Map[iY][iX] = OBSTACLES_DYNAMIC;
//			else
//				_Map[iY][iX] = OBSTACLES_NONE;
//		}
//
//
//	private:
//
//
//		//-------------------------------------------
//		// 맵의 크기 및 속성 배열, 생성자에서 입력.생성
//		//-------------------------------------------
//		int					_iWidth;
//		int					_iHeight;
//
//
//		//-------------------------------------------
//		// Dynamic 속성을 체크 할 것인지 플래그.  false 라면 Dynamic 속성은 무시함.
//		//-------------------------------------------
//		bool				_bCheckDynamic;
//
//		//-------------------------------------------
//		// 길 검색시 최대 Open 노드 탐지 횟수 제한
//		//-------------------------------------------
//		int					_iMaxOpen;
//		int					_iMaxJump;
//
//		unsigned char		_Map[MAX_HEIGHT][MAX_WIDTH];
//
//
//
//
//	private:
//		enum
//		{
//			DIR_LL = 0,					// 길찾기시 사용하는 방향 값
//			DIR_LU,
//			DIR_UU,
//			DIR_RU,
//			DIR_RR,
//			DIR_RD,
//			DIR_DD,
//			DIR_LD
//		};
//
//		//------------------------------------------------------
//		// multiset 정렬시 사용되는 비교자
//		//------------------------------------------------------
//		template < class T >
//		struct NODE_COMPARE : public binary_function< T, T, bool >
//		{
//			bool operator() (T NodeA, T NodeB) const
//			{
//				return NodeA->iF < NodeB->iF;
//			}
//		};
//
//		//------------------------------------------------------
//		// multiset 검색시 사용되는 비교자
//		//------------------------------------------------------
//		struct NODE_FINDER
//		{
//			NODE_FINDER(int iX, int iY) : iPosX(iX), iPosY(iY)
//			{
//			}
//
//			bool operator() (stNODE *pNode) const
//			{
//				if (pNode->iX == iPosX && pNode->iY == iPosY)
//					return true;
//
//				return false;
//			}
//
//			int iPosX;
//			int iPosY;
//		};
//
//
//		//------------------------------------------------------
//		// Clear Open, Closed
//		//------------------------------------------------------
//		void ClearOpenClosed(void);
//
//		//////////////////////////////////////////////////////////////////////////////////
//		// 8방향이 아닌 전방향에 대한 직선이동 후보정 처리 함수.
//		//////////////////////////////////////////////////////////////////////////////////
//		void PathStraightRevison(stNODE *pLastNode);
//
//
//	public:
//		//------------------------------------------------------
//		// 갈 수 있음, 없음.
//		//------------------------------------------------------
//		bool CheckWalkableFunc(int iX, int iY);
//
//	private:
//		//------------------------------------------------------
//		// 다음 꺽임을 찾아주는 Jump 재귀함수.
//		//------------------------------------------------------
//		bool Jump(int iX, int iY, int iDir, int *pJumpX, int *pJumpY);
//
//
//		//------------------------------------------------------
//		// 특정 좌표에서 방향을 기준으로 노드생성 위치를 찾아서 노드 생성.
//		//------------------------------------------------------
//		bool CheckDirection(stNODE *pParent, int iX, int iY, int iDir);
//
//
//		//------------------------------------------------------
//		// 노드 기준으로 주변으로 퍼짐!
//		//------------------------------------------------------
//		void NeighborsNode(stNODE *pNode);
//
//
//		//------------------------------------------------------
//		// 목적지
//		//------------------------------------------------------
//		int		_iDestX;
//		int		_iDestY;
//
//
//		//------------------------------------------------------
//		// 탐색을 진행 할 오픈노드 힙
//		// Node 의 F 값이 가장 작은 녀석이 위로 올라옴.
//		//------------------------------------------------------
//		multiset<stNODE *, NODE_COMPARE<stNODE *>>	_OpenNode;
//
//		//------------------------------------------------------
//		// 지나왔던 노드를 저장하는 클로드리스트
//		//------------------------------------------------------
//		list<stNODE *>								_ClosedNode;
//
//	};
//}
//
//#endif
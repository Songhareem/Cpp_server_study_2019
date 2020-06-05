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

	bool JumpCornerSearch(int PosX, int PosY, BYTE SearchDir, DWORD SearchedColor);

	// �θ��� ���⿡ ���� ��尡 ���ܾ� �� �ڸ� ã���ִ� �Լ� (����� pFindNode�� ��ȯ, ���ܾ��� ��� �ּ� ��ȯ)
	//bool Jump(int PosX, int PosY, BYTE ParentDir, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);

	// �θ��� ���⿡ ���� ��尡 ���ܾ� �� �ڸ� ã���ִ� �Լ� (����� pFindNode�� ��ȯ, ���ܾ��� ��� �ּ� ��ȯ)
	bool JumpLL(int iPosX, int iPosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpRR(int iPosX, int iPosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpUU(int iPosX, int iPosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpDD(int iPosX, int iPosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpLU(int iPosX, int iPosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpUR(int iPosX, int iPosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpRD(int iPosX, int iPosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);
	bool JumpDL(int iPosX, int iPosY, NODE *pParent, NODE **pFindNode, DWORD SearchedColor);

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
//		// �Ӽ�, ����ġ
//		//-------------------------------------------
//		enum
//		{
//			OBSTACLES_NONE = 0,
//			OBSTACLES_FIXED,			// ���� �Ұ����� ���� ��ֹ�
//			OBSTACLES_DYNAMIC,			// ���� ������ ���� ��ֹ� (ĳ���� �� �����̴� ��ֹ���)
//
//			MAX_WIDTH = 600,			// �ִ� �� ������ 
//			MAX_HEIGHT = 200,
//		};
//
//		//-------------------------------------------
//		// ��ã�� ����� �� ����ü�� �˷���.
//		//-------------------------------------------
//		struct stPATH_POINT
//		{
//			int iX;
//			int iY;
//		};
//
//
//		//------------------------------------------------------
//		// ��ã���� Ÿ�� �ϳ��ϳ��� ���� ���
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
//			stNODE *pNext;		// �̴� �������� ���� �� ã�Ƽ� �ϼ� �� ���.
//								// ��������� ���� Ȯ���ϱ� ����
//								// ������ ���� ��������� Parent �� Ÿ���� Next �� �����Ѵ�.
//		};
//
//	public:
//
//		CJumpPointSearch(int iWidth, int iHeight, int iMaxOpen = 65535, int iMaxJump = 65535);	// MaxOpen �� ���� ��ã���� �Ǵ� �ʹ� ����� ���� ã����
//																						// ���������� �����ϵ��� �ϴ� ��ġ.
//																						// MaxOpen Ƚ����ŭ Open ��带 Ž����.
//																						// MaxJump �� Jump Ž���� �ִ�Ƚ��.
//																						// �Ϻ��� ��ã�̰� �ƴ� ���� ���� �������� ã�°�� ����.
//		~CJumpPointSearch();
//
//		//////////////////////////////////////////////////////////////////////////////////
//		// ���� ã���ִ� �Լ�.
//		//
//		// JPS �̹Ƿ� �������� �κи����� ��ġ�� ã�Ƽ� PATH_POINT �迭�� �����.
//		//
//		// Parameters: StartX, StartY, EndX, EndY, �����̵� �÷���. ��θ� ���� PATH_POINT �迭, �迭����
//		// Return: ��� ����. (0 �̸� ��ã��)
//		//////////////////////////////////////////////////////////////////////////////////
//		int		PathFind(int iStartX, int iStartY, int iDestX, int iDestY, bool bStraightRevison, stPATH_POINT *pPath, int iPathMax);
//
//
//		//////////////////////////////////////////////////////////////////////////////////
//		// Dynamic �Ӽ��� üũ ���θ� ����.
//		//////////////////////////////////////////////////////////////////////////////////
//		void	CheckDynamicMode(bool bFlag)
//		{
//			_bCheckDynamic = bFlag;
//		}
//
//		//////////////////////////////////////////////////////////////////////////////////
//		// Fixed �Ӽ��� �ܺο��� ������. �̴� ���� �� ����.
//		//
//		// X, Y ��ǥ�� ���� ��ֹ� ����. 
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
//		// Dynamic �Ӽ��� �ܺο��� ������.
//		//
//		// X, Y ��ǥ�� ���̳��� �Ӽ��� �ְų� ����.  
//		// �����Ӽ�(FIXED) �� ��ġ���� ���̳��� �Ӽ��� �ְ� �E �� ����.
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
//		// ���� ũ�� �� �Ӽ� �迭, �����ڿ��� �Է�.����
//		//-------------------------------------------
//		int					_iWidth;
//		int					_iHeight;
//
//
//		//-------------------------------------------
//		// Dynamic �Ӽ��� üũ �� ������ �÷���.  false ��� Dynamic �Ӽ��� ������.
//		//-------------------------------------------
//		bool				_bCheckDynamic;
//
//		//-------------------------------------------
//		// �� �˻��� �ִ� Open ��� Ž�� Ƚ�� ����
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
//			DIR_LL = 0,					// ��ã��� ����ϴ� ���� ��
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
//		// multiset ���Ľ� ���Ǵ� ����
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
//		// multiset �˻��� ���Ǵ� ����
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
//		// 8������ �ƴ� �����⿡ ���� �����̵� �ĺ��� ó�� �Լ�.
//		//////////////////////////////////////////////////////////////////////////////////
//		void PathStraightRevison(stNODE *pLastNode);
//
//
//	public:
//		//------------------------------------------------------
//		// �� �� ����, ����.
//		//------------------------------------------------------
//		bool CheckWalkableFunc(int iX, int iY);
//
//	private:
//		//------------------------------------------------------
//		// ���� ������ ã���ִ� Jump ����Լ�.
//		//------------------------------------------------------
//		bool Jump(int iX, int iY, int iDir, int *pJumpX, int *pJumpY);
//
//
//		//------------------------------------------------------
//		// Ư�� ��ǥ���� ������ �������� ������ ��ġ�� ã�Ƽ� ��� ����.
//		//------------------------------------------------------
//		bool CheckDirection(stNODE *pParent, int iX, int iY, int iDir);
//
//
//		//------------------------------------------------------
//		// ��� �������� �ֺ����� ����!
//		//------------------------------------------------------
//		void NeighborsNode(stNODE *pNode);
//
//
//		//------------------------------------------------------
//		// ������
//		//------------------------------------------------------
//		int		_iDestX;
//		int		_iDestY;
//
//
//		//------------------------------------------------------
//		// Ž���� ���� �� ���³�� ��
//		// Node �� F ���� ���� ���� �༮�� ���� �ö��.
//		//------------------------------------------------------
//		multiset<stNODE *, NODE_COMPARE<stNODE *>>	_OpenNode;
//
//		//------------------------------------------------------
//		// �����Դ� ��带 �����ϴ� Ŭ�ε帮��Ʈ
//		//------------------------------------------------------
//		list<stNODE *>								_ClosedNode;
//
//	};
//}
//
//#endif
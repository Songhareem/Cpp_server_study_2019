#pragma once

#include <iostream>
#include <math.h>
#include <Windows.h>
#include <Windowsx.h>

enum NODE_COLOR
{
	BLACK = 0,
	RED
};

struct st_NODE
{
	struct st_NODE *pParent;
	struct st_NODE *pLeft;
	struct st_NODE *pRight;

	NODE_COLOR Color;
	int iData;
	int Deapth;
	int index;
	int X;
	int Y;
};

typedef struct PRINT_NODE {
	
	int iData;
	NODE_COLOR Color;
	int X;
	int Y;

}PNODE;

class RedBlackTree
{
public:
	RedBlackTree();
	virtual ~RedBlackTree();

	bool	Insert(int iData, NODE_COLOR iColor = RED);
	bool	Delete(int iData);
	
	void	PrintConsole();
	void	Print();
	int		GetNodeCount(st_NODE *pRoot);
	int		GetNodeCount() { return _iNodeCount; }

	//private:
	void		NilInit();
	st_NODE *	InsertBalance(st_NODE *pNewNode);
	st_NODE *	lnsertNode(st_NODE *pRoot, st_NODE *pNew);
	st_NODE *	deleteNode(st_NODE *pNode, st_NODE *pParent, int iData);
	st_NODE *	deleteNode(st_NODE **pNode, st_NODE **pParent, int iData);
	st_NODE *   deleteBalance(st_NODE *pNode, st_NODE * pParent);
	st_NODE *	findNode(st_NODE *pRoot, int iData);
	st_NODE *	LeftRotation(st_NODE *pPointNode);
	st_NODE *	RightRotation(st_NODE *pPointNode);
	
	void		Inorder(st_NODE *pNowNode);
	void		Consoleorder(st_NODE *pNowNode, int iStartIndex);
	int			ChildNum(st_NODE *pNowNode);
	int			GetMaxDeapth(st_NODE *pRoot);
	int			GetLevel();
	void		MakeNodeDeapth(st_NODE *pNode);
	void		MakeIndex(st_NODE *pRoot);
	void		MakePos(st_NODE *pNode, int iRootIndex);
	void		PrintLine(int NodeX, int NodeY, int ParentX, int ParentY);
	void		PrintAll(st_NODE *pNode);
	void		PrintCircle(RECT rect, NODE_COLOR color);
	void		PrintValue(int NodeX, int NodeY, int iData);
	void		PrintTree();
	void		PrintTree2();

//private:
public:
	st_NODE	_Nil;	// 끝 리프노드. 무조건 블랙 / 데이터 무 / NULL 의 같은 용도.
	st_NODE	*_pRoot;
	int		_iNodeCount;
	PNODE	*_pDataArr;
};


#pragma once
#include <iostream>
#include <Windows.h>
#include <math.h>

#define DUPLICATE_VALUE 

typedef struct st_TREENODE
{
	st_TREENODE(int Data)
	{
		iData = Data;
		pLeft = NULL;
		pRight = NULL;
	}
	struct st_TREENODE *pLeft;
	struct st_TREENODE *pRight;

	int iData;

}TREENODE;


class BSearchTree
{
public:

	BSearchTree();

	~BSearchTree();

	//------------------------------------------------------
	// 새로운 데이터를 추가한다.
	//
	// 중복값 있다면 false, 넣는데 성공이면 true 반환
	//------------------------------------------------------
	bool		InsertNode(int iData);

	//------------------------------------------------------
	// 특정 데이터를 삭제한다.
	//
	// 찾는 데이터가 없다면 false, 지우는데 성공이면 true 반환
	//------------------------------------------------------
	bool		DeleteNode(int iData);

	//------------------------------------------------------
	// 특정 데이터를 검색하여 노드 리턴
	//------------------------------------------------------
	st_TREENODE	*SearchNode(int iData);

	//------------------------------------------------------
	// 트리 전체 삭제
	//------------------------------------------------------
	void		ReleaseTree(void);

	//------------------------------------------------------
	// 출력
	//------------------------------------------------------
	void		PrintTree(st_TREENODE *pNode = NULL, int iDepth = 0, int iLR = 0);
	void		PrintConsole();
	//------------------------------------------------------
	// BSearchTree 잎노드인지 확인
	//
	// 인자 : 노드 포인터
	// 리턴 : 맞으면 true, 틀리면 false
	//------------------------------------------------------
	bool IsLeaf(st_TREENODE *pNode);

	//Getter
	int GetNodeCount() { return _iNodeCount; }
	int GetMaxDeapth() { return _MaxDeapth; }

private:

	//------------------------------------------------------
	// InsertNode 내부에서 호출되는 위치 찾아 추가 재귀 함수
	//
	// 성공실패 리턴
	//------------------------------------------------------
	st_TREENODE *	linkNode(st_TREENODE *pRoot, st_TREENODE *pNew);

	//------------------------------------------------------
	// SearchNode 내부에서 호출되는 노드찾기 재귀함수
	//------------------------------------------------------
	st_TREENODE *	findNode(st_TREENODE *pRoot, int iData);

	//------------------------------------------------------
	// DeleteNode 내부에서 호출되는 노드찾기 & 삭제 & 후처리 재귀함수
	//------------------------------------------------------
	st_TREENODE *	deleteNode(st_TREENODE *pNode, st_TREENODE *pParent, int iData);

	//------------------------------------------------------
	// ReleaseTree 내부에서 호출되는 트리 삭제 재귀함수
	//------------------------------------------------------
	void		releaseNode(st_TREENODE *pRoot);

	//------------------------------------------------------
	// BSearchTree 전체 순회 printf 출력 함수
	//
	// 처음 넣는 인자는 pRootNode
	//------------------------------------------------------
	void		Preorder(st_TREENODE *pNowNode);
	void		Inorder(st_TREENODE *pNowNode);
	void		Postorder(st_TREENODE *pNowNode);
	void		Consoleorder(st_TREENODE *pNowNode, int iStartIndex);

	// deleteNode 내부함수
	int			ChildNum(st_TREENODE *pNowNode);
	
	// 커서 움직이는 코드 필요?

private:
	st_TREENODE	*_pRoot;
	int			_iNodeCount;
	int			_MaxDeapth;
	int			_Deapth;
	int			*_pDataArr;
	int			_iDataIndex;
};

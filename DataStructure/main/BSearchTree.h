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
	// ���ο� �����͸� �߰��Ѵ�.
	//
	// �ߺ��� �ִٸ� false, �ִµ� �����̸� true ��ȯ
	//------------------------------------------------------
	bool		InsertNode(int iData);

	//------------------------------------------------------
	// Ư�� �����͸� �����Ѵ�.
	//
	// ã�� �����Ͱ� ���ٸ� false, ����µ� �����̸� true ��ȯ
	//------------------------------------------------------
	bool		DeleteNode(int iData);

	//------------------------------------------------------
	// Ư�� �����͸� �˻��Ͽ� ��� ����
	//------------------------------------------------------
	st_TREENODE	*SearchNode(int iData);

	//------------------------------------------------------
	// Ʈ�� ��ü ����
	//------------------------------------------------------
	void		ReleaseTree(void);

	//------------------------------------------------------
	// ���
	//------------------------------------------------------
	void		PrintTree(st_TREENODE *pNode = NULL, int iDepth = 0, int iLR = 0);
	void		PrintConsole();
	//------------------------------------------------------
	// BSearchTree �ٳ������ Ȯ��
	//
	// ���� : ��� ������
	// ���� : ������ true, Ʋ���� false
	//------------------------------------------------------
	bool IsLeaf(st_TREENODE *pNode);

	//Getter
	int GetNodeCount() { return _iNodeCount; }
	int GetMaxDeapth() { return _MaxDeapth; }

private:

	//------------------------------------------------------
	// InsertNode ���ο��� ȣ��Ǵ� ��ġ ã�� �߰� ��� �Լ�
	//
	// �������� ����
	//------------------------------------------------------
	st_TREENODE *	linkNode(st_TREENODE *pRoot, st_TREENODE *pNew);

	//------------------------------------------------------
	// SearchNode ���ο��� ȣ��Ǵ� ���ã�� ����Լ�
	//------------------------------------------------------
	st_TREENODE *	findNode(st_TREENODE *pRoot, int iData);

	//------------------------------------------------------
	// DeleteNode ���ο��� ȣ��Ǵ� ���ã�� & ���� & ��ó�� ����Լ�
	//------------------------------------------------------
	st_TREENODE *	deleteNode(st_TREENODE *pNode, st_TREENODE *pParent, int iData);

	//------------------------------------------------------
	// ReleaseTree ���ο��� ȣ��Ǵ� Ʈ�� ���� ����Լ�
	//------------------------------------------------------
	void		releaseNode(st_TREENODE *pRoot);

	//------------------------------------------------------
	// BSearchTree ��ü ��ȸ printf ��� �Լ�
	//
	// ó�� �ִ� ���ڴ� pRootNode
	//------------------------------------------------------
	void		Preorder(st_TREENODE *pNowNode);
	void		Inorder(st_TREENODE *pNowNode);
	void		Postorder(st_TREENODE *pNowNode);
	void		Consoleorder(st_TREENODE *pNowNode, int iStartIndex);

	// deleteNode �����Լ�
	int			ChildNum(st_TREENODE *pNowNode);
	
	// Ŀ�� �����̴� �ڵ� �ʿ�?

private:
	st_TREENODE	*_pRoot;
	int			_iNodeCount;
	int			_MaxDeapth;
	int			_Deapth;
	int			*_pDataArr;
	int			_iDataIndex;
};

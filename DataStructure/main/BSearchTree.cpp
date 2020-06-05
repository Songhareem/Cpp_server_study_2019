
#include "pch.h"
#include "BSearchTree.h"

BSearchTree::BSearchTree() {

	_pRoot = NULL;
	_iNodeCount = 0;
	_MaxDeapth = 1;
	_Deapth = 1;
}

BSearchTree::~BSearchTree() {

	ReleaseTree();
}

// public
bool BSearchTree::InsertNode(int iData) {

	int iOldCount = _iNodeCount;
	TREENODE *pNewNode = new TREENODE(iData);
	_pRoot = linkNode(_pRoot, pNewNode);
	int iNewCount = _iNodeCount;

	// linkNode�� NodeCount���� �������� �ʾҴٸ� false
	if (iOldCount == iNewCount)
		return false;

	// �ִ� ���� ����
	if (_Deapth > _MaxDeapth)
		_MaxDeapth = _Deapth;

	_Deapth = 1;

	return true;
}

bool BSearchTree::DeleteNode(int iData) {
	
	int OldNodeCount = _iNodeCount;
	if (iData < _pRoot->iData)
		_pRoot = deleteNode(_pRoot->pLeft, _pRoot, iData);
	else if (iData > _pRoot->iData)
		_pRoot = deleteNode(_pRoot->pRight, _pRoot, iData);
	int NewNodeCount = _iNodeCount;

	if( OldNodeCount == NewNodeCount)
		return false;

	return true;
}

st_TREENODE	*BSearchTree::SearchNode(int iData) {

	TREENODE *pFoundNode = findNode(_pRoot,iData);

	return pFoundNode;
}

void BSearchTree::ReleaseTree(void) {

	releaseNode(_pRoot);
	delete _pRoot;
	_pRoot = NULL;
	_MaxDeapth = 0;
}

void BSearchTree::PrintTree(st_TREENODE *pNode, int iDepth, int iLR) {

	Inorder(_pRoot);
}

bool BSearchTree::IsLeaf(st_TREENODE *pNode) {

	TREENODE *pNodeCopy = pNode;

	// Root�� NULL�̶�� �Լ� ���Ǹ� �ȵ�
	if (_pRoot == NULL)
		return false;

	// Root�� �ּҿ� pNode�� �ּҰ� ���ٸ� ��Ʈ ���
	if (_pRoot == pNode)
		return false;

	// Root����� �ּҿ� �ٸ���, �� �ڽĳ�尡 NULL�̶�� Leaf���
	if (pNode->pLeft == NULL && pNode->pRight == NULL)
		return true;

	return false;
}

// private
st_TREENODE *BSearchTree::linkNode(st_TREENODE *pRoot, st_TREENODE *pNew) {

	// pRoot�� NULL�̸� �� ��尡 Insert �ؾ��� ���
	if (pRoot == NULL) {
		_iNodeCount += 1;
		return pNew;
	}

	if (pNew->iData == pRoot->iData)
		return pRoot;

	// ��� Ÿ�� ������ ���̹Ƿ�
	_Deapth++;

	if (pNew->iData < pRoot->iData)
		pRoot->pLeft = linkNode(pRoot->pLeft, pNew);

	else if (pNew->iData > pRoot->iData)
		pRoot->pRight = linkNode(pRoot->pRight, pNew);

	return pRoot;
}

st_TREENODE *BSearchTree::findNode(st_TREENODE *pRoot, int iData) {

	if (pRoot == NULL)
		return NULL;

	TREENODE *pTmpNode=NULL;

	if (iData == pRoot->iData)
		pTmpNode = pRoot;
	else if (iData < pRoot->iData)
		pTmpNode = findNode(pRoot->pLeft, iData);
	else if(iData > pRoot->iData)
		pTmpNode = findNode(pRoot->pRight, iData);

	return pTmpNode;
}

st_TREENODE *BSearchTree::deleteNode(st_TREENODE *pNode, st_TREENODE *pParent, int iData) {

	TREENODE *pDeleteNode = findNode(pNode, iData);
	if (pDeleteNode == NULL)
		return pParent;
	
	TREENODE *pDeleteParent = pParent;

	for(;;) {
		if (pDeleteParent->pLeft == pDeleteNode || pDeleteParent->pRight == pDeleteNode)
			break;
		
		if (iData < pDeleteParent->iData)
			pDeleteParent = pDeleteParent->pLeft;
		else if(iData > pDeleteParent->iData)
			pDeleteParent = pDeleteParent->pRight;
	}

	if (ChildNum(pDeleteNode) == 0) {

		if (pDeleteParent->pLeft == pDeleteNode)
			pDeleteParent->pLeft = NULL;
		else if (pDeleteParent->pRight == pDeleteNode)
			pDeleteParent->pRight = NULL;

		delete pDeleteNode;
	}
	
	else if (ChildNum(pDeleteNode) == 1) {

		TREENODE *pNextChild = NULL;
		if (pDeleteNode->pLeft != NULL)
			pNextChild = pDeleteNode->pLeft;
		else if (pDeleteNode->pRight != NULL)
			pNextChild = pDeleteNode->pRight;

		if (pDeleteParent->pLeft == pDeleteNode)
			pDeleteParent->pLeft = pNextChild;
		else if (pDeleteParent->pRight == pDeleteNode)
			pDeleteParent->pRight = pNextChild;

		delete pDeleteNode;
	}

	else if (ChildNum(pDeleteNode) == 2) {

		TREENODE *pChangeNode = pDeleteNode->pLeft;

		if (pChangeNode->pRight == NULL) {

			pDeleteParent->pLeft = pChangeNode;
			pChangeNode->pRight = pDeleteNode->pRight;

			delete pDeleteNode;
		}

		else {
			
			// ������ �ٳ�� ã��
			for (;;) {
				if (pChangeNode->pRight == NULL)
					break;

				pChangeNode = pChangeNode->pRight;
			}
			
			// �ٳ�� �θ��� ã��
			TREENODE *pChangeParent = pDeleteNode->pLeft;

			for(;;) {
				if (pChangeParent->pRight == pChangeNode)
					break;

				pChangeParent = pChangeParent->pRight;
			}

			int breakPoint = 0;

			pDeleteNode->iData = pChangeNode->iData;
			pChangeParent->pRight = pChangeNode->pLeft;
			delete pChangeNode;
		}
	}
	
	_iNodeCount -= 1;
	return pParent;
}

void BSearchTree::releaseNode(st_TREENODE *pRoot) {

	if (pRoot == NULL)
		return;

	releaseNode(pRoot->pLeft);
	releaseNode(pRoot->pRight);

	if (pRoot != _pRoot)
		delete pRoot;

	else {
		pRoot->pLeft = NULL;
		pRoot->pRight = NULL;
	}

	//int breakPoint = 0;
}


// printf
void BSearchTree::Consoleorder(st_TREENODE *pNowNode, int iStartIndex) {

	/*if (pNowNode == NULL) {
		_pDataArr[iStartIndex - 1] = 0;
		return;
	}
	_iDataIndex++;
	*/
	_pDataArr[iStartIndex - 1] = pNowNode->iData;
	if(pNowNode->pLeft != NULL)
		Consoleorder(pNowNode->pLeft, iStartIndex * 2);
	if(pNowNode->pRight != NULL)
		Consoleorder(pNowNode->pRight, iStartIndex * 2 + 1);
}

void BSearchTree::Preorder(st_TREENODE *pNowNode) {

	if (pNowNode == NULL) {
		return;
	}
	
	printf("Data : %02d\n", pNowNode->iData);
	Preorder(pNowNode->pLeft);
	Preorder(pNowNode->pRight);
}

void BSearchTree::Inorder(st_TREENODE *pNowNode) {

	if (pNowNode == NULL)
		return;

	Inorder(pNowNode->pLeft);
	printf("Data : %02d\n", pNowNode->iData);
	Inorder(pNowNode->pRight);
}

void BSearchTree::Postorder(st_TREENODE *pNowNode) {

	if (pNowNode == NULL)
		return;

	Postorder(pNowNode->pLeft);
	Postorder(pNowNode->pRight);
	printf("Data : %02d\n", pNowNode->iData);
}

void BSearchTree::PrintConsole() {

	int ArrSize = (int)pow((double)2, (double)_MaxDeapth) - 1;
	_pDataArr = new int[ArrSize];
	ZeroMemory(_pDataArr, sizeof(int)*ArrSize);
	int iDataIndex = 1;
	
	Consoleorder(_pRoot, iDataIndex);

	//for (int i = 0; i < ArrSize; i++)
		//printf("%d ", _pDataArr[i]);
	
	int index = 0;
	printf("\n");
	for (int i = 1; i <= _MaxDeapth; i++) {

		for (int j = 1; j <= pow(2, i - 1); j++) {

			int powDeapth = pow(2, (_MaxDeapth + 1) - i);

			if(j == 1) {
				for (int gap = 1; gap <= powDeapth; gap++) printf(" ");
			}
			for (int gap = 1; gap <= powDeapth; gap++) printf(" ");
			printf("[%02d]", _pDataArr[index++]);
			for (int gap = 1; gap <= powDeapth; gap++) printf(" ");
		}

		printf("\n\n");
	}

	delete _pDataArr;
}

int	BSearchTree::ChildNum(st_TREENODE *pNowNode) {

	int NumOfChild = 0;

	if (pNowNode->pLeft != NULL)
		NumOfChild += 1;
	
	if (pNowNode->pRight != NULL)
		NumOfChild += 1;

	return NumOfChild;
}

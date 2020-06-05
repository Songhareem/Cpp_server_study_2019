#include "stdafx.h"
#include "RedBlackTree.h"
#include "Resource.h"
#include <Windows.h>


RedBlackTree::RedBlackTree()
{
	NilInit();

	_pRoot = &_Nil;
	_iNodeCount = 0;
}


RedBlackTree::~RedBlackTree()
{
}

bool RedBlackTree::Insert(int iData, NODE_COLOR iColor) {

	// �̹� ���� �����Ѵٸ� false
	st_NODE *pThereIs = findNode(_pRoot, iData);
	if (pThereIs != &_Nil)
		return false;
	
	//NewNode ����
	st_NODE *pNewNode = new st_NODE;
	
	// ������ ����
	pNewNode->iData = iData;
	pNewNode->Color = iColor;
	pNewNode->pParent = NULL;
	pNewNode->pLeft = &_Nil;
	pNewNode->pRight = &_Nil;
	
	// ��� ����
	_pRoot = lnsertNode(_pRoot, pNewNode);
	int iNewCount = _iNodeCount;

	//// TEST DATA
	//st_NODE *pNewNode2 = new st_NODE;
	//pNewNode2->iData = 40;
	//pNewNode2->Color = iColor;
	//pNewNode2->pParent = NULL;
	//pNewNode2->pLeft = &_Nil;
	//pNewNode2->pRight = &_Nil;
	//_pRoot = lnsertNode(_pRoot, pNewNode2);

	//st_NODE *pNewNode3 = new st_NODE;
	//pNewNode3->iData = 80;
	//pNewNode3->Color = RED;
	//pNewNode3->pParent = NULL;
	//pNewNode3->pLeft = &_Nil;
	//pNewNode3->pRight = &_Nil;
	//_pRoot = lnsertNode(_pRoot, pNewNode3);

	//st_NODE *pNewNode4 = new st_NODE;
	//pNewNode4->iData = 20;
	//pNewNode4->Color = iColor;
	//pNewNode4->pParent = NULL;
	//pNewNode4->pLeft = &_Nil;
	//pNewNode4->pRight = &_Nil;
	//_pRoot = lnsertNode(_pRoot, pNewNode4);
	//
	//_pRoot->Color = BLACK;
	//_pRoot = InsertBalance(pNewNode4);
	

	// ��� �߰� �ƴٸ�, �뷱�� ���
	_pRoot = InsertBalance(pNewNode);
	_pRoot->Color = BLACK;

	return true;
}

bool RedBlackTree::Delete(int iData) {

	// findNode ���� _Nil�̶�� ���� ���� �����Ƿ� false;
	st_NODE *pDeleteNode = findNode(_pRoot, iData);
	if (pDeleteNode == &_Nil)
		return false;
	
	st_NODE *pDeleteParent = pDeleteNode->pParent;
	bool isBLACK = false;
	if (pDeleteNode->Color == BLACK)
		isBLACK = true;
	
	// delete
	_pRoot = deleteNode(&pDeleteNode, &pDeleteParent, iData);
	
	// ���� ��尡 BLACK �̶��, ������ ��� �ڸ� ���� �뷱�� ���߱�
	//if (isBLACK == true)
		//DeleteBalance(pDeleteNode, pDeleteParent);

	return true;
}

int	RedBlackTree::GetNodeCount(st_NODE *pRoot) {

	if (pRoot == &_Nil)
		return 0;

	int iNodeCount = 1;
	iNodeCount += GetNodeCount(pRoot->pLeft) + GetNodeCount(pRoot->pRight);
	return iNodeCount;
}

void RedBlackTree::NilInit() {

	_Nil.Color = BLACK;
	_Nil.pParent = NULL;
	_Nil.pLeft = NULL;
	_Nil.pRight = NULL;
}

st_NODE *RedBlackTree::lnsertNode(st_NODE *pRoot, st_NODE *pNew) {

	st_NODE *pRootOrigin = _pRoot;

	// pRoot�� NULL�̸� �� ��尡 Insert �ؾ��� ���
	if (pRoot == &_Nil) {
		_iNodeCount += 1;
		pRoot = pNew;
		return pNew;
	}

	// ���� ����
	if (pNew->iData == pRoot->iData)
		return _pRoot;

	// ��� Ÿ�� ������ ���̹Ƿ�
	pNew->pParent = pRoot;
	if (pNew->iData < pRoot->iData) {
		pRoot->pLeft = lnsertNode(pRoot->pLeft, pNew);
	}
	else if (pNew->iData > pRoot->iData) {
		pRoot->pRight = lnsertNode(pRoot->pRight, pNew);
	}
	
	return pRoot;
}

st_NODE *RedBlackTree::InsertBalance(st_NODE *pNewNode) {

	for (;;) {

		// ���� ��尡 Root �����, return Root���
		if (pNewNode->pParent == NULL)
			return pNewNode;

		// �θ��� ���� ���尡 �ƴϰų� �θ� ��Ʈ��� ��Ʈ ��ȯ
		st_NODE *pParent = pNewNode->pParent;
		if (pParent->pParent == NULL || pParent->Color != RED) {
			for (;;) {
				if (pParent->pParent == NULL)
					return pParent;
				
				pParent = pParent->pParent;
			}
		}
		st_NODE *pGrand = pParent->pParent;
		st_NODE *pUncle = NULL;

		// �θ� ������ ���ʿ� ���� ��
		if (pGrand->pLeft == pParent) {
			pUncle = pGrand->pRight;

			// �θ� RED, ���� RED
			if (pParent->Color == RED && pUncle->Color == RED) {

				pGrand->Color = RED;
				pParent->Color = BLACK;
				pUncle->Color = BLACK;

				pNewNode = pGrand;
			}

			// �θ� RED, ���� BLACK
			else if (pParent->Color == RED && pUncle->Color == BLACK) {

				// �� �ڽĳ�尡 �θ��� �����ʿ� ����
				if (pParent->pRight == pNewNode) {

					pParent = LeftRotation(pParent);
					pNewNode = pParent->pLeft;
				}

				// �� �ڽĳ�尡 �θ��� ���ʿ� ����
				if (pParent->pLeft == pNewNode) {

					pParent->Color = BLACK;
					pGrand->Color = RED;
					pGrand = RightRotation(pGrand);
					pNewNode = pGrand->pLeft;
				}
			}
		}

		// �θ� ������ �����ʿ� ���� ��
		else if (pGrand->pRight == pParent) {
			pUncle = pGrand->pLeft;

			// �θ� RED, ���� RED
			if (pParent->Color == RED && pUncle->Color == RED) {

				pGrand->Color = RED;
				pParent->Color = BLACK;
				pUncle->Color = BLACK;

				pNewNode = pGrand;
			}

			// �θ� RED, ���� BLACK
			else if (pParent->Color == RED && pUncle->Color == BLACK) {

				// �� �ڽĳ�尡 �θ��� �����ʿ� ����
				if (pParent->pLeft == pNewNode) {

					pParent = RightRotation(pParent);
					pNewNode = pParent->pRight;
				}

				// �� �ڽĳ�尡 �θ��� ���ʿ� ����
				if (pParent->pRight == pNewNode) {

					pParent->Color = BLACK;
					pGrand->Color = RED;
					pGrand = LeftRotation(pGrand);
					pNewNode = pGrand->pRight;
				}
			}
		}
	}
}

// ��ü ���� pNode�� ������ ��� �θ�� pParent��, return ���� _Root���
st_NODE *RedBlackTree::deleteNode(st_NODE *pNode, st_NODE *pParent, int iData) {

	// Ʈ���� �������
	if (pNode == &_Nil)
		return pNode;

	if (ChildNum(pNode) == 0) {

		if (pParent->pLeft == pNode) {
			delete pNode;
			pParent->pLeft = &_Nil;
			pNode = pParent->pLeft;
		}
		else if (pParent->pRight == pNode) {
			delete pNode;
			pParent->pRight = &_Nil;
			pNode = pParent->pRight;
		}
	}

	else if (ChildNum(pNode) == 1) {

		st_NODE *pNextChild = &_Nil;
		if (pNode->pLeft != &_Nil)
			pNextChild = pNode->pLeft;
		else if (pNode->pRight != &_Nil)
			pNextChild = pNode->pRight;

		if (pParent->pLeft == pNode)
			pParent->pLeft = pNextChild;
		else if (pParent->pRight == pNode)
			pParent->pRight = pNextChild;

		delete pNode;
		pNode = pNextChild;
	}

	else if (ChildNum(pNode) == 2) {

		st_NODE *pChangeNode = pNode->pLeft;

		if (pChangeNode->pRight == &_Nil) {

			pParent->pLeft = pChangeNode;
			pChangeNode->pRight = pNode->pRight;

			delete pNode;
			pNode = pChangeNode;
		}

		else {

			// ������ �ٳ�� ã��
			for (;;) {
				if (pChangeNode->pRight == &_Nil)
					break;

				pChangeNode = pChangeNode->pRight;
			}

			// �ٳ�� �θ��� ã��
			/*st_NODE *pChangeParent = pNode->pLeft;

			for (;;) {
				if (pChangeParent->pRight == pChangeNode)
					break;

				pChangeParent = pChangeParent->pRight;
			}
			*/
			st_NODE *pChangeParent = pChangeNode->pParent;
			
			int breakPoint = 0;

			pNode->iData = pChangeNode->iData;
			pChangeParent->pRight = pChangeNode->pLeft;
			pNode = pChangeParent->pRight;
			pParent = pChangeParent;
			delete pChangeNode;
		}
	}

	_iNodeCount -= 1;

	// return pRoot
	st_NODE *pRoot = pParent;
	for (;;) {
		if (pRoot->pParent == NULL)
			break;

		pRoot = pRoot->pParent;	
	}

	return pRoot;
}

st_NODE *RedBlackTree::deleteNode(st_NODE **pNode, st_NODE **pParent, int iData) {

	// Ʈ���� �������
	if ((*pNode) == &_Nil)
		return (*pNode);

	if (ChildNum(*pNode) == 0) {

		if ((*pParent)->pLeft == *pNode) {
			delete *pNode;
			(*pParent)->pLeft = &_Nil;
			*pNode = (*pParent)->pLeft;
			(*pNode)->pParent = (*pParent);
		}
		else if ((*pParent)->pRight == *pNode) {
			delete *pNode;
			(*pParent)->pRight = &_Nil;
			*pNode = (*pParent)->pRight;
			(*pNode)->pParent = (*pParent);
		}
	}

	else if (ChildNum(*pNode) == 1) {

		st_NODE *pNextChild = &_Nil;
		if ((*pNode)->pLeft != &_Nil)
			pNextChild = (*pNode)->pLeft;
		else if ((*pNode)->pRight != &_Nil)
			pNextChild = (*pNode)->pRight;

		if ((*pParent)->pLeft == (*pNode))
			(*pParent)->pLeft = pNextChild;
		else if ((*pParent)->pRight == (*pNode))
			(*pParent)->pRight = pNextChild;

		pNextChild->pParent = (*pParent);
		delete *pNode;
		*pNode = pNextChild;
	}

	else if (ChildNum(*pNode) == 2) {

		st_NODE *pChangeNode = (*pNode)->pLeft;

		if (pChangeNode->pRight == &_Nil) {

			if((*pParent)->pLeft == (*pNode))
				(*pParent)->pLeft = pChangeNode;
			else if((*pParent)->pRight == (*pNode))
				(*pParent)->pRight = pChangeNode;
			
			pChangeNode->pParent = (*pParent);
			pChangeNode->pRight = (*pNode)->pRight;

			delete *pNode;
			*pNode = pChangeNode;
		}

		else {

			// ������ �ٳ�� ã��
			for (;;) {
				if (pChangeNode->pRight == &_Nil)
					break;

				pChangeNode = pChangeNode->pRight;
			}

			st_NODE *pChangeParent = pChangeNode->pParent;

			//int breakPoint = 0;

			(*pNode)->iData = pChangeNode->iData;
			pChangeParent->pRight = pChangeNode->pLeft;
			*pNode = pChangeParent->pRight;
			(*pNode)->pParent = pChangeParent;
			(*pParent) = pChangeParent;
			delete pChangeNode;
		}
	}

	_iNodeCount -= 1;

	// return pRoot
	st_NODE *pRoot = (*pParent);
	for (;;) {
		if (pRoot->pParent == NULL)
			break;

		pRoot = pRoot->pParent;
	}

	return pRoot;
}

st_NODE *RedBlackTree::deleteBalance(st_NODE *pNode, st_NODE * pParent) {

	for (;;) {

		// Replace��尡 Root��� ����
		if (pNode->pParent == NULL)
			return pNode;

		// Root��尡 RED ��� BLACK���� �ٲٰ� ��
		if (pNode->Color == RED) {
			pNode->Color = BLACK;
			for (;;) {
				if (pParent->pParent == NULL)
					return pParent;

				pParent = pParent->pParent;
			}
		}

		st_NODE *pSibling = NULL;

		// Replace��尡 ������ ��,
		if (pParent->pLeft == pNode) {
			pSibling = pParent->pRight;

			//���� ��尡 Red���
			if (pSibling->Color == RED) {

				pSibling->Color = BLACK;
				pParent->Color = RED;
				LeftRotation(pParent);
				pParent = pNode->pParent;

				continue;
			}

			// ���� ��尡 ��
			if (pSibling->Color == BLACK) {

				//���� ��� �� �ڽ��� ��� ��
				if (pSibling->pLeft->Color == BLACK &&
					pSibling->pRight->Color == BLACK) {

					pSibling->Color = RED;
					pNode = pParent;
					pParent = pNode->pParent;

					continue;
				}

				// ���� ��� ���ڽ� ����
				if (pSibling->pLeft->Color == RED) {
					pSibling->Color = RED;
					pSibling->pLeft->Color = BLACK;
					pSibling = RightRotation(pSibling);
				}

				// ���� ��� �����ڽ� ����
				if (pSibling->pRight->Color == RED) {
					//pSibling�� ����� �ٲ������ Ȯ�� + BLACK���� Ȯ���ؾ���
					pSibling->Color = pParent->Color;
					pSibling->pRight->Color = BLACK;
					pParent->Color = BLACK;
					LeftRotation(pParent);

					// ����尡 ����� �ö󰡴� �� Ȯ���غ�����
					pNode = _pRoot;
				}
			}
		}
		// Replace��尡 �������� ��,
		/*else if (pParent->pRight == pNode) {
		pSibling = pParent->pLeft;
		}*/
	}
}

st_NODE *RedBlackTree::findNode(st_NODE *pRoot, int iData) {

	if (pRoot == &_Nil)
		return &_Nil;

	st_NODE *pTmpNode = &_Nil;

	if (iData == pRoot->iData)
		pTmpNode = pRoot;
	else if (iData < pRoot->iData)
		pTmpNode = findNode(pRoot->pLeft, iData);
	else if (iData > pRoot->iData)
		pTmpNode = findNode(pRoot->pRight, iData);

	return pTmpNode;
}

void RedBlackTree::Inorder(st_NODE *pNowNode) {

	if (pNowNode == &_Nil)
		return;

	Inorder(pNowNode->pLeft);
	printf("Data : %02d\n", pNowNode->iData);
	Inorder(pNowNode->pRight);
}

void RedBlackTree::Print() {

	Inorder(_pRoot);
}

void RedBlackTree::PrintConsole() {

	int MaxDeapth = GetMaxDeapth(_pRoot);
	int ArrSize = (int)pow((double)2, (double)MaxDeapth) - 1;
	_pDataArr = new PNODE[ArrSize];
	ZeroMemory(_pDataArr, sizeof(PNODE)*ArrSize);
	int iDataIndex = 1;

	Consoleorder(_pRoot, iDataIndex);
	//for (int i = 0; i < ArrSize; i++)
		//printf("%d ", _pDataArr[i]);

	int index = 0;
	printf("\n");
	for (int i = 1; i <= MaxDeapth; i++) {

		for (int j = 1; j <= pow(2, i - 1); j++) {

			int powDeapth = pow(2, (MaxDeapth + 1 - i)) - 1;

			/*if (j == 1) {
				for (int gap = 1; gap <= powDeapth; gap++) printf(" ");
			}*/

			for (int gap = 1; gap <= powDeapth; gap++) printf(" ");
			if(_pDataArr[index].Color == BLACK)
				printf("B %02d", _pDataArr[index++]);	//Black
			else
				printf("R %02d", _pDataArr[index++]);	//Red
			for (int gap = 1; gap <= powDeapth; gap++) printf(" ");
		}

		printf("\n\n");
	}

	delete _pDataArr;
}

void RedBlackTree::Consoleorder(st_NODE *pNowNode, int iStartIndex) {

	_pDataArr[iStartIndex - 1].iData = pNowNode->iData;
	_pDataArr[iStartIndex - 1].Color = pNowNode->Color;
	if (pNowNode->pLeft != &_Nil)
		Consoleorder(pNowNode->pLeft, iStartIndex * 2);
	if (pNowNode->pRight != &_Nil)
		Consoleorder(pNowNode->pRight, iStartIndex * 2 + 1);
}

st_NODE *RedBlackTree::LeftRotation(st_NODE *pPointNode) {

	st_NODE *pRightNode = pPointNode->pRight;
	st_NODE *pRightLeftBranch = pRightNode->pLeft;
	st_NODE *pPointParent = pPointNode->pParent;
	
	// PointNode�� ��Ʈ��尡 �ƴ϶��
	if (pPointNode != _pRoot) {
		if (pPointParent->pLeft == pPointNode)
			pPointParent->pLeft = pRightNode;
		else if (pPointParent->pRight == pPointNode)
			pPointParent->pRight = pRightNode;
	}

	pRightNode->pParent = pPointNode->pParent;

	// LeftNode�� ���� ������ �������� �� ������
	pPointNode->pRight = pRightLeftBranch;
	pRightLeftBranch->pParent = pPointNode;

	// LeftNode�� �θ��
	pRightNode->pLeft = pPointNode;
	pPointNode->pParent = pRightNode;
	
	// �� �������� ����
	return pRightNode;
}

st_NODE *RedBlackTree::RightRotation(st_NODE *pPointNode) {

	st_NODE *pLeftNode = pPointNode->pLeft;
	st_NODE *pLeftRightBranch = pLeftNode->pRight;
	st_NODE *pPointParent = pPointNode->pParent;

	// PointNode�� ��Ʈ��尡 �ƴ϶��
	if (pPointNode != _pRoot) {
		if (pPointParent->pLeft == pPointNode)
			pPointParent->pLeft = pLeftNode;
		else if (pPointParent->pRight == pPointNode)
			pPointParent->pRight = pLeftNode;
	}

	pLeftNode->pParent = pPointNode->pParent;

	// LeftNode�� ���� ������ �������� �� ������
	pPointNode->pLeft = pLeftRightBranch;
	pLeftRightBranch->pParent = pPointNode;

	// LeftNode�� �θ��
	pLeftNode->pRight = pPointNode;
	pPointNode->pParent = pLeftNode;

	// �� �������� ����
	return pLeftNode;
}

int	RedBlackTree::ChildNum(st_NODE *pNowNode) {

	int NumOfChild = 0;

	if (pNowNode->pLeft != &_Nil)
		NumOfChild += 1;

	if (pNowNode->pRight != &_Nil)
		NumOfChild += 1;

	return NumOfChild;
}

// ����� ���� ����ʿ� ( Nil ��� ������ level�� �ƴ� Deapth ������)
int RedBlackTree::GetMaxDeapth(st_NODE *pRoot) {

	if (pRoot == &_Nil)
		return 0;

	int LeftDeapth = GetMaxDeapth(pRoot->pLeft);
	int RightDeapth = GetMaxDeapth(pRoot->pRight);

	if (LeftDeapth > RightDeapth)
		return LeftDeapth + 1;
	else
		return RightDeapth + 1;
}

int	RedBlackTree::GetLevel() {

	int NowDeapth = GetMaxDeapth(_pRoot);

	return NowDeapth - 1;
}



void RedBlackTree::PrintLine(int NodeX, int NodeY, int ParentX, int ParentY) {

	HDC hDC = GetDC(g_hWnd);
	MoveToEx(hDC, NodeX, NodeY, NULL);
	LineTo(hDC, ParentX, ParentY);
	ReleaseDC(g_hWnd, hDC);
}

void RedBlackTree::PrintCircle(RECT rect, NODE_COLOR color) {

	HDC hDC = GetDC(g_hWnd);

	HPEN MyPen = NULL;
	HPEN OldPen = NULL;
	HBRUSH MyBrush = NULL;
	HBRUSH OldBrush = NULL;

	if (color == BLACK) {
		MyPen = CreatePen(PS_SOLID, 5, RGB(0, 0, 0));
		MyBrush = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	}
	else if(color == RED) {
		MyPen = CreatePen(PS_SOLID, 5, RGB(255, 0, 0));
		MyBrush = (HBRUSH)CreateSolidBrush(RGB(255, 0, 0));
	}
	OldPen = (HPEN)SelectObject(hDC, MyPen);
	OldBrush = (HBRUSH)SelectObject(hDC, MyBrush);
	Ellipse(hDC, rect.left, rect.top, rect.right, rect.bottom);
	SelectObject(hDC, OldPen);
	DeleteObject(MyPen);
	SelectObject(hDC, OldBrush);
	DeleteObject(MyBrush);
	ReleaseDC(g_hWnd, hDC);
}

void RedBlackTree::PrintValue(int NodeX, int NodeY, int iData) {

	HDC hDC = GetDC(g_hWnd);
	HFONT font = NULL;
	HFONT oldFont = NULL;

	WCHAR Text[3];
	wsprintf(Text, L"%d", iData);
	font = CreateFont(20, 0, 0, 0, 600, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, 0);
	oldFont = (HFONT)SelectObject(hDC, font);
	SetTextColor(hDC, RGB(255, 255, 255));
	SetBkColor(hDC, TRANSPARENT);
	TextOut(hDC, NodeX-10, NodeY-10, Text, wcslen(Text));

	SelectObject(hDC, oldFont);
	DeleteObject(font);
	ReleaseDC(g_hWnd, hDC);
}

void RedBlackTree::PrintAll(st_NODE *pNode) {

	if (pNode == &_Nil)
		return;

	if (pNode->pLeft != &_Nil)
		pNode = pNode->pLeft;
	if (pNode->pRight != &_Nil)
		pNode = pNode->pRight;

	if(pNode->pParent != NULL)
		PrintLine(pNode->X, pNode->Y, pNode->pParent->X, pNode->pParent->Y);
	
	RECT rect;
	rect.left = pNode->X - 20;
	rect.right = pNode->X + 20;
	rect.top = pNode->X - 20;
	rect.bottom = pNode->X + 20;
	PrintCircle(rect, pNode->Color);
	PrintValue(pNode->X, pNode->Y, pNode->iData);
}

void RedBlackTree::PrintTree() {

	int MaxDeapth = GetMaxDeapth(_pRoot);
	int ArrSize = (int)pow((double)2, (double)MaxDeapth) - 1;
	_pDataArr = new PNODE[ArrSize];
	ZeroMemory(_pDataArr, sizeof(PNODE)*ArrSize);
	int iDataIndex = 1;

	Consoleorder(_pRoot, iDataIndex);
	//for (int i = 0; i < ArrSize; i++)
		//printf("%d ", _pDataArr[i]);

	int index = 0;
	printf("\n");
	for (int i = 1; i <= MaxDeapth; i++) {

		for (int j = 1; j <= pow(2, i - 1); j++) {

			if (_pDataArr[index].iData == 0)
				continue;
			
			_pDataArr[index].X = MAIN_WIDTH/(pow(2, i - 1) + 1) * j;
			_pDataArr[index].Y = i*GAPY;
			int ParentX = 0;
			int ParentY = 0;
		
			//Root �ƴ϶�� ���α׸���
			/*if (index != 0) {
				ParentX = MAIN_WIDTH / i * j;
				ParentY = (i - 1)*GAPY;
				PrintLine(_pDataArr[index].X, _pDataArr[index].Y, ParentX, ParentY);
			}*/

			RECT rect;
			rect.left = (_pDataArr[index].X) - 30;
			rect.top = (_pDataArr[index].Y) - 30;
			rect.right = (_pDataArr[index].X) + 30;
			rect.bottom = (_pDataArr[index].Y) + 30;
			PrintCircle(rect, _pDataArr[index].Color);

			PrintValue(_pDataArr[index].X - 2, _pDataArr[index].Y - 2, _pDataArr[index].iData);
			index++;
		}
	}
	
	delete _pDataArr;
}

void RedBlackTree::MakeNodeDeapth(st_NODE *pNode) {

	if (pNode == &_Nil)
		return;
	
	int iDeapth = 1;
	st_NODE *pFinder = pNode;
	for (;;) {
		if (pFinder->pParent == NULL)
			break;

		pFinder = pFinder->pParent;
		iDeapth += 1;
	}

	pNode->Deapth = iDeapth;
	MakeNodeDeapth(pNode->pLeft);
	MakeNodeDeapth(pNode->pRight);
}

void RedBlackTree::MakeIndex(st_NODE *pRoot) {

	static int index = 0;
	if (pRoot->pParent == NULL)
		index = 1;

	if (pRoot == &_Nil)
		return;

	if(pRoot->pLeft != &_Nil)
		MakeIndex(pRoot->pLeft);
	pRoot->index = index++;
	if (pRoot->pRight != &_Nil)
		MakeIndex(pRoot->pRight);

	//int breakPoint = 0;
}

void RedBlackTree::MakePos(st_NODE *pNode, int iRootIndex) {

	if (pNode == &_Nil)
		return;
	
	if (pNode->pLeft != &_Nil)
		MakeIndex(pNode->pLeft);
	if (pNode->pRight != &_Nil)
		MakeIndex(pNode->pRight);

	pNode->X = ROOTX - (pNode->index - iRootIndex)*GAPX;
	pNode->Y = (pNode->Deapth)*GAPY;
}

void RedBlackTree::RedBlackTree::PrintTree2() {

	MakeNodeDeapth(_pRoot);

	MakeIndex(_pRoot);

	int RootIndex = _pRoot->index;
	MakePos(_pRoot, RootIndex);

	PrintAll(_pRoot);
}
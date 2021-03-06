

#include "pch.h"
#include <iostream>
#include "BSearchTree.h"
#include "RedBlackTree.h"

#pragma comment(lib,"winmm.lib")

BSearchTree BSTree;
RedBlackTree RBTree;
int g_Key = 0;
int g_Val = 1;

void BSearchTreeTest();
void RBTreeTest();
void GetKey();
void TestAll();

int main()
{
	//BSearchTreeTest();
	//RBTreeTest();
	TestAll();

	return 0;
}

void BSearchTreeTest() {

	/*BSTree.InsertNode(100);
	BSTree.InsertNode(60);
	BSTree.InsertNode(40);
	BSTree.InsertNode(20);
	BSTree.InsertNode(10);
	BSTree.InsertNode(25);
	BSTree.InsertNode(55);
	BSTree.InsertNode(52);
	BSTree.InsertNode(70);
	BSTree.InsertNode(65);
	BSTree.InsertNode(80);
	BSTree.InsertNode(75);
	BSTree.InsertNode(120);
*/
	//BSTree.DeleteNode(60);
	
	//BSTree.ReleaseTree();

	//BSTree.PrintTree();
	
	BSTree.InsertNode(60);
	BSTree.InsertNode(40);
	BSTree.InsertNode(50);
	BSTree.InsertNode(30);
	BSTree.InsertNode(80);
	BSTree.InsertNode(70);
	BSTree.InsertNode(90);

	BSTree.PrintConsole();

	printf("NodeCount : %d\n", BSTree.GetNodeCount());
	printf("Deapth : %d\n", BSTree.GetMaxDeapth());
}

void RBTreeTest() {

	RBTree.Insert(60);
	RBTree.Insert(40);
	RBTree.Insert(50);
	RBTree.Insert(30);
	RBTree.Insert(80);
	RBTree.Insert(70);
	RBTree.Insert(20);
	RBTree.Insert(90);
	
	//RBTree.MakeDeapth(RBTree._pRoot);

	RBTree.PrintConsole();
	//RBTree.Print();
	printf("%d", RBTree.GetNodeCount());
	printf("\n\n");

	//RBTree.Delete(1);
	//RBTree.Delete(70);
	//RBTree.Delete(60);
	//RBTree.Delete(50);

	//RBTree.PrintConsole();
	//RBTree.Print();
	//printf("%d", RBTree.GetNodeCount());
	//printf("\n\n");

	/*RBTree._pRoot = RBTree.RightRotation(RBTree._pRoot);
	RBTree.PrintConsole();
	printf("\n\n");

	RBTree._pRoot = RBTree.LeftRotation(RBTree._pRoot);
	RBTree.PrintConsole();
	printf("\n\n");

	RBTree._pRoot = RBTree.LeftRotation(RBTree._pRoot);
	RBTree.PrintConsole();
	printf("\n\n");*/

	//RBTree._MaxDeapth = RBTree.MakeDeapth(RBTree._pRoot);
	//printf("%d", RBTree._MaxDeapth);
}

void GetKey() {

	if (GetKeyState(0x31) & 0x8000)
		g_Key = 1;
	else if (GetKeyState(0x32) & 0x8000)
		g_Key = 2;
	else if (GetKeyState(0x33) & 0x8000)
		g_Key = 3;
}

void TestAll() {

	timeBeginPeriod(1);

	system("mode con: cols=180 lines=60");

	for (;;) {

		system("cls");

		GetKey();

		switch (g_Key) {

		case 1:
		{	
			RBTree.Insert(g_Val++);
			g_Key = 0;
		}
		break;
		default:
			break;
		}

		Sleep(20);

		RBTree.PrintConsole();
	}

	timeEndPeriod(1);

}
// main.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <iostream>

int main()
{

	//for (;;) {
	//	Node = openList(F가장 작은거);
	//	if (Node 가 목적지 ? ) {
	//		true : Node->parent = 경로;
	//		false : 길 못찾고 종료;
	//	}

	//	closeList Insert(Node);
	//	// 팔방 노드 생성 Node 는 부모노드
	//	노드 생성(Node, NodeX + 1, NodeY + 1);
	//	노드 생성(Node, NodeX + 1, NodeY + 1);
	//	노드 생성(Node, NodeX + 1, NodeY + 1);
	//	노드 생성(Node, NodeX + 1, NodeY + 1); 
	//	노드 생성(Node, NodeX + 1, NodeY + 1);
	//	노드 생성(Node, NodeX + 1, NodeY + 1);
	//	노드 생성(Node, NodeX + 1, NodeY + 1);
	//	노드 생성(Node, NodeX + 1, NodeY + 1);
	//}
}

//노드 생성(parent, x, y) {
//
//	x, y 범위예외처리(필드 벗어나면) -> return;
//	if (openList x, y 검색) {
//
//		if (Node->G > 부모 Node->G + 1) {
//
//			Node->parent = 부모 노드;
//			Node->G = 재계산;
//			Node->F = 재계산;
//		}
//	}
//	else if (closeList x, y 검색) {
//
//		if (Node->G > 부모 Node->G + 1) {
//
//			Node->parent = 부모 노드;
//			Node->G = 재계산;
//			Node->F = 재계산;
//		}
//	}
//	// open, close에 없으니 정말
//	노드 생성;
//}

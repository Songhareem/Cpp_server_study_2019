// main.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <iostream>
#include "CSelectionSort.h"

int main()
{
	CSelectionSort SelectSort(5);

	SelectSort._pBuff[0] = 8;
	SelectSort._pBuff[1] = 5;
	SelectSort._pBuff[2] = 10;
	SelectSort._pBuff[3] = 4;
	SelectSort._pBuff[4] = 6;

	SelectSort.ShowConsole();
	printf("\n\nafter\n\n");
	SelectSort.Sort();
	SelectSort.ShowConsole();
}

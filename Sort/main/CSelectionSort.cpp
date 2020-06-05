
#include "pch.h"
#include "CSelectionSort.h"
#include <iostream>

#define SWAP(x,y,type) do{ \
	type temp; \
	temp = x; \
	x = y; \
	y = temp; \
} while (0)

CSelectionSort::CSelectionSort()
{
}

CSelectionSort::CSelectionSort(int szBuff)
{
	_pBuff = new int[szBuff];
	BuffSize = szBuff;
}

CSelectionSort::~CSelectionSort()
{
	delete[] _pBuff;
}

void CSelectionSort::Sort() {

	int MinIndex = NULL;
	
	for (int i = 0; i < BuffSize-1; i++) {

		MinIndex = i;

		for (int j = i+1; j < BuffSize; j++) {

			if (_pBuff[i] > _pBuff[j]) {
				
				if (_pBuff[MinIndex] > _pBuff[j])
					MinIndex = j;
			}
		}
		SWAP(_pBuff[i], _pBuff[MinIndex], int);
	}
}

void CSelectionSort::ShowConsole() {

	for (int i = 0; i < BuffSize; i++) {
		printf("_pBuff[%d] : %d\n", i, _pBuff[i]);
	}
}
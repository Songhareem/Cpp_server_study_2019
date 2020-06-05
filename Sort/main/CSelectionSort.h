#pragma once

#ifndef __SELECTION_SORT__
#define __SELECTION_SORT__

class CSelectionSort
{
public:
	CSelectionSort();
	CSelectionSort(int szBuff);
	~CSelectionSort();

	void Sort();
	void ShowConsole();

public:
	int *_pBuff;
	int BuffSize;
};

#endif
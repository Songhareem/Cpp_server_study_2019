#pragma once

#ifndef __MERGE_SORT__
#define __MERGE_SORT__

class CMergeSort
{
public:
	CMergeSort();
	CMergeSort(int szBuff);
	~CMergeSort();

	void Sort();

public:
	int *_pBuff;
	int _BuffSize;
	int _StartIndex;
	int _EndIndex;
};

#endif

// main

#include "CChunk.h"
#include <iostream>

using namespace std;

int main() {

	int NumOfChunk = 1;
	
	CChunk<int> *pChunk = new CChunk<int>();

	pChunk->ChunkInit(1, NULL);

	return 0;
}
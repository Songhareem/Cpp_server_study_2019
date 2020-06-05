
#include <iostream>
#include <Windows.h>

int main() {

	int arr[3] = { 0, 1, 2};

	int *p0 = &arr[0];
	int *p1 = &arr[1];
	int *p2 = &arr[2];

	BYTE crr[3] = { 1, 2, 3};

	BYTE *c0 = &crr[0];
	BYTE *c1 = &crr[1];
	BYTE *c2 = &crr[2];

	return 0;
}

// 과제1. 버블정렬

#include<stdio.h>

#define MAX 10

int main() {

	int a;
	int b;
	int c;
	int x;
	int y;
	int tmp;
	int check = 0; // 변화 없음

	int arr[10] = { 6,2,5,7,1,3,8,0,9,4 };

	for (a = MAX-1; a >=0; a--) {

		for (b = 0; b < MAX - (MAX-a); b++) {

			if (arr[b] > arr[b + 1]) {

				// core algorithm //
				tmp = arr[b + 1];
				arr[b + 1] = arr[b];
				arr[b] = tmp;
				////////////////////

				x = b;
				y = b + 1;

				check = 1; // 변화
			}

			if (check == 0) continue;

			for (c = 0; c < MAX; c++) {

				if (c == x || c == y) {
					printf("[%d] ", arr[c]);
				}
				else {
					printf(" %d  ", arr[c]);
				}
			}

			printf("\n");

			check = 0;
		}

		printf("\n");
	}
}
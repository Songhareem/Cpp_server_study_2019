/*

과제2. 랜덤 함수 구현 5번째 방법

//*/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<Windows.h>

#define MAX 20

typedef struct {

	char item[MAX];
	int seed;

}ITEM;

void main() {

	int total = 0;
	int random;
	int cnt = 1;

	ITEM item[4] = { {"common",50},
				     {"uncommon",35},
					 {"rare",10},
					 {"unique",5} };

	for (int i = 0; i < 4; i++) {

		total += item[i].seed;
	}

	while (total>0) {

		srand(time(NULL));

		random = rand() % total + 1;

		if (item[0].seed >= random && random > 0) {

			item[0].seed--;
			printf("%d : common\n",cnt);
			cnt++;
		}
		else if (item[1].seed + item[0].seed >= random && random > item[0].seed) {

			item[1].seed--;
			printf("%d : uncommon\n", cnt);
			cnt++;
		}
		else if (item[2].seed + item[1].seed + item[0].seed >= random
			&& random > item[1].seed + item[0].seed) {

			item[2].seed--;
			printf("%d : rare\n", cnt);
			cnt++;
		}
		else {

			item[3].seed--;
			printf("%d : unique\n", cnt);
			cnt++;
		}

		total--;

		Sleep(100);
	}


}

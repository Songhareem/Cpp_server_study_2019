
// 과제 1. sin 그래프 별로 찍어보기
 
#include<stdio.h>
#include<math.h>
#include<windows.h>

int main() {

	double radian;
	int star;
	int i = 0;
	
	while (1) {

		radian = i * 3.14 / 180;
		star = (int)(sin(radian) * 70);

		// 별찍기
		for (int j = 0; j < star; j++) {

			printf("*");
		}
		printf("\n");

		// 시간 조절
		Sleep(10);

		// radian 값 초기화
		if (++i > 180) i = 0;
	}

	return 0;
}
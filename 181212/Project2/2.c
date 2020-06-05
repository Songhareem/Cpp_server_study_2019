
/*
과제 2. unsigned char 변수의 값을 비트단위로 찍어주기

		- 지역변수에 특정 값을 하나 넣음
		- 비트 단위로 분해해서 0 이면 0 출력, 1 이면 1 출력
//*/

#include<stdio.h>

int main() {

	unsigned char input = 7; // 0000 0111 7 = test val
	unsigned char flag;		 // 1000 0000

	while(1) {
		printf("\n이진수로 바꾸고 싶은 값 : ");
		scanf("%d", &input);
		printf("이진수 값 : ");

		for (flag = 0x80; flag >= 0x01; flag = flag >> 1) {

			//printf("0x%02x ",flag);

			if (input&flag) {
				printf("1");
			}
			else {
				printf("0");
			}

		}

		printf("\n");
	}

	return 0;
}
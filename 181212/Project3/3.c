/*
과제 3. unsigned short (16bit) 변수의 각 비트를 컨트롤 하기

		- unsigned short 변수 = 0 으로 초기값 가짐.
		- 키보드로 1 ~ 16의 비트 자리 입력을 받음
		- 1 / 0  을 사용자로부터 받아서 지정된 자리의 비트를 0 또는 1로 바꿔줌.

		- 다른 위체이 입력된 기존 데이터는 보존이 되어야 함
//*/
#include<stdio.h>

int main() {

	unsigned short int tmp = 0x0000;
	unsigned short int set;
	short int bit;
	short int sw;
	
	while (1) {

		// 입력
		printf("바꾸길 원하는 비트자리수(총16bit) : ");
		scanf("%d",&bit);
		printf("OFF/ON[0/1] : ");
		scanf("%d",&sw);

		// tmp 값 세팅
		if (sw) {

			set = 0x0001;
			set = set << (bit - 1);

			tmp |= set;
		}
		else {

			set = 0x0001;
			set = set<<(bit-1);
			set = set^0xffff;

			tmp &= set;
		}

		//printf("0x%04x\n", tmp);

		// 16비트 상태 출력
	    set = 16;
		for (int i = 0x8000; i >= 0x0001; i = i >> 1) {

			printf("[%02d]번째 비트 : ",set--);
			if (tmp&i) {

				printf("ON\n");
			}
			else {

				printf("OFF\n");
			}
		}
	}

	return 0;
}
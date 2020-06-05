
/*
	과제4. unsigned int(32bit)변수를 바이트 단위로 사용하기

		- unsigned int 변수 초기값 0
		- 키보드로 1~4 바이트 위치 입력받고
		- 0~255를 입력받음
		- 사용자가 입력한 바이트 위치에 해당 값 넣음

		- 데이터 입력시 바이트 쪼개서 출력, 4바이트 16진수 출력
		- 기존 데이터 보존
		- 입력된 바이트는 기존값 지우고 넣음
//*/

#include<stdio.h>

int main() {

	unsigned int save=0;
	unsigned int tmp;
	int ctr;
	int input;

	while (1) {
		printf("입력할 자리수 : ");
		scanf("%d", &ctr);
		printf("입력할 수 : ");
		scanf("%d", &input);

		// 자리값 초기화
		tmp = 0x000000ff << (4 - ctr) * 8;
		tmp = tmp ^ 0xffffffff;

		save &= tmp;

		// 자리값 입력
		tmp = input;
		tmp = input << (4 - ctr) * 8;

		save |= tmp;

		for (ctr = 1; ctr <= 4; ctr++) {
			input = 0xff000000 >> (ctr - 1) * 8;
			tmp = (save & input) >> (4 - ctr) * 8;
			printf("%d ", tmp);
		}
		printf("\n");
		printf("16진수로 : 0x%08x\n\n", save);

	}

	return 0;
}

// ���� 1. sin �׷��� ���� ����
 
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

		// �����
		for (int j = 0; j < star; j++) {

			printf("*");
		}
		printf("\n");

		// �ð� ����
		Sleep(10);

		// radian �� �ʱ�ȭ
		if (++i > 180) i = 0;
	}

	return 0;
}
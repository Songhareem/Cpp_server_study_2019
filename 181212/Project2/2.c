
/*
���� 2. unsigned char ������ ���� ��Ʈ������ ����ֱ�

		- ���������� Ư�� ���� �ϳ� ����
		- ��Ʈ ������ �����ؼ� 0 �̸� 0 ���, 1 �̸� 1 ���
//*/

#include<stdio.h>

int main() {

	unsigned char input = 7; // 0000 0111 7 = test val
	unsigned char flag;		 // 1000 0000

	while(1) {
		printf("\n�������� �ٲٰ� ���� �� : ");
		scanf("%d", &input);
		printf("������ �� : ");

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
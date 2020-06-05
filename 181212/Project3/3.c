/*
���� 3. unsigned short (16bit) ������ �� ��Ʈ�� ��Ʈ�� �ϱ�

		- unsigned short ���� = 0 ���� �ʱⰪ ����.
		- Ű����� 1 ~ 16�� ��Ʈ �ڸ� �Է��� ����
		- 1 / 0  �� ����ڷκ��� �޾Ƽ� ������ �ڸ��� ��Ʈ�� 0 �Ǵ� 1�� �ٲ���.

		- �ٸ� ��ü�� �Էµ� ���� �����ʹ� ������ �Ǿ�� ��
//*/
#include<stdio.h>

int main() {

	unsigned short int tmp = 0x0000;
	unsigned short int set;
	short int bit;
	short int sw;
	
	while (1) {

		// �Է�
		printf("�ٲٱ� ���ϴ� ��Ʈ�ڸ���(��16bit) : ");
		scanf("%d",&bit);
		printf("OFF/ON[0/1] : ");
		scanf("%d",&sw);

		// tmp �� ����
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

		// 16��Ʈ ���� ���
	    set = 16;
		for (int i = 0x8000; i >= 0x0001; i = i >> 1) {

			printf("[%02d]��° ��Ʈ : ",set--);
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
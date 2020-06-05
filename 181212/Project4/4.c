
/*
	����4. unsigned int(32bit)������ ����Ʈ ������ ����ϱ�

		- unsigned int ���� �ʱⰪ 0
		- Ű����� 1~4 ����Ʈ ��ġ �Է¹ް�
		- 0~255�� �Է¹���
		- ����ڰ� �Է��� ����Ʈ ��ġ�� �ش� �� ����

		- ������ �Է½� ����Ʈ �ɰ��� ���, 4����Ʈ 16���� ���
		- ���� ������ ����
		- �Էµ� ����Ʈ�� ������ ����� ����
//*/

#include<stdio.h>

int main() {

	unsigned int save=0;
	unsigned int tmp;
	int ctr;
	int input;

	while (1) {
		printf("�Է��� �ڸ��� : ");
		scanf("%d", &ctr);
		printf("�Է��� �� : ");
		scanf("%d", &input);

		// �ڸ��� �ʱ�ȭ
		tmp = 0x000000ff << (4 - ctr) * 8;
		tmp = tmp ^ 0xffffffff;

		save &= tmp;

		// �ڸ��� �Է�
		tmp = input;
		tmp = input << (4 - ctr) * 8;

		save |= tmp;

		for (ctr = 1; ctr <= 4; ctr++) {
			input = 0xff000000 >> (ctr - 1) * 8;
			tmp = (save & input) >> (4 - ctr) * 8;
			printf("%d ", tmp);
		}
		printf("\n");
		printf("16������ : 0x%08x\n\n", save);

	}

	return 0;
}
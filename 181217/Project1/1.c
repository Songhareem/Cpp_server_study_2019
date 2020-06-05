
/* 

���� 1. ������ �˰��� �������� ����
		���� = ��Ʈ���� or �����迭[][2]
//*/

#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>

#define row 6
#define col 5
#define MAX 100

typedef struct {

	int x;
	int y;
	
}STACK;

void main() {

	int x;				// �ʱ� x ��ǥ
	int y;				// �ʱ� y ��ǥ
	int tmp_x;
	int tmp_y;
	int layer = 0;		// ���� ����
	int *pcnt = &layer;

	STACK stack[MAX];
	STACK *pst = &stack[0];

	char paint[row][col] =
	  { {'.','.','0','0','.'},
		{'.','.','0','.','.'},
		{'.','0','0','.','.'},
		{'.','.','0','0','.'},
		{'0','0','.','0','0'},
		{'0','.','.','0','.'} };

	char *ppaint = paint;
	
	// ù��° ��ǥ ����
	printf("%d\n", *pcnt);

	push(pst, 2, 2, pcnt); //0
	pst = &stack[*pcnt]; //1
	
	printf("%d\n", *pcnt);
	
	push(pst, 3, 3, pcnt); //1
	pst = &stack[*pcnt]; //2
	
	printf("%d\n", *pcnt);
	
	push(pst, 4, 4, pcnt); //2
	pst = &stack[*pcnt]; //3
	
	printf("%d\n", *pcnt);
	
	push(pst, 5, 5, pcnt); //3
	pst = &stack[*pcnt]; //4

	printf("%d\n", *pcnt);

	for (int i = 0; i < 4; i++) {

		printf("%d %d\n", stack[i].x, stack[i].y);
	}

	printf("%d\n", *pcnt);

	pop(pst, stack, pcnt);
	printf("%d %d\n", pst->x, pst->y);
	//pst = &stack[--(*pcnt)];
	//printf("%d %d\n", pst->x, pst->y);
	/*
	for(int i=0; i < 4; i++) {

		pst = &stack[*pcnt]; //4
		pop(pst,stack,pcnt); //3
		printf("%d %d\n",pst->x,pst->y);
	}
	*/
	/*
	while (1) {
		
		//��ǥ ����
		pst = &stack[layer];
		pop(pst, pcnt);
		tmp_x = pst->x;
		tmp_y = pst->y;
		//printf("%d %d", pst->x, pst->y);

		// �� ��ġ�� 0 �̶�� 1�� �����ϰ� ������
		if (paint[tmp_x][tmp_y] == '0') {

			paint[tmp_x][tmp_y] = '1';
			painting(ppaint);
		}

		// ������ ��ǥ �����¿� ������� Ȯ�� �� ���ÿ� �ֱ�
		if (tmp_x > 0 && paint[tmp_x - 1][tmp_y] == '0') {

			push(pst, tmp_x-1, tmp_y, pcnt);
			pst = &stack[layer];
		}
		if (tmp_x < row-1 && paint[tmp_x+1][tmp_y] == '0') {

			push(pst,tmp_x+1,tmp_y,pcnt);
			pst = &stack[layer];
		}
		if (tmp_y > 0 && paint[tmp_x][tmp_y-1] == '0') {

			push(pst,tmp_x,tmp_y-1,pcnt);
			pst = &stack[layer];
		}
		if (tmp_y < col - 1 && paint[tmp_x][tmp_y+1] == '0') {

			push(pst,tmp_x,tmp_y+1,pcnt);
			pst = &stack[layer];
		}

		// ������ ����ٸ� ����
		if (layer == 0) break;
	}

	printf("The End\n");
	//*/
}

int push(STACK *pst, int x, int y,int *pcnt) {

	if (*pcnt >= 100) {

		printf("full\n");
		return 0;
	}

	pst->x = x;
	pst->y = y;
	
	*pcnt += 1;

	return 1;
}
//4
int pop(STACK *pst, STACK *stack,int *pcnt) {

	if (*pcnt <= 0) {

		printf("empty\n");
		return 0;
	}

	printf("%d\n", *pcnt);
	*pcnt = *pcnt - 1; // 3
	printf("%d\n", *pcnt);
	pst = &stack[*pcnt];

	return 1;
}

int painting(char *ppaint) {

	system("cls");

	for (int i = 0; i < row; i++) {

		for (int j = 0; j < col; j++) {

			printf("%c ", *ppaint);
			ppaint++;
		}
		printf("\n");
	}

	Sleep(500);

	return;
}
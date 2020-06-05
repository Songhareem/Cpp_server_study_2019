
// ����1. ���� �����̱�

#include<stdio.h>
#include<Windows.h>
#include<time.h>

#define WIDTH 64
#define HEIGHT 20
void SetStartPoint(char back[][WIDTH], int y, int x);	// ������ġ ���� �Լ�
void SetBackByKey(char back[][WIDTH], int *py, int *px); // Ű�Է¿� ���� ����� ����
void Ready(char back[][WIDTH], char rander[][WIDTH]);	// ����� -> ��������
void Ready_2(char back[][WIDTH], char rander[][WIDTH]); // ����� -> ��������2  
void Display(char p[][WIDTH]);							// ȭ������Լ�
void Display_2(char *p);								// ȭ������Լ�2

int main() {

	char *p;
	
	unsigned int x=WIDTH/2;
	unsigned int y=HEIGHT/2;

	int *px;
	int *py;

	px = &x;
	py = &y;

	// Ŀ�� ��ǥ ����ü
	COORD pos = { 0, 0 }; // y, x

	// Ŀ�� ���� ����ü (�β�, ���ü�)
	CONSOLE_CURSOR_INFO cursorInfo = { 0, };
	cursorInfo.dwSize = 1;	// Ŀ�� �β� ����
	cursorInfo.bVisible = FALSE; // ���ü� TRUE : ���� FALSE : �Ұ���
	
	//���� �ΰ�, �������� �����, �ʱ�ȭ
	char rander[HEIGHT][WIDTH];
	char back[HEIGHT][WIDTH];
	
	memset(rander, ' ', sizeof(rander));
	memset(back, ' ', sizeof(back));

	//Display(rander);
	//printf("\n\n");
	//p = &rander[0][0];
	//Display_2(p);

	// 'A' ���� ��ġ ���� �Լ�
	SetStartPoint(back, y, x);
	
	// Ŀ�� �Ұ���
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

	//Ready_2(back, rander);
	//Display(rander);

	// �ݺ���
	while (1) {
		
		// Ű���Է¿� ���� ����� �����Լ�
		SetBackByKey(back, py, px);
		
		// ����� -> �������� 1,2 �� ���� �� ������ üũ�غ���
		Ready(back, rander);

		// ȭ������Լ�
		Display(rander);

		// Ŀ�� ��ǥ �̵�
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
		
		// Ű�Է� �ð�����
		Sleep(100);
	}

	return 0;
}

// ������ġ ����
void SetStartPoint(char back[][WIDTH],int y, int x) {

	back[y][x] = 'A';
}

// Ű���Է¿� ���� ����� �����Լ�
void SetBackByKey(char back[][WIDTH],int *py, int *px) {

	int origin_x = *px; // 32
	int origin_y = *py; // 10
	int change_x = *px; // 32
	int change_y = *py; // 10

	printf("%02x\n", change_x);
	printf("%02x\n", change_y);

	if (GetKeyState(VK_UP) & 0x8000 && change_y > 0) {

		change_y--;
	}
	if (GetKeyState(VK_DOWN) & 0x8000 && change_y < HEIGHT-1) {

		change_y++;
	}
	if (GetKeyState(VK_LEFT) & 0x8000 && change_x > 0) {

		change_x--;
	}
	if (GetKeyState(VK_RIGHT)& 0x8000 && change_x < WIDTH-1) {

		change_x++;
	}

	printf("%02x\n", change_x);
	printf("%02x\n", change_y);

	*px = change_x;
	*py = change_y;

	back[origin_y][origin_x] = ' ';
	back[change_y][change_x] = 'A';
}

// ����� -> ��������, display
void Ready(char back[][WIDTH], char rander[][WIDTH]) {

	//clock_t start = clock();

	for (int i = 0; i < HEIGHT; i++) {

		for (int j = 0; j < WIDTH; j++) {

			if(rander[i][j] != back[i][j])
				rander[i][j] = back[i][j];
		}
	}

	//printf("%d\n", (clock()-start));
}

void Ready_2(char back[][WIDTH], char rander[][WIDTH]) {
	
	//printf("%f\n", clock());

	memcpy(rander, back, sizeof(back));

	//printf("%f\n", clock());
}

// ȭ������Լ�
void Display(char p[][WIDTH]) {

	for (int i = 0; i < HEIGHT; i++) {

		for (int j = 0; j < WIDTH; j++) {

			printf("%c", p[i][j]);
		}

		printf("\n");
	}
}

void Display_2(char *p) {

	for (int i = 0; i < HEIGHT; i++) {

		for (int j = 0; j < WIDTH; j++) {

			printf("%c", *p);
			p++;
		}

		printf("\n");
	}
}
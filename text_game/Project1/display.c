
#include"setting.h"

void cs_Initial(void)
{
	CONSOLE_CURSOR_INFO stConsoleCursor;

	//-------------------------------------------------------------
	// ȭ���� Ŀ���� �Ⱥ��̰Բ� �����Ѵ�.
	//-------------------------------------------------------------
	stConsoleCursor.bVisible = FALSE;
	stConsoleCursor.dwSize = 1;			// Ŀ�� ũ��.
											// �̻��ϰԵ� 0 �̸� ���´�. 1���ϸ� �ȳ��´�.

	//-------------------------------------------------------------
	// �ܼ�ȭ�� (���ٴٵ� �ƿ�ǲ) �ڵ��� ���Ѵ�.
	//-------------------------------------------------------------
	
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &stConsoleCursor);
}

// Ŀ�� �̵�
void cs_MoveCursor(int iPosX, int iPosY)
{
	COORD stCoord;
	stCoord.X = iPosX;
	stCoord.Y = iPosY;
	//-------------------------------------------------------------
	// ���ϴ� ��ġ�� Ŀ���� �̵���Ų��.
	//-------------------------------------------------------------
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), stCoord);
}

// ���� �ʱ�ȭ
void initBuf(char buf[][WIDTH]) {

	// memset�� ������� �ʰ�, for������ �ʱ�ȭ
	// ���� �������� NULL���� �ֱ� ����
	for (int i = 1; i < HEIGHT-1; i++) {

		for (int j = 1; j < WIDTH - 2; j++) {

			// ' ' �� ȭ�� �ʱ�ȭ
			buf[i][j] = ' ';
		}
		buf[i][WIDTH - 1] = NULL;
	}

	// �� ���� ���� ����
	for (int i = 1; i < HEIGHT; i++) buf[i][0] = buf[i][WIDTH - 2] = '!';

	for (int i = 0; i < WIDTH-1; i++) buf[0][i] = buf[HEIGHT-1][i] = '=';
}

// ���� �ø�
void flip(char rander[][WIDTH], char back[][WIDTH]) {

	for(int i=0; i<HEIGHT; i++) {
		
		// NULL ����
		for (int j = 0; j < WIDTH-1; j++) {

			if (rander[i][j] != back[i][j]) rander[i][j] = back[i][j];
		}
	}
}

// ȭ�� ���
void display(char rander[][WIDTH]) {

	for (int i = 0; i < HEIGHT; i++) {

		cs_MoveCursor(0, i);
		printf("%s", rander[i]);
	}
}
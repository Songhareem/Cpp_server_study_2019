
#include"setting.h"

void cs_Initial(void)
{
	CONSOLE_CURSOR_INFO stConsoleCursor;

	//-------------------------------------------------------------
	// 화면의 커서를 안보이게끔 설정한다.
	//-------------------------------------------------------------
	stConsoleCursor.bVisible = FALSE;
	stConsoleCursor.dwSize = 1;			// 커서 크기.
											// 이상하게도 0 이면 나온다. 1로하면 안나온다.

	//-------------------------------------------------------------
	// 콘솔화면 (스텐다드 아웃풋) 핸들을 구한다.
	//-------------------------------------------------------------
	
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &stConsoleCursor);
}

// 커서 이동
void cs_MoveCursor(int iPosX, int iPosY)
{
	COORD stCoord;
	stCoord.X = iPosX;
	stCoord.Y = iPosY;
	//-------------------------------------------------------------
	// 원하는 위치로 커서를 이동시킨다.
	//-------------------------------------------------------------
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), stCoord);
}

// 버퍼 초기화
void initBuf(char buf[][WIDTH]) {

	// memset을 사용하지 않고, for문으로 초기화
	// 행의 마지막에 NULL문자 넣기 위해
	for (int i = 1; i < HEIGHT-1; i++) {

		for (int j = 1; j < WIDTH - 2; j++) {

			// ' ' 로 화면 초기화
			buf[i][j] = ' ';
		}
		buf[i][WIDTH - 1] = NULL;
	}

	// 맨 앞줄 뒷줄 막기
	for (int i = 1; i < HEIGHT; i++) buf[i][0] = buf[i][WIDTH - 2] = '!';

	for (int i = 0; i < WIDTH-1; i++) buf[0][i] = buf[HEIGHT-1][i] = '=';
}

// 버퍼 플립
void flip(char rander[][WIDTH], char back[][WIDTH]) {

	for(int i=0; i<HEIGHT; i++) {
		
		// NULL 제외
		for (int j = 0; j < WIDTH-1; j++) {

			if (rander[i][j] != back[i][j]) rander[i][j] = back[i][j];
		}
	}
}

// 화면 출력
void display(char rander[][WIDTH]) {

	for (int i = 0; i < HEIGHT; i++) {

		cs_MoveCursor(0, i);
		printf("%s", rander[i]);
	}
}
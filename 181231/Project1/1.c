
// 과제1. 글자 움직이기

#include<stdio.h>
#include<Windows.h>
#include<time.h>

#define WIDTH 64
#define HEIGHT 20
void SetStartPoint(char back[][WIDTH], int y, int x);	// 시작위치 지정 함수
void SetBackByKey(char back[][WIDTH], int *py, int *px); // 키입력에 따른 백버퍼 변경
void Ready(char back[][WIDTH], char rander[][WIDTH]);	// 백버퍼 -> 랜더버퍼
void Ready_2(char back[][WIDTH], char rander[][WIDTH]); // 백버퍼 -> 랜더버퍼2  
void Display(char p[][WIDTH]);							// 화면출력함수
void Display_2(char *p);								// 화면출력함수2

int main() {

	char *p;
	
	unsigned int x=WIDTH/2;
	unsigned int y=HEIGHT/2;

	int *px;
	int *py;

	px = &x;
	py = &y;

	// 커서 좌표 구조체
	COORD pos = { 0, 0 }; // y, x

	// 커서 설정 구조체 (두께, 가시성)
	CONSOLE_CURSOR_INFO cursorInfo = { 0, };
	cursorInfo.dwSize = 1;	// 커서 두께 설정
	cursorInfo.bVisible = FALSE; // 가시성 TRUE : 가시 FALSE : 불가시
	
	//버퍼 두개, 랜더버퍼 백버퍼, 초기화
	char rander[HEIGHT][WIDTH];
	char back[HEIGHT][WIDTH];
	
	memset(rander, ' ', sizeof(rander));
	memset(back, ' ', sizeof(back));

	//Display(rander);
	//printf("\n\n");
	//p = &rander[0][0];
	//Display_2(p);

	// 'A' 시작 위치 지정 함수
	SetStartPoint(back, y, x);
	
	// 커서 불가시
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

	//Ready_2(back, rander);
	//Display(rander);

	// 반복문
	while (1) {
		
		// 키값입력에 따른 백버퍼 변경함수
		SetBackByKey(back, py, px);
		
		// 백버퍼 -> 랜더버퍼 1,2 중 뭐가 더 빠를지 체크해보기
		Ready(back, rander);

		// 화면출력함수
		Display(rander);

		// 커서 좌표 이동
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
		
		// 키입력 시간조절
		Sleep(100);
	}

	return 0;
}

// 시작위치 지정
void SetStartPoint(char back[][WIDTH],int y, int x) {

	back[y][x] = 'A';
}

// 키값입력에 따른 백버퍼 변경함수
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

// 백버퍼 -> 랜더버퍼, display
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

// 화면출력함수
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
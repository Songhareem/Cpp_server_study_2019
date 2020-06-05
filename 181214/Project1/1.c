
/*

빈칸 찾아서 페인트 뿌리기 (길찾기 알고리즘 비슷)

//*/

#include<stdio.h>
#include<windows.h>

#define row 6
#define col 5

void output(char *paint[5]);
void painting(int x, int y, char *paint[5]);

int main() {

	char paint[row][col] = 
	{   {'.','.','0','0','.'},
		{'.','.','0','.','.'},
		{'.','0','0','.','.'},
		{'.','.','0','0','.'},
		{'0','0','.','0','0'},
		{'0','.','.','0','.'} };
			
	while (1) {

		painting(0, 2, paint);

		break;
	}

	return 0;
}

void output(char (*paint)[5]) {

	int i;
	int j;

	for (i = 0; i < 6; i++) {

		for (j = 0; j < 5; j++) {

			printf("%c ", paint[i][j]);
		}

		printf("\n");
	}
}

void painting(int x, int y, char (*paint)[5]) {

	if (x < 0 || y < 0 || x >= row || y >= col) return;
	if (paint[x][y] == '.' || paint[x][y] == '1') return;

	paint[x][y] = '1';

	output(paint);

	Sleep(1000);

	system("cls");

	painting(x-1,y,paint);
	painting(x+1, y, paint);
	painting (x, y-1, paint);
	painting(x, y+1, paint);

}	
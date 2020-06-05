
#include<stdio.h>
#include<windows.h>					// timeBeginPeriod(1), timeEndPeriod(1), system("cls");
#include<time.h>					// clock_t, CLOCKS_PER_SEC
#include<conio.h>					// _kbhit(), _getch()
#pragma comment(lib, "Winmm.lib")	// timeBeginPeriod(1), timeEndPeriod(1)

int g_time[6] = { 5, 10, 15, 20, 25 ,30 };
int g_check[6] = { 0,0,0,0,0,0 };

void main() {

	timeBeginPeriod(1);
	clock_t time;
	int index=0;

	while (1) {

		system("cls");
		time = clock();

		// 출력부
		printf("%03d.%03d  \n", time / CLOCKS_PER_SEC, time % CLOCKS_PER_SEC); // 맨 첫줄

		for (int i = 0; i < 6; i++) {
			printf("%d Sec : ", g_time[i]); // 

			switch (g_check[i]) {

			case 0: printf("\n");
				break;

			case 1: printf("Fail\n");
				break;

			case 2: printf("Bad\n");
				break;

			case 3: printf("Good\n");
				break;

			case 4: printf("Great\n");
				break;

			default: break;
			}
		}
		
		// 로직부
		// 2초 지나면 fail로 변형if
		if ((time / CLOCKS_PER_SEC) == (g_time[index] + 2) && g_check[index] == 0) {

			g_check[index] = 1;
			index++;
		}
		
		// 키 입력에 따른 시간 정확도 입력
		while (_kbhit()) {

			// 버퍼비우기
			_getch();
			_getch();
			//fflush(stdin) // 2015이전 버전 가능
			//printf("kbhit : %d\n", _kbhit());

			//test
			//printf("%d\n", time);
			//printf("%d\n", g_time[index] * CLOCKS_PER_SEC);
			
			int now = g_time[index] * CLOCKS_PER_SEC;

			if (time > now - 200 && time < now + 200) {
				//Great
				g_check[index] = 4;
				index++;
			}
			else if (time > now - 500 && time < now + 500) {
				//good
				g_check[index] = 3;
				index++;
			}
			else if (time > now - 1200 && time < now + 1200) {
				//Bad
				g_check[index] = 2;
				index++;
			}
			else  {
				// Fail
				g_check[index] = 1;
				index++;
			}
			
		}

		if ((time / CLOCKS_PER_SEC) >= g_time[5] + 3) break;
	}

	timeBeginPeriod(1);

	return;
}
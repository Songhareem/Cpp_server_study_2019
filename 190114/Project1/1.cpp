
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include "stack.h"
#include "class.h"

#define KEY_1 0x31
#define KEY_2 0x32
#define KEY_3 0x33

using namespace std;

// test
//BaseObject *p;

BaseObject *g_base[MAX];

bool getKey();
void Run();
void Render();

int main() {

	// g_base init
	for (int i = 0; i < MAX; i++) g_base[i] = NULL;

	// consol
	system("mode con: cols=77 lines=21");

	while (1) {

		// 키입력
		getKey();

		//Run
		Run();

		//Render
		system("cls");
		Render();

		// Frame control
		Sleep(10);
	}
	
	return 0;
}

// key 입력 및 객체 생성 ( 0 : 미입력  / 1 : 입력 )
bool getKey() {

	int key;
	int blank;

	if (_kbhit() == 0) return false;

	// get key value 
	key = _getch();

	// view Key value
	// printf("%x\n", key);

	// get blank line
	for (blank = 0; blank < MAX; blank++) {

		if (g_base[blank] == NULL) break;
	}

	// 사실은 메세지 형식이 더 좋을듯
	switch (key) {

	case KEY_1:
		g_base[blank] = new OneStar;
		break;
	case KEY_2:
		g_base[blank] = new TwoStar;
		break;
	case KEY_3:
		g_base[blank] = new ThreeStar;
		break;
	}

	return true;
}

void Run() {

	int destroyTiming;

	for (int i = 0; i < MAX; i++) {

		if (g_base[i] == NULL) continue;

		destroyTiming = g_base[i]->Run();

		if (destroyTiming) {

			delete[] g_base[i];
			g_base[i] = NULL;
		}
	}
}

void Render() {

	for (int i = 0; i < MAX; i++) {

		if (g_base[i] == NULL) {

			cout << endl;
			continue;
		}

		g_base[i]->Render();

	}
}

/* test block
{
	bool destroyTiming;

	p = new OneStar;

	while(1) {

		destroyTiming = p->Run();

		system("cls");
		p->Render(destroyTiming);

		Sleep(50);
	}

	//delete[] p; // 이게 왜 차이가 나더라?
}
//*/
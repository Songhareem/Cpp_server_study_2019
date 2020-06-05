
#include"stack.h"

int g_stack[MAX];
int g_top = -1;

// 0 : 비정상 종료, 1 : 정상 종료
bool push(const int val) {

	if (g_top + 1 >= MAX) return false;

	g_stack[++g_top] = val;

	return true;
}

bool pop(int &get_val) {

	if (g_top <= -1) return false;

	get_val = g_stack[g_top--];

	return true;
}

/*  test stack
	int err;
	int get_val = 0;

	push(1);

	err = pop(get_val);
	cout << err << endl;
	cout << get_val << endl;

	push(1);
	push(2);
	push(3);
	push(4);
	push(5);

	for (int i = 0; i < MAX; i++) {

		cout << g_stack[i] << endl;
	}

	cout << endl;

	err = pop(get_val);
	cout << get_val << endl;
	*/

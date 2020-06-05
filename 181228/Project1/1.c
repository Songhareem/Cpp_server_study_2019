
// ť�� �̿��� ��Ƽ �׽�ŷ ����
// GetTickCount() ����� ��, DWORD return��
// DWORD Q[5]
// �Է½�, �ð���(���� ���� Ȥ�� ���� �ð�) -> ����ð� - ���۽ð� = �� ������ �� ���� -> ������ ����
// ��½�, %�� ǥ��

//Bool Enqueue()
//Bool Dequeue()
//Bool Peek(&Data, Pos) (�����ʰ� �����͸� ���°�)
// Pos 0 -> ���� ��ġ ���� Dequeue, 1 -> ���� ��ġ + 1 ���� Dequeue ...
// fflush ��� SHORT GetAsyncKeyState (Async = �񵿱�)
// GetAsyncKeyState�� 0x8000 : ȣ���ϴ� ���� ���� �� / 0x0001 : ������ ���� ���� ������ �Լ��� ȣ��Ǵ� ���� ���̿� �������� �ִٸ� ����
// �� ��� �� ������� ��, if( GetAsyncKeyState(VK_asd) ) �� ���
// if left & 0x8000 else if right & 8000 ���� ����ϰų� if left if right �� ����� ��
// _kbhit()�� ���� �Լ� GetAsyncKeyState�� ��� �� ��

// ����, ����(���) ��Ʈ ���� �� ��
// ť �̻� ���� �� ����������
// �ϼ��Ǹ� ť���� �����鼭 �ϼ� ���

#include<stdio.h>
#include<Windows.h>

#pragma comment (lib,"Winmm.lib")

DWORD queue[5];
int front = 1;
int rear = 1;
int len = 0;

int Enqueue();
int Dequeue(DWORD *p);
int Peek(DWORD *p, int pos);

int main() {

	DWORD result;
	DWORD *pr = &result;
	/* test Enqueue, Dequeue, Peek
	if (Enqueue() == 0) printf("full\n");
	if (Enqueue() == 0) printf("full\n");
	if (Enqueue() == 0) printf("full\n");
	if (Enqueue() == 0) printf("full\n");
	if (Dequeue(pr) == 0) printf("empty\n");
	if (Dequeue(pr) == 0) printf("empty\n");

	for (int i = 0; i < len; i++) {

		Peek(pr, i);
		printf("%d\n", *pr);
	}
	//*/
	
	while (1) {

		// ��º�
		printf("1.���� �ֱ�  2.���� \n\n");
		printf("==============================================================================\n|");
		for (int i = 0; i < len; i++) {


		}
		printf("==============================================================================\n");


		// ������
		while (!GetAsyncKeyState(0x31)) {
			
			rewind(stdin);

			printf("%d\n", GetAsyncKeyState(0x31));

			Sleep(1000);
		}
	}

	return 0;
}
// 0 ���� = full, 1 ���� = ��ť ����
int Enqueue() {

	//printf("front : %d rear : %d\n", front, rear);

	int check = rear;

	if (++check >= 5) check = 0;

	// ť�� ����á���� �ƴ��� Ȯ��
	if (front == check) return 0;
	
	//printf("save\n");
	
	// ������ ����
	queue[rear] = GetTickCount();
	rear++;
	if (rear >= 5) rear = 0;

	// �ڷ᰹��++
	len++;

	return 1;
}

// 0 ���� = empty, 1 ���� = ��ť ����
int Dequeue(DWORD *p) {

	// ť�� ������� Ȯ��
	if (front == rear) return 0;

	*p = queue[front];

	//for test
	queue[front] = 0;
	front++;
	if (front >= 5) front = 0;

	// �ڷ᰹��--
	len--;

	return 1;
}

// 0 = ���� ����, �� ���� -> �ּҰ� ���� �б�
int Peek(DWORD *p, int pos) {

	int pick = front;
	*p = queue[pick + pos];
}

// change per
int MakePer(DWORD *p) {

	DWORD now = GetTickCount() - *p;
	DWORD et = 5000;

	long per = *p / et * 100;
}

/*
	timeBeginPeriod(1);

	DWORD time = GetTickCount();

	printf("%d\n", time);

	Sleep(1000);

	while (1) {
		if (GetAsyncKeyState(0x30) & 0x8000) {

			printf("0 Key\n");
			break;
		}
	}

	time = GetTickCount();
	printf("%d", time);

	timeEndPeriod(1);
	*/
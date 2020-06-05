
// 큐를 이용한 멀티 테스킹 구현
// GetTickCount() 사용할 것, DWORD return형
// DWORD Q[5]
// 입력시, 시간값(생성 시작 혹은 종료 시간) -> 현재시간 - 시작시간 = 새 게이지 찬 정도 -> 오차가 없음
// 출력시, %로 표현

//Bool Enqueue()
//Bool Dequeue()
//Bool Peek(&Data, Pos) (뽑지않고 데이터만 보는것)
// Pos 0 -> 현재 위치 값만 Dequeue, 1 -> 현재 위치 + 1 값만 Dequeue ...
// fflush 대신 SHORT GetAsyncKeyState (Async = 비동기)
// GetAsyncKeyState는 0x8000 : 호출하는 순간 눌린 값 / 0x0001 : 이전에 눌린 때와 다음에 함수가 호출되는 순간 사이에 눌린적이 있다면 리턴
// 두 경우 다 상관없을 시, if( GetAsyncKeyState(VK_asd) ) 로 사용
// if left & 0x8000 else if right & 8000 으로 사용하거나 if left if right 로 사용할 것
// _kbhit()는 느린 함수 GetAsyncKeyState로 대신 할 것

// 로직, 랜더(출력) 파트 따로 할 것
// 큐 이상 들어가면 더 넣을수없음
// 완성되면 큐에서 빠지면서 완성 출력

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

		// 출력부
		printf("1.유닛 넣기  2.종료 \n\n");
		printf("==============================================================================\n|");
		for (int i = 0; i < len; i++) {


		}
		printf("==============================================================================\n");


		// 로직부
		while (!GetAsyncKeyState(0x31)) {
			
			rewind(stdin);

			printf("%d\n", GetAsyncKeyState(0x31));

			Sleep(1000);
		}
	}

	return 0;
}
// 0 리턴 = full, 1 리턴 = 인큐 성공
int Enqueue() {

	//printf("front : %d rear : %d\n", front, rear);

	int check = rear;

	if (++check >= 5) check = 0;

	// 큐가 가득찼는지 아닌지 확인
	if (front == check) return 0;
	
	//printf("save\n");
	
	// 데이터 삽입
	queue[rear] = GetTickCount();
	rear++;
	if (rear >= 5) rear = 0;

	// 자료갯수++
	len++;

	return 1;
}

// 0 리턴 = empty, 1 리턴 = 디큐 성공
int Dequeue(DWORD *p) {

	// 큐가 비었는지 확인
	if (front == rear) return 0;

	*p = queue[front];

	//for test
	queue[front] = 0;
	front++;
	if (front >= 5) front = 0;

	// 자료갯수--
	len--;

	return 1;
}

// 0 = 현재 기준, 값 증가 -> 주소값 증가 읽기
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
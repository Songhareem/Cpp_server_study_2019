
#include"setting.h"

// 메세지 큐에 쓰기
bool enqueue(byte cmd) {

	//printf("cmd : %d\n", cmd);

	// 큐가 가득 찼는지 체크
	byte check = g_mRear;

	if (++check >= QUEUE_SIZE ) check = 0;
	if (check == g_mFront) return FALSE;

	// 데이터 쓰기
	g_message[g_mRear] = cmd;
	g_mRear++;
	if (g_mRear >= QUEUE_SIZE) g_mRear = 0;

	// 메시지 큐++ 갱신
	g_mLen++;

	return TRUE;
}

// 큐에서 읽기
bool dequeue(byte *reader) {

	// 큐가 비었는지 체크
	if (g_mFront == g_mRear) return FALSE;

	// 데이터 읽기
	*reader = g_message[g_mFront];
	g_mFront++;
	if (g_mFront >= QUEUE_SIZE) g_mFront = 0;

	// 메시지 큐-- 갱신
	g_mLen--;

	return TRUE;
}

// 키값입력에 따른 메시지 큐 쓰기
void inputKey() {

	if (GetKeyState(VK_UP) & 0x8000) {

		enqueue(UP);
	}
	if (GetKeyState(VK_DOWN) & 0x8000) {

		enqueue(DOWN);
	}
	if (GetKeyState(VK_LEFT) & 0x8000) {

		enqueue(LEFT);
	}
	if (GetKeyState(VK_RIGHT) & 0x8000) {

		enqueue(RIGHT);
	}

	// Z 버튼
	if (GetKeyState(0x5A) & 0x8000) {

		enqueue(Z);
	}
}

#include"setting.h"

// �޼��� ť�� ����
bool enqueue(byte cmd) {

	//printf("cmd : %d\n", cmd);

	// ť�� ���� á���� üũ
	byte check = g_mRear;

	if (++check >= QUEUE_SIZE ) check = 0;
	if (check == g_mFront) return FALSE;

	// ������ ����
	g_message[g_mRear] = cmd;
	g_mRear++;
	if (g_mRear >= QUEUE_SIZE) g_mRear = 0;

	// �޽��� ť++ ����
	g_mLen++;

	return TRUE;
}

// ť���� �б�
bool dequeue(byte *reader) {

	// ť�� ������� üũ
	if (g_mFront == g_mRear) return FALSE;

	// ������ �б�
	*reader = g_message[g_mFront];
	g_mFront++;
	if (g_mFront >= QUEUE_SIZE) g_mFront = 0;

	// �޽��� ť-- ����
	g_mLen--;

	return TRUE;
}

// Ű���Է¿� ���� �޽��� ť ����
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

	// Z ��ư
	if (GetKeyState(0x5A) & 0x8000) {

		enqueue(Z);
	}
}

#include<iostream>

class CPlayer {


};

CPlayer *pPlayer; // �迭�� ��ü ����
// char buff[1000];

int main() {

	pPlayer = (CPlayer *)malloc(sizeof(CPlayer));
	//pPlayer = (T *)malloc(sizeof(T));

	// player ���Ӱ� ���
	new(pPlayer) CPlayer;
	//new(pPlayer) T;

	// player �ı�
	pPlayer->~CPlayer();
	//pPlayer->~T();

	// player ���Ӱ� ���
	new (pPlayer) CPlayer; // placement new

	// player �ı�
	pPlayer->~CPlayer();

	free(pPlayer);

	return 0;
}
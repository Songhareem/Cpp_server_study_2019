
#include<iostream>

class CPlayer {


};

CPlayer *pPlayer; // 배열로 대체 가능
// char buff[1000];

int main() {

	pPlayer = (CPlayer *)malloc(sizeof(CPlayer));
	//pPlayer = (T *)malloc(sizeof(T));

	// player 새롭게 사용
	new(pPlayer) CPlayer;
	//new(pPlayer) T;

	// player 파괴
	pPlayer->~CPlayer();
	//pPlayer->~T();

	// player 새롭게 사용
	new (pPlayer) CPlayer; // placement new

	// player 파괴
	pPlayer->~CPlayer();

	free(pPlayer);

	return 0;
}
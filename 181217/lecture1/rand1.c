
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

// 기초적인 랜덤 방법 (컨셉만)
void real_random();

void main() {

	rand_1();
}

// 일반 랜덤 가챠
void real_random() {

	int get;

	// { 아이템번호, 아이템 활률 }
	int gatcha[][2] = { {1,10},
						{2,20},
						{3,50},
						{4,5},
						{5,100} };

	// 75가 리턴이라면 3번 아이템이 나와야함 ( 31 ~ 80 사이 확률 ) 
	get = rand() % 185 + 1;
}

// 중복값 있는 횟수 강제 랜덤 가챠
void fake_random() {

	// 가챠 횟수
	static int cnt=0;

	// {아이템번호, 아이템확률, 아이템이 반드시 뽑히는 횟수 제한 }
	int gatcha[][3] = { {1,10,0},
					{2,20,0},
					{3,50,0},
					{4,5,51},
					{5,100,10} };

}

// 중복값 없는 랜덤가챠 (속도 느림)
void fake_random2() {

	// 한번 뽑힌 값을 넣어줌
	int randTable[1000];
}

// 미리 테이블짜고 셔플후 하나씩 제공

// 원본테이블 지정(이차배열), 뽑히면 확률 차감
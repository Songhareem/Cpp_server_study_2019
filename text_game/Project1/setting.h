
#include<stdio.h>
#include<Windows.h>
#include<stdbool.h>
#include<time.h>
#include<stdlib.h>

/*/////////////////////////
		define 모음
*//////////////////////////

// 게임 화면 size
#define WIDTH 127
#define HEIGHT 32

// 게임화면 test size
//#define WIDTH 11
//#define HEIGHT 5

// 메시지 큐 size = 10, 1은 링버퍼를 위한 빈공간
#define QUEUE_SIZE 11

// 메시지 큐 test size
//#define QUEUE_SIZE 5

// 메시지 큐 입력 값 cmd 정의
#define UP    1
#define DOWN  2
#define LEFT  3
#define RIGHT 4
#define Z	  5

// 총알 및 적 배열 크기
#define MAX_ENEMY 20
#define MAX_BULLET 500

// 시퀀스 플래그
#define START 0
#define GAME 1
#define OVER 2
#define END  3

/*///////////////////////////
      구조체 정의부분
*////////////////////////////

typedef struct {

	bool flag; // 총알 피아구분 (TRUE : 아군, FALSE : 적군)
	bool visual;

	short x; // 총알 X
	short y; // 총알 Y

}BULLET;

typedef struct {

	bool visual; // 보이기 안보이기

	short hp;	 // hp

	short x;	 // X
	short y;	 // Y

}PLAIN;

typedef struct {

	bool visual; // 보이기 안보이기

	short hp;
	
	short x;
	short y;

	short vector;
	short moveCnt;

}ENEMY;


/*//////////////////////////////
		exturn 선언부분
*///////////////////////////////

// 게임 시퀀스 플래그
extern byte g_sequence;

// 콘솔 초기화
extern bool g_initConsol;

// 게임 진행 시간
extern clock_t startTime;
extern clock_t gameTime;
extern clock_t overTime;

// 게임 난이도
extern byte g_level;

// 게임 스코어
extern int g_score;

// 컨트롤 메시지 큐 관련 글로벌 선언
extern byte g_message[QUEUE_SIZE];
extern byte g_mFront;
extern byte g_mRear;
extern byte g_mLen;

// 백버퍼
extern char back[HEIGHT][WIDTH];

// 플레이어 및 적
extern PLAIN player;
extern ENEMY enemy[MAX_ENEMY];

// 아군 발사 flag
extern bool fire;


/*//////////////////////////////
	display.c 함수 정의부분
*///////////////////////////////

// 화면 커서 안보이게 설정
void cs_Initial(void);

// 커서 이동
void cs_MoveCursor(int iPosX, int iPosY);

// 버퍼 초기화
void initBuf(char buf[][WIDTH]);

// 버퍼 플립
void flip(char rander[][WIDTH], char back[][WIDTH]);

// 화면 출력
void display(char rander[][WIDTH]);

/*///////////////////////////////
	keyInput.c 함수 정의부분
*////////////////////////////////

// 메시지 큐에 쓰기
bool enqueue(byte cmd);

// 메시지 큐에서 읽기
bool dequeue(byte *reader);

// 키값입력에 따른 메시지 큐 쓰기
void inputKey();

/*////////////////////////////
	logic.c 함수 정의부분
*/////////////////////////////

// 총알 초기화
void initBullet(BULLET *bullet, bool flag, bool visual, short x, short y);

// 적 초기화
void initEnemy(ENEMY *enemy, bool visual, short hp, short x, short y);

// 플레이어 초기화
void initPlain(PLAIN *player, bool visual, short hp, short x, short y);

// 플레이어 격추
void damagePlayer(PLAIN *player, char *back[][WIDTH], int bullet_x, int bullet_y);

// 적 격추
void damageEnemy(ENEMY *enemy, char *back[][WIDTH], int bullet_x, int bullet_y);

// 총알 지우기
void removeBullet(BULLET *bullet, char back[][WIDTH]);

// 총알 이동 및 충돌 체크 ( 사용시 visual == TRUE 가 맞는지 확인 )
void moveBullet(BULLET *bullet, char back[][WIDTH]);

// 총알 그리기
void paintBullet(BULLET *bullet, char back[][WIDTH]);

// 적 지우기
void removeEnemy(ENEMY *enemy, char back[][WIDTH]);

// 적 움직이기
void moveEnemy(ENEMY *enemy, char back[][WIDTH]);

void paintEnemy(ENEMY *enemy, char back[][WIDTH]);

// 플레이어 지우기
void removePlayer(PLAIN *player, char back[][WIDTH]);

// 플레이어 움직이기
void movePlayer(PLAIN *player);

// 플레이어 그리기
void paintPlayer(PLAIN *player, char back[][WIDTH]);

// 게임오버 체크
void overCheck(ENEMY *enemy, PLAIN player);

// 상태바 표기
void stateView(PLAIN player, char back[][WIDTH]);


#include<stdio.h>
#include<Windows.h>
#include<stdbool.h>
#include<time.h>
#include<stdlib.h>

/*/////////////////////////
		define ����
*//////////////////////////

// ���� ȭ�� size
#define WIDTH 127
#define HEIGHT 32

// ����ȭ�� test size
//#define WIDTH 11
//#define HEIGHT 5

// �޽��� ť size = 10, 1�� �����۸� ���� �����
#define QUEUE_SIZE 11

// �޽��� ť test size
//#define QUEUE_SIZE 5

// �޽��� ť �Է� �� cmd ����
#define UP    1
#define DOWN  2
#define LEFT  3
#define RIGHT 4
#define Z	  5

// �Ѿ� �� �� �迭 ũ��
#define MAX_ENEMY 20
#define MAX_BULLET 500

// ������ �÷���
#define START 0
#define GAME 1
#define OVER 2
#define END  3

/*///////////////////////////
      ����ü ���Ǻκ�
*////////////////////////////

typedef struct {

	bool flag; // �Ѿ� �ǾƱ��� (TRUE : �Ʊ�, FALSE : ����)
	bool visual;

	short x; // �Ѿ� X
	short y; // �Ѿ� Y

}BULLET;

typedef struct {

	bool visual; // ���̱� �Ⱥ��̱�

	short hp;	 // hp

	short x;	 // X
	short y;	 // Y

}PLAIN;

typedef struct {

	bool visual; // ���̱� �Ⱥ��̱�

	short hp;
	
	short x;
	short y;

	short vector;
	short moveCnt;

}ENEMY;


/*//////////////////////////////
		exturn ����κ�
*///////////////////////////////

// ���� ������ �÷���
extern byte g_sequence;

// �ܼ� �ʱ�ȭ
extern bool g_initConsol;

// ���� ���� �ð�
extern clock_t startTime;
extern clock_t gameTime;
extern clock_t overTime;

// ���� ���̵�
extern byte g_level;

// ���� ���ھ�
extern int g_score;

// ��Ʈ�� �޽��� ť ���� �۷ι� ����
extern byte g_message[QUEUE_SIZE];
extern byte g_mFront;
extern byte g_mRear;
extern byte g_mLen;

// �����
extern char back[HEIGHT][WIDTH];

// �÷��̾� �� ��
extern PLAIN player;
extern ENEMY enemy[MAX_ENEMY];

// �Ʊ� �߻� flag
extern bool fire;


/*//////////////////////////////
	display.c �Լ� ���Ǻκ�
*///////////////////////////////

// ȭ�� Ŀ�� �Ⱥ��̰� ����
void cs_Initial(void);

// Ŀ�� �̵�
void cs_MoveCursor(int iPosX, int iPosY);

// ���� �ʱ�ȭ
void initBuf(char buf[][WIDTH]);

// ���� �ø�
void flip(char rander[][WIDTH], char back[][WIDTH]);

// ȭ�� ���
void display(char rander[][WIDTH]);

/*///////////////////////////////
	keyInput.c �Լ� ���Ǻκ�
*////////////////////////////////

// �޽��� ť�� ����
bool enqueue(byte cmd);

// �޽��� ť���� �б�
bool dequeue(byte *reader);

// Ű���Է¿� ���� �޽��� ť ����
void inputKey();

/*////////////////////////////
	logic.c �Լ� ���Ǻκ�
*/////////////////////////////

// �Ѿ� �ʱ�ȭ
void initBullet(BULLET *bullet, bool flag, bool visual, short x, short y);

// �� �ʱ�ȭ
void initEnemy(ENEMY *enemy, bool visual, short hp, short x, short y);

// �÷��̾� �ʱ�ȭ
void initPlain(PLAIN *player, bool visual, short hp, short x, short y);

// �÷��̾� ����
void damagePlayer(PLAIN *player, char *back[][WIDTH], int bullet_x, int bullet_y);

// �� ����
void damageEnemy(ENEMY *enemy, char *back[][WIDTH], int bullet_x, int bullet_y);

// �Ѿ� �����
void removeBullet(BULLET *bullet, char back[][WIDTH]);

// �Ѿ� �̵� �� �浹 üũ ( ���� visual == TRUE �� �´��� Ȯ�� )
void moveBullet(BULLET *bullet, char back[][WIDTH]);

// �Ѿ� �׸���
void paintBullet(BULLET *bullet, char back[][WIDTH]);

// �� �����
void removeEnemy(ENEMY *enemy, char back[][WIDTH]);

// �� �����̱�
void moveEnemy(ENEMY *enemy, char back[][WIDTH]);

void paintEnemy(ENEMY *enemy, char back[][WIDTH]);

// �÷��̾� �����
void removePlayer(PLAIN *player, char back[][WIDTH]);

// �÷��̾� �����̱�
void movePlayer(PLAIN *player);

// �÷��̾� �׸���
void paintPlayer(PLAIN *player, char back[][WIDTH]);

// ���ӿ��� üũ
void overCheck(ENEMY *enemy, PLAIN player);

// ���¹� ǥ��
void stateView(PLAIN player, char back[][WIDTH]);

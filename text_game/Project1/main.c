
#include"setting.h"

// ���� ������ �÷���
byte g_sequence = 0;

// �ܼ� �ʱ�ȭ
bool g_initConsol = TRUE;

// ���� ���� �ð�
clock_t startTime;
clock_t gameTime;
clock_t overTime;

// ���� ���̵�
byte g_level = 1;

// ���� ���ھ�
int g_score = 0;

// ��Ʈ�� �޽��� ť �� front, rear����
byte g_message[QUEUE_SIZE];
byte g_mFront = 1;
byte g_mRear = 1;
byte g_mLen; // 0 �ʱ�ȭ

// ��������, ����� ����
char rander[HEIGHT][WIDTH];
char back[HEIGHT][WIDTH];

// �÷��̾� �� �� ���� 8 * 4 
PLAIN player;
ENEMY enemy[MAX_ENEMY];

// �Ѿ� �迭
BULLET bullet[MAX_BULLET];

// �Ѿ� �� cnt, �÷��̾� �߻� flag
unsigned short b_enemyCnt = 0;
unsigned short b_playerCnt = 0;
unsigned short bulletCnt = 0;
bool fire = FALSE;

// ���� �� �Լ�
void start();
void game();
void over();

void main() {

	// ȭ�� Ŀ�� �Ⱥ��̰� ����
	cs_Initial();

	g_sequence = START;

	while (1) {
		
		switch (g_sequence) {

		case START: start();
			break;
			
		case GAME: game();
			break;

		case OVER: over();
			break;
		
		case END: system("mode con: cols=100 lines=20");
			return; 
		}
		
		// ������
		Sleep(100);
	}

	return;
}

void start() {

	static int flash = 0;

	cs_MoveCursor(0, 0);

	if (g_initConsol) {
		
		system("mode con: cols=38 lines=15");
		g_initConsol = FALSE;
		startTime = clock();
	}

	char printFront[14][38] = { "=====================================",	// 0
								"!                                   !",	// 1
								"!        < SHOOTING GAME >          !",	// 2
								"!                                   !",	// 3
								"!      Control                      !",	// 4
								"!      Up    key : move up          !",	// 5
								"!      Down  key : move down        !",	// 6
								"!      Left  key : move left        !",	// 7
								"!      Right key : move right       !",	// 8
								"!      Z     key : shoot bullet     !",	// 9
								"!                                   !",	// 10
								"!                                   !",    // 11
								"!                                   !",	// 12
								"=====================================" };	// 13

							//   0123456789012345678901234567890123456

	char blank[20] = "                    ";
	char press[20] = "press Z key for play";

	// Ű �Էº�(Z��ư)
	if (GetKeyState(0x5a) & 0x8001) {

		g_sequence = GAME;
		g_initConsol = TRUE;
		
		return;
	}

	// ������
	if (flash < 5) {

		memcpy(&printFront[11][8],&press[0],sizeof(press));
	}
	else if (flash >= 5) {

		memcpy(&printFront[11][8], &blank[0], sizeof(blank));
	}
	
	if ((flash++) == 10) flash = 0;
	
	// ������ ��
	
	for (int i = 0; i < 14; i++) {

		cs_MoveCursor(0, i);
		printf("%s\n", printFront[i]);
	}
}

void game() {

	/*///////////////
		 �ʱ�ȭ
	///////////////*/	

	static int shoot = 0;
	int shootTime = 100 - g_level*10;

	cs_MoveCursor(0, 0);

	if (g_initConsol) {
		system("cls");
		system("mode con: cols=127 lines=32");
		g_initConsol = FALSE;
		gameTime = clock();

		// ���� �ʱ�ȭ
		initBuf(back);

		// �÷��̾� �ʱ�ȭ
		initPlain(&player, TRUE, 3, WIDTH / 2, HEIGHT / 5 * 4);

		// �� �ʱ�ȭ (4 7 10 13 HEIGHT)
		int cnt = 0; // �� ���� �� Ȯ��
		for (int x = 8; x < WIDTH - 15; x += 16) initEnemy(&enemy[cnt++], TRUE, g_level, x, 4);
		for (int x = 16; x < WIDTH - 15; x += 16) initEnemy(&enemy[cnt++], TRUE, g_level, x, 7);
		for (int x = 8; x < WIDTH - 15; x += 16) initEnemy(&enemy[cnt++], TRUE, g_level, x, 10);
		
		// �Ѿ� �ʱ�ȭ
		for (int i = 0; i < MAX_BULLET; i++) bullet[i].visual = FALSE;

		// ù�� �߻� ���� �ʱ�ȭ
		shoot = shootTime;
	}

	/*////////////////
		Ű�Է� ��Ʈ
	////////////////*/

	// Ű�Է�
	inputKey();

	/*////////////////
		 ���� ��Ʈ
	*/////////////////

	// ���� �Ѿ� ����	
	for (int i = 0; i < MAX_BULLET; i++)
		if (bullet[i].visual == TRUE) removeBullet(&bullet[i], back);
//	printf("");
	// �Ѿ� �̵� �� �浹 ó��
	for (int i = 0; i < MAX_BULLET; i++)
		if (bullet[i].visual == TRUE) moveBullet(&bullet[i], back);
	
	// �̵��� �Ѿ� �׸���
	for(int i=0; i<MAX_BULLET; i++) 
		if(bullet[i].visual == TRUE) paintBullet(&bullet[i], back);

	// Ű�Է¿� ���� �÷��̾� ������ ( �Ѿ� ���� ���� )
	removePlayer(&player, back);
	movePlayer(&player, back);
	paintPlayer(&player, back);
	if (fire) {
		initBullet(&bullet[bulletCnt], TRUE, TRUE, player.x, player.y);
		bulletCnt++;
		fire = FALSE;
	}

	// �� ������
	for (int i = 0; i < MAX_ENEMY; i++) {
		if (enemy[i].visual == TRUE) {
			removeEnemy(&enemy[i], back);
			moveEnemy(&enemy[i], back);
			paintEnemy(&enemy[i], back);
			
			if (shoot % shootTime == 0) {
				initBullet(&bullet[bulletCnt], FALSE, TRUE, enemy[i].x, enemy[i].y); // 20�� ���� ������ �ȵŴµ� 
				bulletCnt++;
			}
		}
	}
	shoot++;

	// �Ѿ� ī��Ʈ �ʱ�ȭ
	if (bulletCnt > MAX_BULLET - 1) bulletCnt = 0;

	// ���¹� ǥ��
	stateView(player, back);

	// ���� ���� ���� üũ
	overCheck(enemy, player);
	
	/*////////////////
		������ ��Ʈ
	////////////////*/

	// ���� �ø�
	flip(rander, back);

	// ȭ�� ���
	display(rander);
}

void over() {

	static int flash = 0;
	static unsigned int playTime = 0;

	static char pTime[10];
	static char lastLevel[10];
	static char score[10];

	cs_MoveCursor(0, 0);

	if (g_initConsol) {
		system("mode con: cols=38 lines=15");
		g_initConsol = FALSE;
		overTime = clock();

		playTime = (overTime - gameTime) / CLOCKS_PER_SEC;
		g_score = g_level * g_score;
		itoa((int)playTime, pTime, 10);
		itoa((int)g_level, lastLevel, 10);
		itoa((int)g_score, score, 10);
	}

	char printFront[14][38] = { "=====================================",	// 0
								"!                                   !",	// 1
								"!           < GAME OVER >           !",	// 2
								"!                                   !",	// 3
								"!      Play time  :                 !",	// 4
								"!                                   !",	// 5
								"!      Last level :                 !",	// 6
								"!                                   !",	// 7
								"!      Score      :                 !",	// 8
								"!                                   !",	// 9
								"!                                   !",	// 10
								"!                                   !",    // 11
								"!                                   !",	// 12
								"=====================================" };	// 13

							//   0123456789012345678901234567890123456

	char blank[20] = "                    ";
	char press[20] = "One more play? [Y/N]";

	// Ű �Էº�(Y��ư / N��ư)
	if (GetKeyState(0x59) & 0x8000) {

		g_sequence = GAME;
		g_initConsol = TRUE;
		g_level = 1;
		g_score = 0;
		return;
	}
	else if (GetKeyState(0x4E) & 0x8000) {

		g_sequence = END;
		return;
	}

	// ������
	for (int i = 0; i < 10; i++) {

		if (pTime[i] == '\0') break;
		printFront[4][20 + i] = pTime[i];
	}

	for (int i = 0; i < 10; i++) {

		if (lastLevel[i] == '\0') break;
		printFront[6][20 + i] = lastLevel[i];
	}
	for (int i = 0; i < 10; i++) {

		if (score[i] == '\0') break;
		printFront[8][20 + i] = score[i];
	}

	if (flash < 5) {

		memcpy(&printFront[11][8], &press[0], sizeof(press));
	}
	else if (flash >= 5) {

		memcpy(&printFront[11][8], &blank[0], sizeof(blank));
	}
	
	if ((flash++) == 10) flash = 0;

	// ������ ��
	for (int i = 0; i < 14; i++) {

		cs_MoveCursor(0, i);
		printf("%s\n", printFront[i]);
	}

}

/*///////////////////// �޽��� ť �׽�Ʈ ///////////////////////////

	if (dequeue(reader)) printf("%d\n", *reader);
	else printf("empty\n");
	if (enqueue(UP) == FALSE) printf("full\n");
	if (enqueue(DOWN) == FALSE) printf("full\n");
	if (enqueue(LEFT) == FALSE) printf("full\n");
	if (enqueue(RIGHT) == FALSE) printf("full\n");
	if (enqueue(CTRL) == FALSE) printf("full\n");
	if (dequeue(reader)) printf("%d\n", *reader);
	else printf("empty\n");
	if (dequeue(reader)) printf("%d\n", *reader);
	else printf("empty\n");
	if (dequeue(reader)) printf("%d\n", *reader);
	else printf("empty\n");
	if (dequeue(reader)) printf("%d\n", *reader);
	else printf("empty\n");

	for (int i = 0; i < QUEUE_SIZE; i++) printf("%d ", g_message[i]);

///////////////////////////////////////////////////////////////////*/

/*////////////// display test ////////////////

// �ʱ� ȭ�� ���
flip(rander, back);
display(rander);

Sleep(10000);

////////////////////////////////////////////*/


#include"setting.h"

// 게임 시퀀스 플래그
byte g_sequence = 0;

// 콘솔 초기화
bool g_initConsol = TRUE;

// 게임 진행 시간
clock_t startTime;
clock_t gameTime;
clock_t overTime;

// 게임 난이도
byte g_level = 1;

// 게임 스코어
int g_score = 0;

// 컨트롤 메시지 큐 및 front, rear선언
byte g_message[QUEUE_SIZE];
byte g_mFront = 1;
byte g_mRear = 1;
byte g_mLen; // 0 초기화

// 랜더버퍼, 백버퍼 선언
char rander[HEIGHT][WIDTH];
char back[HEIGHT][WIDTH];

// 플레이어 및 적 생성 8 * 4 
PLAIN player;
ENEMY enemy[MAX_ENEMY];

// 총알 배열
BULLET bullet[MAX_BULLET];

// 총알 수 cnt, 플레이어 발사 flag
unsigned short b_enemyCnt = 0;
unsigned short b_playerCnt = 0;
unsigned short bulletCnt = 0;
bool fire = FALSE;

// 메인 내 함수
void start();
void game();
void over();

void main() {

	// 화면 커서 안보이게 설정
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
		
		// 프레임
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

	// 키 입력부(Z버튼)
	if (GetKeyState(0x5a) & 0x8001) {

		g_sequence = GAME;
		g_initConsol = TRUE;
		
		return;
	}

	// 로직부
	if (flash < 5) {

		memcpy(&printFront[11][8],&press[0],sizeof(press));
	}
	else if (flash >= 5) {

		memcpy(&printFront[11][8], &blank[0], sizeof(blank));
	}
	
	if ((flash++) == 10) flash = 0;
	
	// 랜더링 부
	
	for (int i = 0; i < 14; i++) {

		cs_MoveCursor(0, i);
		printf("%s\n", printFront[i]);
	}
}

void game() {

	/*///////////////
		 초기화
	///////////////*/	

	static int shoot = 0;
	int shootTime = 100 - g_level*10;

	cs_MoveCursor(0, 0);

	if (g_initConsol) {
		system("cls");
		system("mode con: cols=127 lines=32");
		g_initConsol = FALSE;
		gameTime = clock();

		// 버퍼 초기화
		initBuf(back);

		// 플레이어 초기화
		initPlain(&player, TRUE, 3, WIDTH / 2, HEIGHT / 5 * 4);

		// 적 초기화 (4 7 10 13 HEIGHT)
		int cnt = 0; // 적 생성 수 확인
		for (int x = 8; x < WIDTH - 15; x += 16) initEnemy(&enemy[cnt++], TRUE, g_level, x, 4);
		for (int x = 16; x < WIDTH - 15; x += 16) initEnemy(&enemy[cnt++], TRUE, g_level, x, 7);
		for (int x = 8; x < WIDTH - 15; x += 16) initEnemy(&enemy[cnt++], TRUE, g_level, x, 10);
		
		// 총알 초기화
		for (int i = 0; i < MAX_BULLET; i++) bullet[i].visual = FALSE;

		// 첫발 발사 위한 초기화
		shoot = shootTime;
	}

	/*////////////////
		키입력 파트
	////////////////*/

	// 키입력
	inputKey();

	/*////////////////
		 로직 파트
	*/////////////////

	// 이전 총알 제거	
	for (int i = 0; i < MAX_BULLET; i++)
		if (bullet[i].visual == TRUE) removeBullet(&bullet[i], back);
//	printf("");
	// 총알 이동 및 충돌 처리
	for (int i = 0; i < MAX_BULLET; i++)
		if (bullet[i].visual == TRUE) moveBullet(&bullet[i], back);
	
	// 이동된 총알 그리기
	for(int i=0; i<MAX_BULLET; i++) 
		if(bullet[i].visual == TRUE) paintBullet(&bullet[i], back);

	// 키입력에 따른 플레이어 움직임 ( 총알 생성 포함 )
	removePlayer(&player, back);
	movePlayer(&player, back);
	paintPlayer(&player, back);
	if (fire) {
		initBullet(&bullet[bulletCnt], TRUE, TRUE, player.x, player.y);
		bulletCnt++;
		fire = FALSE;
	}

	// 적 움직임
	for (int i = 0; i < MAX_ENEMY; i++) {
		if (enemy[i].visual == TRUE) {
			removeEnemy(&enemy[i], back);
			moveEnemy(&enemy[i], back);
			paintEnemy(&enemy[i], back);
			
			if (shoot % shootTime == 0) {
				initBullet(&bullet[bulletCnt], FALSE, TRUE, enemy[i].x, enemy[i].y); // 20이 들어옴 들어오면 안돼는데 
				bulletCnt++;
			}
		}
	}
	shoot++;

	// 총알 카운트 초기화
	if (bulletCnt > MAX_BULLET - 1) bulletCnt = 0;

	// 상태바 표시
	stateView(player, back);

	// 게임 오버 상태 체크
	overCheck(enemy, player);
	
	/*////////////////
		랜더링 파트
	////////////////*/

	// 버퍼 플립
	flip(rander, back);

	// 화면 출력
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

	// 키 입력부(Y버튼 / N버튼)
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

	// 로직부
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

	// 랜더링 부
	for (int i = 0; i < 14; i++) {

		cs_MoveCursor(0, i);
		printf("%s\n", printFront[i]);
	}

}

/*///////////////////// 메시지 큐 테스트 ///////////////////////////

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

// 초기 화면 출력
flip(rander, back);
display(rander);

Sleep(10000);

////////////////////////////////////////////*/

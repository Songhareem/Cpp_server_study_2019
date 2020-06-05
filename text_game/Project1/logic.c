
#include"setting.h"

// �Ѿ� �ʱ�ȭ
void initBullet(BULLET *bullet, bool flag, bool visual, short x, short y) {

	bullet->flag = flag;
	bullet->visual = visual;
	bullet->x = x;
	bullet->y = y;

}

// �� �ʱ�ȭ
void initEnemy(ENEMY *enemy, bool visual, short hp, short x, short y) {

	enemy->visual = visual;
	enemy->hp = hp;
	enemy->x = x;
	enemy->y = y;

	enemy->vector = 1;  // �����̴� ����
	enemy->moveCnt = 0; // ��� ����������
}

// �÷��̾� �ʱ�ȭ
void initPlain(PLAIN *player, bool visual, short hp, short x, short y) {

	player->visual = visual;
	player->hp = hp;
	player->x = x;
	player->y = y;
}

// �÷��̾� ����
void damagePlayer(PLAIN *player, char *back[][WIDTH], int bullet_x, int bullet_y) {

	// �Ѿ� ��ġ�� �� ��ġ�� ����, ���� ����ִٸ�
	if (player->x == bullet_x && player->y == bullet_y && player->visual == TRUE) {

		player->hp -= 1;

		if (player->hp == 0) {
			player->visual = FALSE;
			back[player->y][player->x] == ' ';
		}
	}
}

// �� ����
void damageEnemy(ENEMY *enemy,char *back[][WIDTH],int bullet_x, int bullet_y) {

	// �Ѿ� ��ġ�� �� ��ġ�� ����, ���� ����ִٸ�
	if (enemy->x == bullet_x && enemy->y == bullet_y && enemy->visual == TRUE) {

		enemy->hp -= 1;
		g_score++;

		if (enemy->hp == 0) {
			enemy->visual = FALSE;
			back[enemy->y][enemy->x] == ' ';
		}
	}
}

// �Ѿ� �����
void removeBullet(BULLET *bullet, char back[][WIDTH]) {

	if (back[bullet->y][bullet->x] != 'T' && back[bullet->y][bullet->x] != 'O') {

		back[bullet->y][bullet->x] = ' ';
	}
}

// �Ѿ� �̵� �� �浹 üũ ( ���� visual == TRUE �� �´��� Ȯ�� )
void moveBullet(BULLET *bullet, char back[][WIDTH]) {

	short origin_x = bullet->x;
	short origin_y = bullet->y;
	short change_x = bullet->x;
	short change_y = bullet->y;

	int dis_x = abs(player.x - origin_x);
	
	// �Ѿ� �̵�
	switch (bullet->flag) {

	case FALSE:
		if (dis_x > 0) {

			if (origin_x < player.x)	  change_x += 1;
			else if (origin_x > player.x) change_x -= 1;
		}
		change_y += 1;
		break;

	case TRUE: 
		change_y -= 1;
		break;

	default: break;

	}
	
	// ������ �浹 üũ
	if (origin_y <= 0 || origin_y >= HEIGHT - 1 || origin_x <= 0 || origin_x >= WIDTH - 3) {

		bullet->visual = FALSE;
		return;
	}
	
	if (change_y <= 0 || change_y >= HEIGHT - 1 || change_x <= 0 || change_x >= WIDTH - 3) {

		bullet->visual = FALSE;
		return;
	}
	
	// �Ǿ� �Ѿ˿� ���� �浹üũ
	switch (bullet->flag) {

	case TRUE:	// �Ѿ��� �Ʊ��� ��
		if (back[change_y][change_x] == 'T') {
			for (int i = 0; i < MAX_ENEMY; i++) {
				damageEnemy(&enemy[i], back, change_x, change_y);
			}
		}
		break;

	case FALSE:	// �Ѿ��� ������ ��
		if (back[change_y][change_x] == 'O') {
			damagePlayer(&player, back, change_x, change_y);
		}
		break;
	}
	//*/

	// �ٲ� x,y��ǥ �ݿ�
	bullet->x = change_x;
	bullet->y = change_y;
}

// �Ѿ� �׸���
void paintBullet(BULLET *bullet, char back[][WIDTH]) {

	int x = bullet->x;
	int y = bullet->y;
	int visual = bullet->visual;
	int flag = bullet->flag;

	switch (flag) {

	case TRUE: // �Ʊ� �Ѿ�
		back[y][x] = 'I';
		break;
	
	case FALSE: // ���� �Ѿ�
		back[y][x] = 'V';
		break;
	}
}

// �� �����
void removeEnemy(ENEMY *enemy, char back[][WIDTH]) {

	back[enemy->y][enemy->x] = ' ';
}

// �� �����̱�
void moveEnemy(ENEMY *enemy, char back[][WIDTH]) {

	short origin_x = enemy->x;
	short origin_y = enemy->y;
	short change_x = enemy->x;
	short change_y = enemy->y;

	change_x = origin_x + enemy->vector;
	enemy->moveCnt += 1;
	
	if (enemy->moveCnt > 10) {

		enemy->vector = enemy->vector * -1;
		enemy->moveCnt = 0;
		//change_y = origin_y + 1;
	}

	enemy->x = change_x;
	//enemy->y = change_y;
}

void paintEnemy(ENEMY *enemy, char back[][WIDTH]) {

	back[enemy->y][enemy->x] = 'T';
}

// �÷��̾� �����
void removePlayer(PLAIN *player, char back[][WIDTH]) {

	back[player->y][player->x] = ' ';
}

// �÷��̾� �����̱�
void movePlayer(PLAIN *player) {

	int origin_x = player->x;
	int origin_y = player->y;
	int change_x = player->x;
	int change_y = player->y;

	byte tmp;
	byte *reader = &tmp;

	while (g_mLen > 0) {

		dequeue(reader);

		switch(*reader) {
		
		case UP: 
			if (origin_y > 1) change_y--;
			break;
			
		case DOWN: 
			if (origin_y < HEIGHT-2) change_y++;
			break;
		
		case LEFT: 
			if (origin_x > 1) change_x-=2;
			break;

		case RIGHT: 
			if (origin_x < WIDTH-4) change_x+=2;
			break;

		case Z: 
			fire = TRUE;
			break;

		default: break;
		}
	}

	player->x = change_x;
	player->y = change_y;
}

// �÷��̾� �׸���
void paintPlayer(PLAIN *player, char back[][WIDTH]) {

	back[player->y][player->x] = 'O';
}

// ���ӿ��� üũ
void overCheck(ENEMY *enemy, PLAIN player) {

	int cnt = 0;

	for (int i = 0; i < MAX_ENEMY; i++) {

		if (enemy[i].visual == FALSE) cnt++;
	}

	if (cnt == MAX_ENEMY) {
		g_level++;
		g_initConsol = TRUE;
	}

	if (player.visual == FALSE) {
		g_sequence = OVER;
		g_initConsol = TRUE;
		return;
	}
}

// ���� ��
void stateView(PLAIN player, char back[][WIDTH]) {

	int hp = player.hp;

	char level1[20] = "LEVEL : ";
	char level2[5] = "     ";

	char score1[20] = "SCORE : ";
	char score2[10] = "     ";

	char life1[20] = "HP : ";
	char life2[5] = "     ";

	itoa((int)g_level, &level2[0], 10);
	itoa((int)g_score, &score2[0], 10);
	itoa((int)player.hp, &life2[0], 10);

	strcat(level1, level2);
	strcat(score1, score2);
	strcat(life1, life2);

	for (int i = 0; i < 20; i++) {

		if (level1[i] == '\0') break;
		back[2][4 + i] = level1[i];
	}

	for (int i = 0; i < 20; i++) {

		if (score1[i] == '\0') break;
		back[2][34 + i] = score1[i];
	}

	for (int i = 0; i < 20; i++) {

		if (life1[i] == '\0') break;
		back[2][64 + i] = life1[i];
	}
}
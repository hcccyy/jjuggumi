#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"

#define DIR_UP		0
#define DIR_DOWN	1
#define DIR_STAY	2
#define DIR_LEFT	3

bool next_game;

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // 각 플레이어 위치, 이동 주기

int young_x, young_y;
bool young_change = false;

bool pass_player[PLAYER_MAX] = { 0 };
int death_count = 0;
bool move_1 = false;
bool move_player[PLAYER_MAX] = { 0 };
int alive_player=0;
int died[PLAYER_MAX];
int live_count = 0;

void mugunghwa_dialog(int p[]) {
	int i, j;
	
	for (i = DIALOG_DURATION_SEC; i > 0; i--) {
		gotoxy(4, 10);

		for (int j = 0; j < 30; j++) {
			printf("*");
		}
		printf("\n");

		gotoxy(5, 10);
		
		printf("  %d 탈락자명단:", i);
		for (int k = 0; k < n_player; k++) {
			if (p[k] != 0) {
				printf(" %d ", p[k]);
			}
		}
		printf("\n");


		gotoxy(6, 10);
		for (int j = 0; j < 30; j++) {
			printf("*");
		}

		Sleep(1000);
	}

	for (j = 0; j < 3; j++) {
		for (i = 4; i <= 6; i++) {
			gotoxy(i, 10);
			printf("                                 \n");
		}
	}
}

//void mugunghwa_dialog(int player) {
//	int i, j;
//	int p = player;
//	for (i = DIALOG_DURATION_SEC; i > 0; i--) {
//		gotoxy(4, 10);
//
//		for (int j = 0; j < 30; j++) {
//			printf("*");
//		}
//		printf("\n");
//
//		gotoxy(5, 10);
//		printf("  %d 탈락자명단:   %d \n", i, p);
//		
//		/*for (int j = 0; j < n_player; j++) {
//			if(move_player[j]==true)
//				printf(" %d ", j);
//		}*/
//
//		gotoxy(6, 10);
//		for (int j = 0; j < 30; j++) {
//			printf("*");
//		}
//
//		Sleep(1000);
//	}
//
//	for (j = 0; j < 3; j++) {
//		for (i = 4; i <= 6; i++) {
//			gotoxy(i, 10);
//			printf("                              \n");
//		}
//	}
//}


void younghyee_print()
{
	young_x = n_player / 2 + 1.5; young_y = 1;

	if (young_change == false) {
		for (int i = 0; i < 3; i++) {
			back_buf[young_x][young_y] = '#';
			young_x++;
		}
	}
	else {
		for (int i = 0; i < 3; i++) {
			back_buf[young_x][young_y] = '@';
			young_x++;
		}
	}
}

void mugunghwa_init(void)
{
	map_init(n_player + 4, 45);

	younghyee_print();

	//canvas.c map_init에서 문자 #->*
	int x = 2, y = N_COL - 2;
	for (int i = 0; i < n_player; i++) {
		if (pass_player[i] == true || player[i] == false) continue;
		px[i] = x;
		py[i] = y;
		period[i] = randint(400, 500);	//

		back_buf[px[i]][py[i]] = '0' + i; 
		x++;
	}
}

int get_rand(bool move, bool stop)
{
	int random = rand();

	double r = random / (double)RAND_MAX;
	double dr = r * 100.0f;

	double p1[4] = { 10.0f, 10.0f, 10.0f, 70.0f };
	double p2[2] = { 10.0f, 90.0f};	

	double cumulative = 0.0f;

	if (move) {
		for (int i = 0; i < 4; i++) {
			cumulative += p1[i];

			if (dr <= cumulative) return i;
		}
	}

	if (stop) {
		for (int i = 0; i < 2; i++) {
			cumulative += p2[i];

			if (dr <= cumulative) return i;
		}
	}
}

void enemy_move_manual(int player) {
	int p = player;
	int nx, ny;  // 움직여서 다음에 놓일 자리
	int dir;

	// 각 방향으로 움직일 때 x, y값 delta
	static int dx[4] = { -1, 1, 0, 0 };
	static int dy[4] = { 0, 0, 0, -1 };

	if (pass_player[p] == true || player == false) return;

	if (young_change == true) {
		switch (get_rand(0, 1)) {
		case 0: dir = DIR_LEFT; break;
		case 1: dir = DIR_STAY; break;
		default: return;
		}
	}
	else {
		switch (get_rand(1, 0)) {
		case 0: dir = DIR_UP; break;
		case 1: dir = DIR_DOWN; break;
		case 2: dir = DIR_STAY; break;
		case 3: dir = DIR_LEFT; break;
		default: return;
		}
	}

	nx = px[p] + dx[dir];
	ny = py[p] + dy[dir];
	if (!placable(nx, ny)) return;
	
	move_tail(p, nx, ny);
}

int s = 0;
void Say_mugunghwa()
{
	char* say[10] = { "무", "궁", "화", "꽃", "이", "피", "었", "습", "니", "다" };

	if (s >= 6 && s < 10) {
		if (tick % 50 * (10-s) == 0) {	//
			gotoxy(N_ROW, s *3);
			printf("%s ", say[s]);
			s++;
			tick = 0;
		}
	}
	else if (s < 6) {
		if (tick % (600 + 200 * s) == 0) { //
			gotoxy(N_ROW, s*3);
			printf("%s ", say[s]);
			s++;
			tick = 0;
		}
	}

	if (s == 10) {
		young_change = true;
		if (tick >= 2990) {
			s = 0;
			tick = 0;
			young_change = false;
			move_1 = false;

			for (int i = 0; i <= 30; i++) {
				printxy(' ', N_ROW, i);
			}
		}
	}
}

bool behind_move(int p)
{
	int front, back;
	for (int i = 0; i < n_player; i++) {
		if (py[i] > py[p]) front = i, back = p;
		else front = p, back = i;

		if (i != p && px[front] == px[back]) {
			if (front_buf[px[back]][py[back]] != back_buf[px[back]][py[back]]) {
				player[back] = true;
				move_player[back] = false;
				return true;
			}
		}
	}
	return false;
}

bool move_check(int p)
{
	key_t key = get_key();
	move_player[p] = false;

	if (front_buf[px[p]][py[p]] != back_buf[px[p]][py[p]] || key != K_UNDEFINED) {
		if (behind_move(p) == true) return;
		player[p] = false;
		move_player[p] = true;
		n_alive--;
	}
}


void mugunghwa(void)	
{
	srand((unsigned int)time(NULL));
	next_game = false;

	mugunghwa_init();

	system("cls");
	display();

	
	while (1) {
		
		Say_mugunghwa();
		younghyee_print();

		// player 0만 손으로 움직임(4방향)
		key_t key = get_key();
		if (key == K_QUIT) {
			break;
		}
		else if (key != K_UNDEFINED) {
			if (player[0] == false) continue;
			move_manual(key);
		}

		// 확률 이동
		for (int i = 1; i < n_player; i++) {
			if (tick % period[i] < 10 && move_1 == false) {
				enemy_move_manual(i);
			}
		}

		if (young_change == true) {
			move_check(0);
			if(move_player[0]==true)
				mugunghwa_dialog(0);

			if (move_player[0] == true) {
				back_buf[px[0]][py[0]] = ' ';
				px[0] = 0; py[0] = 0;
			}
		}
		
		// 영희가 뒤를 돌아봤을 때
		if (young_change == true && move_1 == false) {

			for (int i = 1; i < n_player; i++) {
				move_check(i);
			}

			for (int i = 0; i < n_player; i++) {
				if (pass_player[i] == true) continue;
				if (move_player[i] == true) {
					death_count++;
				}
			}

			move_1 = true;		//한 번만 움직이게
		}

		display();
		Sleep(10);
		tick += 10;
		
		if (young_change == true && death_count > 0) {

			for (int i = 0; i < n_player; i++) {
				if (player[i] == false) {
					died[i] = i;
				}
			}
			
			mugunghwa_dialog(died);
			
			death_count = 0;
			tick = 0;

			for (int i = 0; i < n_player; i++) {
				if (move_player[i] == true) {
					back_buf[px[i]][py[i]] = ' ';
					px[i] = 0; py[i] = 0;
				}
			}
			continue;
		}


		//플레이어 통과, 다음 게임으로
		young_x = n_player / 2 + 1.5; young_y = 1;
		for (int i = 0; i < n_player; i++) {
			if ((px[i] == young_x + 3 || px[i] == young_x - 1) && py[i] == young_y ||
				(py[i] == young_y + 1 && (px[i] == young_x || px[i] == young_x + 1 || px[i] == young_x + 2))) {
				pass_player[i] = true;

				//Sleep(500);
				back_buf[px[i]][py[i]] = ' ';
				px[i] = 0; py[i] = 0;

				live_count++;
			}
		}

		if (live_count == n_alive) break;
	}
}
#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"

#define WIN_RIGHT	11
#define WIN_LEFT	12
#define ING			13


int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // 각 플레이어 위치, 이동 주기
bool die_dialog[PLAYER_MAX] = { 0 };

bool dropped_player[PLAYER_MAX] = { 0 };

int center = 15;
double master_str = 0;

bool lie_work[2] = { 0 };
bool lie_1[2] = { 0 };


void player_pos() {
	int even_y = n_player * 5 / 2 - 2, odd_y = n_player * 5 / 2 + 2;
	int l_idx = 0, r_idx = 0;

	for (int i = 0; i < n_player; i++) {
		PLAYER* p = &player[i];

		if (p->is_alive == false) continue;

		if (i % 2 == 0) {
			py[i] = even_y;
			px[i] = 1;

			back_buf[px[i]][py[i]] = '0' + i;
			even_y--;
		}
		else {
			py[i] = odd_y;
			px[i] = 1;

			back_buf[px[i]][py[i]] = '0' + i;
			odd_y++;
		}
	}
}

void juldarigi_init(void) {
	map_init(3, n_player * 5);
	back_buf[0][n_player * 5 / 2] = ' ';
	back_buf[2][n_player * 5 / 2] = ' ';

	//줄
	back_buf[1][n_player * 5 / 2] = '-';
	back_buf[1][n_player * 5 / 2 + 1] = '-';
	back_buf[1][n_player * 5 / 2 - 1] = '-';

	//모든 플레이어 부활
	for (int i = 0; i < n_player; i++) {
		PLAYER* p = &player[i];

		if (p->is_alive == false) {
			//탈락자 기록
			dropped_player[i] = true;

			p->is_alive = true;
			n_alive++;

			p->hasitem = false;
		}
	}

	player_pos();
}

double vaild_str_sum() {
	double even_str = 0, odd_str = 0;

	for (int i = 0; i < n_player; i++) {
		PLAYER* p = &player[i];

		if (p->is_alive == false) continue;

		double vaild_str = p->str * (p->stamina * 0.01);
		if (i % 2 == 0) {
			even_str += vaild_str;
		}
		else {
			odd_str += vaild_str;
		}
	}

	double str_sum = master_str + odd_str - even_str;
	return str_sum;
}

int line_change() {
	if (vaild_str_sum() < 0) {
		if (lie_work[0] == true) {
			center -= 2;

			back_buf[0][center + 1] = '*';
			back_buf[0][center + 2] = '*';
			back_buf[2][center + 1] = '*';
			back_buf[2][center + 2] = '*';
		}
		else {
			center -= 1;

			back_buf[0][center + 1] = '*';
			back_buf[2][center + 1] = '*';
		}
	}
	else {
		if(lie_work[1]==true){
			center += 2;

			back_buf[0][center - 1] = '*';
			back_buf[0][center - 2] = '*';
			back_buf[2][center - 1] = '*';
			back_buf[2][center - 2] = '*';
		}
		else {
			center += 1;

			back_buf[0][center - 1] = '*';
			back_buf[2][center - 1] = '*';
		}
	}
}
//2개 합치는거 고민좀
void juldarigi_hole() {
	back_buf[0][center] = ' ';
	back_buf[2][center] = ' ';

	master_str = 0;
}

int die_player(PLAYER *p) {
	p->intel /= 2;
	p->str /= 2;
	p->hasitem = false;
}

void hole_die() {
	for (int i = 0; i < n_player; i++) {
		PLAYER* p = &player[i];
		
		if (py[i] == center && die_dialog[i] == false) {
			p->is_alive = false;
			n_alive--;

			if (i % 2 == 0) {
				back_buf[px[i]][py[i]] = ' ';
				back_buf[px[i]][py[i] - 1] = ' ';
				back_buf[px[i]][py[i] - 2] = ' ';
			}
			else {
				back_buf[px[i]][py[i]] = ' ';
				back_buf[px[i]][py[i] + 1] = ' ';
				back_buf[px[i]][py[i] + 2] = ' ';
			}

			if (dropped_player[i] == 0) die_player(p);
			Sleep(1000);
		}
	}
}

void lie() {
	if (lie_work[0] == true && lie_1[0] == false) {
		for (int i = 0; i < n_player; i++) {
			PLAYER* p = &player[i];

			if (i % 2 == 0) {
				p->str *= 2;
				lie_1[0] = true;
			}
		}
	}
	else if (lie_work[1] == true && lie_1[1] == false) {
		for (int i = 0; i < n_player; i++) {
			PLAYER* p = &player[i];

			if (i % 2 != 0) {
				p->str *= 2;
				lie_1[1] = true;
			}
		}
	}
}

void lie_after() {
	if (lie_work[0] == true) {
		for (int i = 0; i < n_player; i++) {
			PLAYER* p = &player[i];

			if (i % 2 == 0) {
				p->str /= 2;
				p->stamina -= (p->stamina * 0.3); 

				if (p->stamina <= 0) p->stamina = 0;
			}
		}
	}
	else {
		for (int i = 0; i < n_player; i++) {
			PLAYER* p = &player[i];

			if (i % 2 != 0) { 
				p->str /= 2;
				p->stamina -= (p->stamina * 0.3);

				if (p->stamina <= 0) p->stamina = 0;
			}
		}
	}

	lie_work[0] = false;
	lie_work[1] = false;
}

void juldarigi_dailog() {
	if (lie_work[0] == true) {
		gotoxy(N_ROW + 2, 0);
		printf("왼쪽 팀이 누웠습니다! 힘이 순간적으로 2배가 됩니다!\n");
	}
	else if (lie_work[1] == true) {
		gotoxy(N_ROW + 2, 0);
		printf("오른쪽 팀이 누웠습니다! 힘이 순간적으로 2배가 됩니다!\n");
	}

	for (int i = 0; i < n_player; i++) {
		PLAYER* p = &player[i];

		if (p->is_alive == false && die_dialog[i]==0) {
			gotoxy(N_ROW + 2, 0);
			printf("%d번 플레이어가 구멍에 빠졌습니다!\n", i);

			die_dialog[i] = 1;
			tick = 10;
		}
	}

	//다일로그 지우기
	if (tick % 1000 == 0) {
		gotoxy(N_ROW + 2, 0);
		printf("                                                        ");
	}
}

int winner_check() {
	int l_die_cnt = 0, r_die_cnt = 0;
	for (int i = 0; i < n_player; i++) {
		PLAYER* p = &player[i];

		if (i % 2 == 0 && p->is_alive == false) { l_die_cnt++; }
		else if (i % 2 != 0 && p->is_alive == false) { r_die_cnt++; }
	}

	if (l_die_cnt == 3) return WIN_LEFT;
	else if (r_die_cnt == 3) return WIN_RIGHT;
	else return ING;
}


tick = 10;
void juldarigi(void) {
	
	juldarigi_init();
	system("cls");
	display();


	while (1) {
		gotoxy(N_ROW + 1, 0);
		printf("                ");
		gotoxy(N_ROW + 1, 0);
		printf("str:	%.1f\n", vaild_str_sum());

		key_t key = get_key();
		if (key == K_QUIT) {
			break;
		}
		else if (key == K_STRDECR) { master_str -= 1; }
		else if (key == K_STRINCR) { master_str += 1; }
		else if (key == K_LIE_L) {
			lie_work[0] = true;
			lie();
		}
		else if (key == K_LIE_R) {
			lie_work[1] = true;
			lie();
		}


		if (tick % 1000 == 0) {	
			line_change();
			juldarigi_hole();

			display();
			
			
			if (lie_work[0] == true || lie_work[1] == true) {
				lie_after();
			}
			hole_die();
			player_pos();
		}

		display();
		Sleep(10);
		tick += 10;

		juldarigi_dailog();

		if (winner_check() != ING) break;
	}
}
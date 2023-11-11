#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"


int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // 각 플레이어 위치, 이동 주기
bool die_dialog[PLAYER_MAX] = { 0 };

int center = 15;
double master_str = 0;

void player_pos() {
	int even_y = n_player * 5 / 2 - 2, odd_y = n_player * 5 / 2 + 2;

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
		center -= 1;

		back_buf[0][center+1] = '*';
		back_buf[2][center+1] = '*';
	}
	else {
		center += 1;

		back_buf[0][center - 1] = '*';
		back_buf[2][center - 1] = '*';
	}
}
//2개 합치는거 고민좀
void juldarigi_hole() {
	back_buf[0][center] = ' ';
	back_buf[2][center] = ' ';

	master_str = 0;
}

void hole_die() {
	for (int i = 0; i < n_player; i++) {
		PLAYER* p = &player[i];

		if (py[i] == center) {
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

			Sleep(1000);
		}
	}
}

void juldarigi_dailog() {
	for (int i = 0; i < n_player; i++) {
		PLAYER* p = &player[i];

		if (p->is_alive == false && die_dialog[i]==0) {
			gotoxy(N_ROW + 2, 0);
			printf("%d번 플레이어가 구멍에 빠졌습니다!\n", i);

			die_dialog[i] = 1;
		}
	}

	//다일로그 지우기
	if (tick % 1000 == 0) {
		gotoxy(N_ROW + 2, 0);
		printf("                                 ");
	}
}

tick = 10;	//왜 잘되지? 시발? 왜? 왜 잘되지? 나 멈출수 없어 멈추면 존나 못할거같아 이제
void juldarigi(void) {
	
	juldarigi_init();
	system("cls");
	display();


	while (1) {
		gotoxy(N_ROW + 1, 0);
		printf("str:	%.1f\n", vaild_str_sum());

		key_t key = get_key();
		if (key == K_QUIT) {
			break;
		}
		else if (key == K_STRDECR) {
			master_str -= 1;
		}
		else if (key == K_STRINCR) {
			master_str += 1;
		}


		if (tick % 1000 == 0) {	
			line_change();
			juldarigi_hole();

			display();
			
			hole_die();
			player_pos();
		}

		display();
		Sleep(10);
		tick += 10;

		juldarigi_dailog();
	}
}
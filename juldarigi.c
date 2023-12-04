#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"

#define WIN_RIGHT	11
#define WIN_LEFT	12
#define ING			13


int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // 각 플레이어 위치, 이동 주기
bool die_dialog[PLAYER_MAX] = { 0 };

bool dropped_player[PLAYER_MAX] = { 0 };
bool star_present[PLAYER_MAX] = { 0 };

int center, jul;
double master_str = 0;

bool lie_work[2] = { 0 };
bool lie_1[2] = { 0 };


void juldarigi_line() {
	back_buf[1][jul] = '-';
	back_buf[1][jul + 1] = '-';
	back_buf[1][jul - 1] = '-';
}

void player_pos() {
	int even_y = jul - 2, odd_y = jul + 2;


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
	back_buf[0][center] = ' ';
	back_buf[2][center] = ' ';

	//줄
	juldarigi_line();

	//모든 플레이어 부활
	for (int i = 0; i < n_player; i++) {
		PLAYER* p = &player[i];

		if (p->is_alive == false) {
			//탈락자 기록(죽으면 힘, 지능 하락)
			dropped_player[i] = true;

			//별 표시
			star_present[i] = true;

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
			jul -= 2;
			juldarigi_line();
		}
		else {
			jul -= 1;
			juldarigi_line();
		}
	}
	else {
		if(lie_work[1]==true){
			jul += 2;
			juldarigi_line();
		}
		else {
			jul += 1;
			juldarigi_line();
		}
	}
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

			back_buf[px[i]][py[i]] = ' ';
			jul = 15;
			
			if (dropped_player[i] == 0) die_player(p);
			Sleep(1000);
		}
		
		//구멍을 지나갔을 때
		if (i % 2 == 0) {
			if (py[i] > center && die_dialog[i] == false) {
				p->is_alive = false;
				n_alive--;

				back_buf[px[i]][py[i]] = ' ';
				jul = 15;

				if (dropped_player[i] == 0) die_player(p);
				Sleep(1000);
			}
		}
		else {
			if (py[i] < center && die_dialog[i] == false) {
				p->is_alive = false;
				n_alive--;

				back_buf[px[i]][py[i]] = ' ';
				jul = 15;

				if (dropped_player[i] == 0) die_player(p);
				Sleep(1000);
			}
		}
	}
}

void lie() {
	if (lie_work[1] == true && lie_work[0] == true && lie_1[0] == false && lie_1[1] == false) {
		for (int i = 0; i < n_player; i++) {
			PLAYER* p = &player[i];

			p->str *= 2;
			lie_1[1] = true;
			lie_1[0] = true;
			printf("양쪽 다 누움!"); Sleep(1000);
		}
	}
	else if (lie_work[0] == true && lie_1[0] == false) {
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
	if (lie_work[0] && lie_work[1]) {
		for (int i = 0; i < n_player; i++) {
			PLAYER* p = &player[i];

			p->str /= 2;
			p->stamina -= (p->stamina * 0.3);

			if (p->stamina <= 0) p->stamina = 0;
		}
	}
	else if (lie_work[0] == true) {
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
	lie_1[0] = false;
	lie_1[1] = false;
}

void juldarigi_dailog() {
	//별 표시
	for (int i = 0; i < n_player; i++) {
		PLAYER* p = &player[i];

		if (star_present[i] == 1) {
			gotoxy(N_ROW + 8 + i, 16);
			printf("*");
		}
	}
	
	if (lie_work[0] && lie_work[1]) {
		gotoxy(N_ROW + 2, 0);
		printf("양쪽팀이 누웠습니다! 모든 플레이어의 힘이 순간적으로 2배가 됩니다!\n");
	}
	else if (lie_work[0] == true) {
		gotoxy(N_ROW + 2, 0);
		printf("왼쪽 팀이 누웠습니다! 힘이 순간적으로 2배가 됩니다!\n");
	}
	else if (lie_work[1] == true) {
		gotoxy(N_ROW + 2, 0);
		printf("오른쪽 팀이 누웠습니다! 힘이 순간적으로 2배가 됩니다!\n");
	}

	//몇명 죽었는지, 해당턴에 죽은p, 프린트 위치
	int die_1time = 0;
	bool die_now[PLAYER_MAX] = {0};
	int print_pos = 0;
	for (int i = 0; i < n_player; i++) {
		PLAYER* p = &player[i];

		if (p->is_alive == false && die_dialog[i] == false) {
			die_1time++;
			die_now[i] = true;

			die_dialog[i] = true;
			tick = 10;
		}
	}
	if (die_1time > 0) {
		for (int i = 0; i < n_player; i++) {
			if (die_now[i] == true) {
				gotoxy(N_ROW + 2, print_pos);
				printf("%d ", i);

				die_now[i] = false;
				print_pos += 2;
			}
		}
		gotoxy(N_ROW + 2, die_1time * 2);
		printf("번 플레이어가 구멍에 빠졌습니다!\n");
	}



	//다일로그 지우기
	if (tick % 1000 == 0) {
		gotoxy(N_ROW + 2, 0);
		printf("                                                                             ");
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

void line1_buf0() {	//버퍼 비우기
	for (int i = 0; i < n_player; i++) {
		PLAYER* p = &player[i];

		back_buf[px[i]][py[i]] = ' ';
	}

	/*back_buf[1][jul] = ' ';
	back_buf[1][jul + 1] = ' ';
	back_buf[1][jul - 1] = ' ';*/
	for (int i = 0; i < 10; i++) {
		back_buf[1][jul - 5 + i] = ' ';
	}
	juldarigi_line();
}

tick = 10;
void juldarigi(void) {
	jul = n_player * 5 / 2;
	center = n_player * 5 / 2;

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
			master_str = 0;	//마스터 조작 초기화
			line1_buf0();
			player_pos();

			display();
			
			
			if (lie_work[0] == true || lie_work[1] == true) {
				lie_after();
			}
			hole_die();
		}

		display();
		Sleep(10);
		tick += 10;

		juldarigi_dailog();

		if (winner_check() != ING) { 
			stamina_recov();
			break; 
		}
	}
}
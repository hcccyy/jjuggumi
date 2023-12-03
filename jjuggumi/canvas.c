#include "jjuggumi.h"
#include "canvas.h"



void draw(void);
void print_status(void);

// (zero-base) row행, col열로 커서 이동
void gotoxy(int row, int col) {
	COORD pos = { col,row };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// row행, col열에 ch 출력
void printxy(char ch, int row, int col) {
	gotoxy(row, col);
	printf("%c", ch);
}

void map_init(int n_row, int n_col) {
	// 두 버퍼를를 완전히 비우기
	for (int i = 0; i < ROW_MAX; i++) {
		for (int j = 0; j < COL_MAX; j++) {
			back_buf[i][j] = front_buf[i][j] = ' ';
		}
	}

	N_ROW = n_row;
	N_COL = n_col;
	for (int i = 0; i < N_ROW; i++) {
		// 대입문 이렇게 쓸 수 있는데 일부러 안 가르쳐줬음
		back_buf[i][0] = back_buf[i][N_COL - 1] = '*';

		for (int j = 1; j < N_COL - 1; j++) {
			back_buf[i][j] = (i == 0 || i == N_ROW - 1) ? '*' : ' ';
		}
	}
}

// back_buf[row][col]이 이동할 수 있는 자리인지 확인하는 함수
bool placable(int row, int col) {
	if (row < 0 || row >= N_ROW ||
		col < 0 || col >= N_COL ||
		back_buf[row][col] != ' ') {
		return false;
	}
	return true;
}

// 상단에 맵을, 하단에는 현재 상태를 출력
void display(void) {
	draw();

	gotoxy(N_ROW + 6, 0);  // 추가로 표시할 정보가 있으면 맵과 상태창 사이의 빈 공간에 출력
	print_status();
}

void draw(void) {
	for (int row = 0; row < N_ROW; row++) {
		for (int col = 0; col < N_COL; col++) {
			if (front_buf[row][col] != back_buf[row][col]) {
				front_buf[row][col] = back_buf[row][col];
				printxy(front_buf[row][col], row, col);
			}
		}
	}
}

void print_status(void) {
	printf("no. of players left: %d\n", n_alive);
	printf("			intl	str	stm\n");
	for (int i = 0; i < n_player; i++) {
		PLAYER* p = &player[i];
		printf("player %2d: %5s", i, p->is_alive ? "alive" : "DEAD");	

		printf("	%2d(%s%d)	%2d(%s%d)	%3d%%\n", p->intel, p->item.intel_buf >= 0 ? ("+") : (""), p->item.intel_buf, p->str, p->item.str_buf >= 0 ? ("+") :(""), p->item.str_buf, p->stamina);
	}
}

//void dialog(char message[]) {
//	int remaining_time = DIALOG_DURATION_SEC;
//	while (remaining_time >= 0) {
//		// 현재 맵과 상태를 백업
//		char backup[20][10];
//		for (int row = 0; row < N_ROW; row++) {
//			for (int col = 0; col < N_COL; col++) {
//				backup[row][col] = back_buf[row][col];
//			}
//		}
//
//		// 남은 시간과 메시지 출력
//		gotoxy(N_ROW / 2, N_COL / 2 - strlen(message) / 2);
//		printf("Remaining time: %d seconds", remaining_time);
//		gotoxy(N_ROW / 2 + 1, N_COL / 2 - strlen(message) / 2);
//		printf("%s", message);
//
//		// 1초 대기
//		Sleep(1000);
//		remaining_time--;
//
//		// 복구
//		for (int row = 0; row < N_ROW; row++) {
//			for (int col = 0; col < N_COL; col++) {
//				back_buf[row][col] = backup[row][col];
//			}
//		}
//		display();
//	}
//}

void dialog(char message[]) {
	int i, j;
	int messageLength = strlen(message);

	for (int i = DIALOG_DURATION_SEC; i > 0; i--) {
		gotoxy(4, 10);

		for (int j = 0; j < 12; j++) {
			printf("*");
		}
		printf("\n");

		gotoxy(5, 10);
		printf("*       %d %s        *\n", i, message);

		gotoxy(6, 10);
		for (int j = 0; j < 12; j++) {
			printf("*");
		}

		Sleep(1000);
	}


	for (j = 0; j < 3; j++) {
		for (i = 4; i <= 6; i++) {
			gotoxy(i, 10);
			printf("                   \n");
		}
	}
}

// 2023-2 고급프로그래밍 과제: 쭈꾸미 게임
#include "jjuggumi.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define	DATA_FILE	"jjuggumi.dat"

int jjuggumi_init(void);

// low 이상 high 이하 난수를 발생시키는 함수
int randint(int low, int high) {
	int rnum = rand() % (high - low + 1) + low;
	return rnum;
}

int jjuggumi_init(void) {
	srand((unsigned int)time(NULL));

	printf("플레이어 수: ");
	scanf_s("%d", &n_player);

	n_alive = n_player;
	for (int i = 0; i < n_player; i++) {
		player[i] = true;
	}
	return 0;
}

void intro(void) {
	// "GOOD LUCK"을 나타내는 ASCII 아트 출력
	printf("  ____  ___   ___  ____    _    _   _  ____ _  __\n");
	printf(" / ___|/ _ \\ / _ \\|  _ \\  | |  | | | |/ ___| |/ /\n");
	printf("| |  _| | | | | | | | | | | |  | | | | |   | ' /\n");
	printf("| |_| | |_| | |_| | |_| | | |__| |_| | |___| . \n");
	printf(" \\____|\\___/ \\___/|____/  |_____\\___/ \\____|_|\\_\\ \n");


	// 게임 시작을 위한 대기 시간 (5초)
	printf("\n로딩중");
	for (int i = 0; i < 5; i++) {
		printf("#########");
		Sleep(1000);
	}
	printf("\n");
	//Sleep(4800); //  (5초) 대기

	// 콘솔 화면을 지우는 명령 (Windows 환경)
	system("cls");
}

void ending(void) {
	int winner = -1; // 우승자의 인덱스, 초기에는 무승부(-1)로 설정

	for (int i = 0; i < n_player; i++) {
		if (player[i]) {
			if (winner == -1) {
				winner = i; // 처음으로 살아남은 플레이어를 우승자로 설정
			}
			else {
				winner = -1; // 다른 플레이어가 이미 살아남아 있으면 무승부로 설정
				break;
			}
		}
	}
	if (winner == -1) {
		gotoxy(5, 6);
		printf("우승자를 가리지 못했습니다!\n");
	}
	else {
		gotoxy(5, 6);
		printf("플레이어 %d가 이겼습니다!\n", winner);
	}

	for (int i = 0; i < 20; i++) {
		printf("\n");
	}
	// 게임 종료 로직 추가
	// 이후 게임을 종료하거나 다시 시작할 수 있도록 필요한 코드를 여기에 추가
}

int main(void) {

	jjuggumi_init();
	//intro();
	//sample();
	
	mugunghwa();
	//nightgame();
	//juldarigi();
	ending();
	//jebi();

	return 0;
}

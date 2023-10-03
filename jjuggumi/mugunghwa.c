#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // 각 플레이어 위치, 이동 주기

void mugungwha_init(void)
{
	int young_row = N_ROW / n_player;
	for (int i = 0; i < 3; i++)
	{
		printxy('#', young_row, N_COL + 2);
		young_row++;
	}
	Sleep(5000);
	map_init(n_player + 4, 60);
	//영희
	//영희 위치 = row/n_player ++;



	//canvas.c map_init에서 문자 #->*
	int x = 2, y = N_COL - 2;
	for (int i = 0; i < n_player; i++)
	{
		px[i] = x;
		py[i] = y;
		period[i] = randint(100, 500);

		back_buf[px[i]][py[i]] = '0' + i; 
		x++;
	}
}


void mugunghwa(void)
{
	mugungwha_init();
	system("cls");
	display();
	
}
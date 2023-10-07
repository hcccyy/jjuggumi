#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"

#define DIR_UP		0
#define DIR_DOWN	1
#define DIR_STAY	2
#define DIR_LEFT	3

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // �� �÷��̾� ��ġ, �̵� �ֱ�
int young_x, young_y;

bool pass_player[PLAYER_MAX] = { 0 };
bool next_game = false;

void mugungwha_init(void)
{
	map_init(n_player + 4, 60);
	young_x = n_player / 2 + 1.5; young_y = 1;
	//����
	for (int i = 0; i < 3; i++) {
		back_buf[young_x][young_y] = '#';
		young_x++;
	}
	

	//canvas.c map_init���� ���� #->*
	int x = 2, y = N_COL - 2;
	for (int i = 0; i < n_player; i++) {
		if (pass_player[i] == true) continue;
		px[i] = x;
		py[i] = y;
		period[i] = randint(100, 500);

		back_buf[px[i]][py[i]] = '0' + i; 
		x++;
	}
}

int get_rand()
{
	
	int random = rand();

	double r = random / (double)RAND_MAX;
	double dr = r * 100.0f;

	double p[4] = { 10.0f, 10.0f, 10.0f, 70.0f };

	double cumulative = 0.0f;

	for (int i = 0; i < 4; i++) {
		cumulative += p[i];
		if (dr <= cumulative) {
			return i;
		}
	}
}

void enemy_move_manual(int player) {
	int p = player;
	int nx, ny;  // �������� ������ ���� �ڸ�

	// �� �������� ������ �� x, y�� delta
	static int dx[4] = { -1, 1, 0, 0 };
	static int dy[4] = { 0, 0, 0, -1 };


	int dir;
	switch (get_rand()) {
	case 0: dir = DIR_UP; break;
	case 1: dir = DIR_DOWN; break;
	case 2: dir = DIR_STAY; break;
	case 3: dir = DIR_LEFT; break;
	default: return;
	}

	nx = px[p] + dx[dir];
	ny = py[p] + dy[dir];
	if (!placable(nx, ny)) return;
	
	move_tail(p, nx, ny);
}

void say_mugunghwa(void)
{
	char say[] = "����ȭ�����Ǿ����ϴ�";

	int skip = 0;

	for (int i = 0; say[i] != '\0'; i++) {
		putchar(say[i]);
		
		if (i == 11) {
			skip = 1;
			Sleep(600);
		}
		else if (skip == 1) {
			if (i % 2 == 0) continue;
			else Sleep(500 - (i / 2 - 6) * 100);
		}
		else {
			if (i % 2 == 0) continue;
			else Sleep(1000 + 200 * i);
		}

	}
	Sleep(3000);

	for (int i = 0; say[i] != '\0'; i++) {
		putchar('\b');
		putchar(' ');
		putchar('\b');
	}

	return say_mugunghwa();
}

void mugunghwa(void)	
{
	srand((unsigned int)time(NULL));
	next_game = false;
	mugungwha_init();
	
	system("cls");
	display();
	
	
	while (1) {

		//say_mugunghwa();	//�۵��ϸ� �÷��̾ �ȿ�����.

		// player 0�� ������ ������(4����)
		key_t key = get_key();
		if (key == K_QUIT) {
			break;
		}
		else if (key != K_UNDEFINED) {
			move_manual(key);
		}

		// Ȯ�� �̵�
		// �ʹ� ������ ������ ==0 ���� �ϤӤ�
		for (int i = 1; i < n_player; i++) {
			if (tick % period[i] < 10) {
				enemy_move_manual(i);
			}
		}

		//�÷��̾� ���, ���� ��������
		//0���� ����ϸ� ���� �������ϳ�?
		//? �Ѹ��� ����� �ϸ� �ȿ����̴µ�?
		young_x = n_player / 2 + 1.5; young_y = 1;
		for (int i = 0; i < n_player; i++) {
			if ((px[i] == young_x + 3 || px[i] == young_x - 1 ) && py[i] == young_y ||
				(py[i] == young_y + 1 && (px[i] == young_x || px[i] == young_x + 1 || px[i] == young_x + 2))) {
				pass_player[i] = true;
				next_game = true;
				Sleep(1500);
			}
		}
		if (next_game == true) return mugunghwa();

		display();
		Sleep(10);
		tick += 10;
	}


}
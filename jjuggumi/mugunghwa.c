#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"

#define DIR_UP		0
#define DIR_DOWN	1
#define DIR_RE		2
#define DIR_RIGHT	3

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // �� �÷��̾� ��ġ, �̵� �ֱ�


void mugungwha_init(void)
{
	map_init(n_player + 4, 60);
	//����
	int young_x = n_player / 2 + 1.5, young_y = 2;
	for (int i = 0; i < 3; i++) {
		back_buf[young_x][young_y] = '#';
		young_x++;
	}
	
	//canvas.c map_init���� ���� #->*
	int x = 2, y = N_COL - 2;
	for (int i = 0; i < n_player; i++) {
		px[i] = x;
		py[i] = y;
		period[i] = randint(100, 500);

		back_buf[px[i]][py[i]] = '0' + i; 
		x++;
	}
}

int get_rand()
{
	srand((unsigned int)time(NULL));
	int random = rand();

	double r = random / (double)RAND_MAX;
	double dr = r * 100.0f;

	double p[] = { 10.0f, 10.0f, 10.0f, 70.0f };

	double cumulative = 0.0f;

	for (int i = 0; i < 4; i++) {
		cumulative += p[i];
		if (dr <= cumulative) {
			return 3 - i;
		}
	}
}

void player_move_random(int player, int dir) {
	int p = player;  // �̸��� ��...
	int nx, ny;  // �������� ������ ���� �ڸ�

	// ������ ������ ���� ���� ���ٰ� ����(���� ������ ����)	

	// �� �������� ������ �� x, y�� delta
	static int e_dx[4] = { -1, 1, 0, 0 };
	static int e_dy[4] = { 0, 0, -1, 1 };

	//���� �ȿ����̸� �ȿ�����.
	//�� �����. ���߿� 14�� ���������� ����. 1�� ��ŸƮ ��ġ�� �̻���.
	int e_dir;
	switch (get_rand()) {
	case 0: e_dir = DIR_UP; break;
	case 1: e_dir = DIR_DOWN; break;
	case 2: e_dir = DIR_RE; break;
	case 3: e_dir = DIR_RIGHT; break;
	default: return;
	}

	nx = px[0] + e_dx[e_dir];
	ny = py[0] + e_dy[e_dir];
	if (!placable(nx, ny)) return;

	move_tail(p, nx, ny);
}

void mugunghwa(void)
{
	mugungwha_init();
	
	system("cls");
	display();
	
	while (1) {
		// player 0�� ������ ������(4����)
		key_t key = get_key();
		if (key == K_QUIT) {
			break;
		}
		else if (key != K_UNDEFINED) {
			move_manual(key);
		}

		// ��70% �װ�
		for (int i = 1; i < n_player; i++) {
			if (tick % period[i] == 0) {
				player_move_random(i, 1);
			}
		}

		display();
		Sleep(10);
		tick += 10;
	}
}
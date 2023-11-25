#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"

#define FIELD_SIZE_X 30 // 필드 가로 크기
#define FIELD_SIZE_Y 15 // 필드 세로 크기
#define NUM_ITEM     5

char field[FIELD_SIZE_Y][FIELD_SIZE_X];
int px[PLAYER_MAX], py[PLAYER_MAX]; // 플레이어 위치
int itemX[ITEM_MAX], itemY[ITEM_MAX]; // 아이템 위치
int userPlayer = 0; // 사용자 플레이어 번호

void nightgame_init() {
    map_init(FIELD_SIZE_Y, FIELD_SIZE_X);

    // 아이템 랜덤 배치
    srand((unsigned int)time(NULL));
    for (int i = 0; i < NUM_ITEM; ++i) {
        ITEM* tem = &item[i];
        // 아이템 배치 로직
        int x, y;
        do {
            x = randint(1, (FIELD_SIZE_Y - 2));
            y = randint(1, (FIELD_SIZE_X - 2));
        } while (back_buf[x][y] != ' ' || !placable(x, y));
        back_buf[x][y] = 'I'; // 아이템을 나타내는 문자
        itemX[i] = x;
        itemY[i] = y;
    }

    // 플레이어 배치 (0번 플레이어는 사용자로 설정)
    for (int i = 0; i < n_player; ++i) {
        PLAYER* p = &player[i];
        int x, y;

        if (p->is_alive == true) {
            do {
                x = randint(1, (FIELD_SIZE_Y - 2));
                y = randint(1, (FIELD_SIZE_X - 2));
            } while (back_buf[x][y] != ' ' || !placable(x, y));
            back_buf[x][y] = '0' + i; // 플레이어 번호 출력 (0~3)

            px[i] = x;
            py[i] = y;
        }

        if (i == 0) {
            userPlayer = 0; // 사용자 플레이어 번호 설정
        }
    }
}


void exchangeItem(int px, int py, int itemX, int itemY) {
    // 플레이어가 아이템 위치로 이동하여 교환하는 로직
    back_buf[px][py] = ' '; // 플레이어가 아이템을 가지게 됨
    back_buf[itemX][itemY] = '0'; // 아이템을 플레이어가 가지게 됨
}

int calculateDistance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

//플레이어 수, 아이템 수 조정
void playerItemInteraction(int PlayerNum) {
    // 아이템이 있는지 확인하는 플래그-> PLAYER 구조체에 있음

    // 아이템 번호 확인
    for (int i = 0; i < n_item; i++) {
        PLAYER* p = &player[PlayerNum];

        if ((px[PlayerNum] == itemX[i] && abs(py[PlayerNum] - itemY[i]) == 1) ||
            (py[PlayerNum] == itemY[i] && abs(px[PlayerNum] - itemX[i]) == 1)) {

            // player 아이템이 있을 때만 아이템 교환 또는 무시
            if (p->hasitem == true) {
                // 플레이어가 아이템을 가지고 있을 때 교환 여부 묻기
                if (PlayerNum == 0) {
                    printf("플레이어 0은 아이템을 교환하시겠습니까? (y/n)\n");
                    char input;
                    scanf_s("%c", &input, 1);

                    if (input == 'y' || input == 'Y') {
                        exchangeItem(px[0], py[0], itemX[i], itemY[i]);
                        printf("플레이어 0이 아이템을 교환합니다!\n");

                        back_buf[itemX[i]][itemY[i]] = ' ';
                        itemX[i] = 0;
                        itemY[i] = 0;
                    }
                    else {
                        printf("플레이어 0이 아이템을 무시합니다.\n");
                    }
                }
                else {
                    int random = rand() % 2;
                    if (random == 0) {
                        exchangeItem(px[PlayerNum], py[PlayerNum], itemX[i], itemY[i]);
                        printf("플레이어 %d가 아이템을 교환합니다!\n", PlayerNum);

                        back_buf[itemX[i]][itemY[i]] = ' ';
                        itemX[i] = 0;
                        itemY[i] = 0;
                    }
                    else {
                        printf("플레이어 %d가 아이템을 무시합니다.\n", PlayerNum);
                    }
                }
            }
            else {
                printf("플레이어 %d가 아이템을 획득했습니다!\n", PlayerNum);
                p->hasitem = true;
            }
        }
        //return p;
    }

    
}

// 플레이어들을 이동하는 함수
void movePlayers(int userPlayer,int count) {
    if (count % 5 == 0) {
        for (int i = 1; i < n_player; i++) {
            int minDistance = FIELD_SIZE_X + FIELD_SIZE_Y;
            int closestItemX, closestItemY;
            for (int j = 0; j < n_item; ++j) {
                int distance = calculateDistance(px[i], py[i], itemX[j], itemY[j]);
                if (distance < minDistance) {
                    minDistance = distance;
                    closestItemX = itemX[j];
                    closestItemY = itemY[j];
                }
            }

            int dx = closestItemX - px[i];
            int dy = closestItemY - py[i];

            // 가장 가까운 아이템 방향으로 이동
            if (abs(dx) > abs(dy)) {
                if (dx > 0 && back_buf[px[i] + 1][py[i]] == ' ') {
                    back_buf[px[i]][py[i]] = ' ';
                    px[i]++;
                    back_buf[px[i]][py[i]] = '0' + i;
                }
                else if (dx < 0 && back_buf[px[i] - 1][py[i]] == ' ') {
                    back_buf[px[i]][py[i]] = ' ';
                    px[i]--;
                    back_buf[px[i]][py[i]] = '0' + i;
                }
            }
            else {
                if (dy > 0 && back_buf[px[i]][py[i] + 1] == ' ') {
                    back_buf[px[i]][py[i]] = ' ';
                    py[i]++;
                    back_buf[px[i]][py[i]] = '0' + i;
                }
                else if (dy < 0 && back_buf[px[i]][py[i] - 1] == ' ') {
                    back_buf[px[i]][py[i]] = ' ';
                    py[i]--;
                    back_buf[px[i]][py[i]] = '0' + i;
                }
            }
        }
    }
}


void setCursorPosition(int x, int y) {
    COORD coord = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void nightgame() {
    n_item = n_player - 1;

    hideCursor(); // 커서 숨기기
    nightgame_init();
    
    system("cls");
    display();

    // 게임 루프
    int count = 0;
    while (1) {
        key_t key = get_key();
        if (key == K_QUIT) {
            break;
        }
        else if (key != K_UNDEFINED) {
            PLAYER* p0 = &player[0];
            if (p0->is_alive == false) continue;
            move_manual(key);
        }

        movePlayers(userPlayer, count);
        
        for (int i = 0; i < n_player; i++) {
            PLAYER* p = &player[i];

            playerItemInteraction(i);
        }

        //// 필드 갱신
        //for (int i = 1; i < FIELD_SIZE_Y - 1; ++i) {
        //    for (int j = 1; j < FIELD_SIZE_X - 1; ++j) {
        //        setCursorPosition(j, i);
        //        printf("%c", field[i][j]);
        //    }
        //}
        
        count++;
        tick += 100;
        Sleep(100);

        display();

    }
    
}

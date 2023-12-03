#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"

#define FIELD_SIZE_X 30 // 필드 가로 크기
#define FIELD_SIZE_Y 14 // 필드 세로 크기



char field[FIELD_SIZE_Y][FIELD_SIZE_X];
int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX]; // 플레이어 위치
int itemX[ITEM_MAX], itemY[ITEM_MAX]; // 아이템 위치

bool ignoreitem[PLAYER_MAX][ITEM_MAX] = { 0 };
bool ignoreplayer[PLAYER_MAX][PLAYER_MAX] = { 0 };

//int userPlayer = 0; // 사용자 플레이어 번호


void nightgame_init() {
    map_init(FIELD_SIZE_Y, FIELD_SIZE_X);

    // 아이템 랜덤 배치
    srand((unsigned int)time(NULL));
    for (int i = 0; i < n_item; ++i) {
        ITEM* tem = &item[i];

        int x, y;
        do {
            x = randint(1, (FIELD_SIZE_Y - 2));
            y = randint(1, (FIELD_SIZE_X - 2));
        } while (back_buf[x][y] != ' ' || !placable(x, y));
        back_buf[x][y] = 'I'; // 아이템을 나타내는 문자
        itemX[i] = x;
        itemY[i] = y;
    }

    // 플레이어 배치, 주기
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
            period[i] = randint(100, 500);
        }
    }
}


void getItem(PLAYER* p, ITEM* tem) {
    strncpy_s(p->item.name, sizeof(p->item.name), tem->name, sizeof(tem->name));
    p->item.intel_buf = tem->intel_buf;
    p->item.str_buf = tem->str_buf;
    p->item.stamina_buf = tem->stamina_buf;
}

// 다일로그 지우기
void remove_itemDialog() {
    if (tick % 1000 == 0) {
        gotoxy(15, 0);
        printf("                                                                       ");
        gotoxy(16, 0);
        printf("                                                                            ");
    }
}

void exchangeItem(PLAYER* p, ITEM* tem, int new_itemX, int new_itemY) {
    for (int j = 0; j < n_item; j++) {
        ITEM* tem = &item[j];

        if (strcmp(tem->name, p->item.name) == 0) {
            //갖고있던 템 버리기
            itemX[j] = new_itemX;
            itemY[j] = new_itemY;

            //버린 템 무시
            ignoreitem[p->id][j] = true;
            break;
        }
    }
    printf("플레이어 %d가 아이템(%s)를 ", p->id, p->item.name);

    //새로운 템 먹기
    getItem(p, tem);

    for (int j = 0; j < n_item; j++) {
        ITEM* tem = &item[j];

        if (strcmp(tem->name, p->item.name) == 0) {
            // 새로운 템 위치 초기화
            itemX[j] = 0;
            itemY[j] = 0;

            break;
        }
    }
    printf("아이템(%s)로 교환합니다!\n", p->item.name);
}

int calculateDistance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}


void playerItemInteraction(int PlayerNum) {
    PLAYER* p = &player[PlayerNum];

    // 아이템 번호 확인
    for (int i = 0; i < n_item; i++) {
        ITEM* tem = &item[i];

        if ((px[p->id] == itemX[i] && abs(py[p->id] - itemY[i]) == 1) ||
            (py[p->id] == itemY[i] && abs(px[p->id] - itemX[i]) == 1)) {

            if (ignoreitem[PlayerNum][i] == true) continue;

            // player 아이템이 있을 때만 아이템 교환 또는 무시
            if (p->hasitem == true) {
                // 플레이어가 아이템을 가지고 있을 때 교환 여부 묻기
                if (p->id == 0) {
                    gotoxy(15, 0);
                    printf("플레이어 0은 아이템을 교환하시겠습니까? (y/n)>> ");
                    char input;
                    scanf_s("%c", &input, 1);
                    getchar();

                    Sleep(500);
                    gotoxy(15, 0);
                    printf("                                                                       ");
                    gotoxy(16, 0);
                    printf("          ");

                    if (input == 'y' || input == 'Y') {
                        gotoxy(16, 0);
                        exchangeItem(p, tem, itemX[i], itemY[i]);

                        tick = 10;
                    }
                    else {
                        gotoxy(15, 0);
                        printf("플레이어 0이 아이템(%s)을 무시합니다.\n", tem->name);
                        tick = 10;

                        ignoreitem[p->id][i] = true;
                    }
                }
                else {
                    int random = rand() % 2;
                    if (random == 0) {
                        //ignoreitem 초기화
                        for (int k = 0; k < n_item; k++) ignoreitem[p->id][k] = false;

                        gotoxy(16, 0);
                        exchangeItem(p, tem, itemX[i], itemY[i]);
                        tick = 10;

                    }
                    else {
                        gotoxy(15, 0);
                        printf("플레이어 %d가 아이템(%s)을 무시합니다.\n", p->id, tem->name);
                        tick = 10;

                        ignoreitem[p->id][i] = true;

                    }
                }
            }
            else {
                p->hasitem = true;
                getItem(p, tem);

                gotoxy(15, 0);
                printf("플레이어 %d가 아이템(%s)을 획득했습니다!\n", PlayerNum, p->item.name);
                tick = 10;

                back_buf[itemX[i]][itemY[i]] = ' ';
                itemX[i] = 0;
                itemY[i] = 0;
            }
        }
    }
}

// 플레이어들을 이동하는 함수
// 맵에 템이 없으면 0,0 으로 감
void movePlayers(int PlayerNum) {
    int i = PlayerNum;
    int minDistance = 100;
    int closestItemX = 0, closestItemY = 0;

    for (int j = 0; j < n_item; ++j) {
        if (ignoreitem[PlayerNum][j] == true) continue;
        if (itemX[j] == 0 && itemY[j] == 0) continue;

        int distance = calculateDistance(px[PlayerNum], py[PlayerNum], itemX[j], itemY[j]);
        if (distance < minDistance) {
            minDistance = distance;
            closestItemX = itemX[j];
            closestItemY = itemY[j];
        }
    }

    int dx = closestItemX - px[PlayerNum];
    int dy = closestItemY - py[PlayerNum];

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


void clearItem(PLAYER* p) {
    //memset(p->item.name, 0, sizeof(p->item.name));
    strncpy_s(p->item.name, sizeof(p->item.name), "", 0);
    p->item.intel_buf = 0;
    p->item.stamina_buf = 0;
    p->item.str_buf = 0;
    p->hasitem = false;
}

void remove_robberyDialog() {
    if (tick % 1000 == 0) {
        for (int i = 0; i < 3; i++) {
            gotoxy(17 + i, 0);
            printf("                                                                            ");
        }
    }

    if (tick % 1000 == 0) {
        for (int i = 0; i < 10; i++) {
            gotoxy(3 + i, 32);
            printf("                                                                      ");
        }
    }
}

void robberyAttempt(int player1, int player2) {
    PLAYER* p1 = &player[player1];
    PLAYER* p2 = &player[player2];
    ITEM* i1 = &p1->item;
    ITEM* i2 = &p2->item;

    int powerPlayer1 = p1->str * (p1->stamina * 0.01);
    int powerPlayer2 = p2->str * (p2->stamina * 0.01);

    //gotoxy(19, 0);
    // 강탈 조건 확인
    if (powerPlayer1 > powerPlayer2) {
        if (p1->id == 0) gotoxy(9, 32);
        printf("강탈 시도 성공!\n");

        // 아이템 강탈 또는 교환
        if (p1->hasitem && p2->hasitem) {
            if (p1->id == 0) gotoxy(10, 32);
            printf("아이템을 교환합니다.\n");

            ITEM temp = p1->item;
            p1->item = p2->item;
            p2->item = temp;
            //exchangeItem2(p1, p2);
        }
        else if (p1->hasitem == false && p2->hasitem == true) {
            if (p1->id == 0) gotoxy(10, 32);
            printf("아이템(%s)을 강탈합니다.\n", i2->name);

            getItem(p1, i2);
            clearItem(p2);

        }
        else {
            if (p1->id == 0) gotoxy(10, 32);
            printf("강탈할 것이 없습니다...\n");
        }
        if (p1->id == 0) gotoxy(11, 32);
        printf("스태미나를 40%% 소모했습니다.\n");
        p1->stamina -= (p1->stamina * 0.4);
    }
    else {
        if (p1->id == 0) gotoxy(9, 32);
        printf("강탈 시도 실패!\n");
        if (p1->id == 0) gotoxy(10, 32);
        printf("스태미나를 60%% 소모했습니다.\n");
        p1->stamina -= (p1->stamina * 0.6);
    }
    // 스태미나가 0 이하인 경우 0으로 설정
    if (p1->stamina < 0) {
        p1->stamina = 0;
    }

    ignoreplayer[p1->id][p2->id] = true;
    ignoreplayer[p2->id][p1->id] = true;
}


// 회유 시도
void persuasionAttempt(int player1, int player2) {
    PLAYER* p1 = &player[player1];
    PLAYER* p2 = &player[player2];

    ITEM* i1 = &p1->item;
    ITEM* i2 = &p2->item;
     
    int intellPlayer1 = p1->intel * (p1->stamina * 0.01);
    int intellPlayer2 = p2->intel * (p2->stamina * 0.01);

    // 회유 조건 확인
    if (intellPlayer1 > intellPlayer2) {
        if (p1->id == 0) gotoxy(9, 32);
        printf("회유 시도 성공!\n");

        // 아이템 강탈 또는 교환
        if (p1->hasitem && p2->hasitem) {
            if (p1->id == 0) gotoxy(10, 32);
            printf("아이템을 교환합니다.\n");

            ITEM temp = p1->item;
            p1->item = p2->item;
            p2->item = temp;
            //exchangeItem2(p1, p2);
        }
        else if (p1->hasitem == false && p2->hasitem == true) {
            if (p1->id == 0) gotoxy(10, 32);
            printf("아이템(%s)을 강탈합니다.\n", i2->name);

            getItem(p1, i2);
            clearItem(p2);

        }

        else {
            if (p1->id == 0) gotoxy(10, 32);
            printf("강탈할 것이 없습니다...\n");
        }
        if (p1->id == 0) gotoxy(11, 32);
        printf("스태미나를 20%% 소모했습니다.\n");
        p1->stamina -= (p1->stamina * 0.2);
    }
    else {
        if (p1->id == 0) gotoxy(9, 32);
        printf("회유 시도 실패!\n");
        if (p1->id == 0) gotoxy(10, 32);
        printf("스태미나를 40%% 소모했습니다.\n");
        p1->stamina -= (p1->stamina * 0.4);
    }
    // 스태미나가 0 이하인 경우 0으로 설정
    if (p1->stamina < 0) {
        p1->stamina = 0;
    }

    ignoreplayer[p1->id][p2->id] = true;
    ignoreplayer[p2->id][p1->id] = true;
}

// 상호작용
void interaction() {
    for (int i = 0; i < n_player; i++) {
        for (int j = i + 1; j < n_player; j++) {
            if (calculateDistance(px[i], py[i], px[j], py[j]) == 1) {
                if (ignoreplayer[i][j] == true || ignoreplayer[j][i] == true) continue;

                if (i == 0) {
                    for (int i = 0; i < 10; i++) {
                        gotoxy(3 + i, 32);
                        printf("                                                                      ");
                    }

                    gotoxy(3, 32);
                    printf("플레이어 0과 플레이어 %d가 만났습니다.\n", j);
                    gotoxy(4, 32);
                    printf("1) 강탈 시도\n");
                    gotoxy(5, 32);
                    printf("2) 회유 시도\n");
                    gotoxy(6, 32);
                    printf("3) 무시\n");
                    gotoxy(7, 32);
                    printf("어떤 행동을 하시겠습니까? >> ");

                    int choice;
                    scanf_s("%d", &choice);
                    getchar(); // 개행문자 처리

                    switch (choice) {
                    case 1:
                        robberyAttempt(i, j);

                        break;
                    case 2:
                        persuasionAttempt(i, j);
                        break;
                    case 3:
                        gotoxy(8, 32);
                        printf("플레이어 0이 상대를 무시합니다.\n");
                        ignoreplayer[i][j] = true;
                        ignoreplayer[j][i] = true;
                        break;
                    default:
                        gotoxy(8, 32);
                        printf("잘못된 선택입니다. 행동을 취하지 않습니다.\n");
                        break;
                    }
                }
                else {
                    gotoxy(17, 0);
                    // 다른 플레이어끼리의 상호작용

                    printf("플레이어 %d과 플레이어 %d가 만났습니다.\n", i, j);
                    Sleep(1000);

                    int randomPlayer = rand() % 2; // 무작위 플레이어 선택
                    int actionChoice = rand() % 3 + 1; // 1: 강탈 시도, 2: 회유 시도, 3: 무시

                    switch (actionChoice) {
                    case 1:
                        if (player[i].hasitem || player[j].hasitem) {
                            if (!player[i].hasitem) {
                                printf("플레이어 %d이(가) 플레이어 %d에게 강탈 시도합니다.", i, j);
                                Sleep(1000);
                                robberyAttempt(i, j);
                            }
                            else if (!player[j].hasitem) {
                                printf("플레이어 %d이(가) 플레이어 %d에게 강탈 시도합니다.", j, i);
                                Sleep(1000);
                                robberyAttempt(j, i);
                            }
                            else {
                                int randomChoice = rand() % 2;
                                if (randomChoice == 0) {
                                    printf("플레이어 %d이(가) 플레이어 %d에게 강탈 시도합니다.", i, j);
                                    Sleep(1000);
                                    robberyAttempt(i, j);
                                }
                                else {
                                    printf("플레이어 %d이(가) 플레이어 %d에게 강탈 시도합니다.", j, i);
                                    Sleep(1000);
                                    robberyAttempt(j, i);
                                }
                            }
                        }
                        else {
                            int randomChoice = rand() % 2;
                            if (randomChoice == 0) {
                                printf("플레이어 %d이(가) 플레이어 %d에게 강탈 시도합니다.\n", i, j);
                                robberyAttempt(i, j);
                            }
                            else {
                                printf("플레이어 %d이(가) 플레이어 %d에게 강탈 시도합니다.\n", j, i);
                                robberyAttempt(j, i);
                            }
                        }
                        break;
                    case 2:
                        if (player[i].hasitem || player[j].hasitem) {
                            if (!player[i].hasitem) {
                                printf("플레이어 %d이(가) 플레이어 %d에게 회유 시도합니다.", i, j);
                                Sleep(1000);
                                persuasionAttempt(i, j);
                            }
                            else if (!player[j].hasitem) {
                                printf("플레이어 %d이(가) 플레이어 %d에게 회유 시도합니다.", j, i);
                                Sleep(1000);
                                persuasionAttempt(j, i);
                            }
                            else {
                                int randomChoice = rand() % 2;
                                if (randomChoice == 0) {
                                    printf("플레이어 %d이(가) 플레이어 %d에게 회유 시도합니다.", i, j);
                                    Sleep(1000);
                                    persuasionAttempt(i, j);
                                }
                                else {
                                    printf("플레이어 %d이(가) 플레이어 %d에게 회유 시도합니다.", j, i);
                                    Sleep(1000);
                                    persuasionAttempt(j, i);
                                }
                            }
                        }
                        else {
                            int randomChoice = rand() % 2;
                            if (randomChoice == 0) {
                                printf("플레이어 %d이(가) 플레이어 %d에게 회유 시도합니다.\n", i, j);
                                persuasionAttempt(i, j);
                            }
                            else {
                                printf("플레이어 %d이(가) 플레이어 %d에게 회유 시도합니다.\n", j, i);
                                persuasionAttempt(j, i);
                            }
                        }
                        break;
                    case 3:
                        if (randomPlayer == 0) {
                            printf("플레이어 %d이(가) 상대를 무시합니다.\n", i);
                            ignoreplayer[i][j] = true;
                            ignoreplayer[j][i] = true;
                        }
                        else {
                            printf("플레이어 %d이(가) 상대를 무시합니다.\n", j);
                            ignoreplayer[j][i] = true;
                            ignoreplayer[i][j] = true;
                        }
                        break;
                    default:
                        printf("잘못된 선택입니다. 행동을 취하지 않습니다.\n");
                        break;
                    }
                }
            }
        }
    }
}


void restoreStamina() {
    // 게임 종료 시 모든 플레이어의 스태미나 40% 회복
    for (int i = 0; i < n_player; i++) {
        player[i].stamina += (100 - player[i].stamina) * 0.4; // 현재 스태미나의 40% 회복
        // 최대 스태미너를 넘지 않도록 제한
        if (player[i].stamina > 100) {
            player[i].stamina = 100;
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
    n_item = 7;

    hideCursor(); // 커서 숨기기
    nightgame_init();

    system("cls");
    display();

    // 게임 루프
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


        for (int i = 0; i < n_player; i++) {
            PLAYER* p = &player[i];

            if (tick % period[i] < 10) {

                if (i == 0) continue;
                movePlayers(i);
            }
        }


        display();
        tick += 10;
        Sleep(10);

        for (int i = 0; i < n_player; i++) {
            PLAYER* p = &player[i];

            playerItemInteraction(i);
        }
        interaction();
        remove_itemDialog();
        remove_robberyDialog();
       
    }
    restoreStamina();
}
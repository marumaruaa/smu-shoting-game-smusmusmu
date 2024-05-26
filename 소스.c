#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAP_X_MAX 80
#define MAP_Y_MAX 40
#define MAX_VALUE MAP_X_MAX * MAP_Y_MAX
#define MAX_BULLET 10
#define _CRT_SECURE_NO_WARNINGS

int b_index = 0;// �Ѿ˿� ���� �ε���
int i_index = 0;// ���� ���� �ε���
int eb_index = 0;// �� �Ѿ˿� ���� �ε���
int r_index = 0;// �������� ���� �ε���
int gameStatus = 0;// ���� ���� (��: ���� ��, �Ͻ� ����, ���� ��)// �������� ���� �ε���
int score = 0; // �÷��̾��� ����

char mapData[MAX_VALUE];

typedef struct Character {
    int x, y, size_x, size_y;
    char sprite[6];
} plane;

plane myPlane;

typedef struct bullet {
    int x, y;
    bool active;
} Bullet;

Bullet Bullet_info[MAX_BULLET];
Bullet Enemy_Bullet[MAX_BULLET * 10000];

typedef struct enemy {
    int x, y;
    bool active;
    Bullet Enemy_Bullet;
} Enemy;

Enemy Enemy_info[11];

void SetConsole();
void textcolor(int color_number);
void gotoxy(int x, int y);
void FillMap(char str[], char str_s, int max_value);
void EditMap(int x, int y, char str);
void DrawSprite(int x, int y, int size_x, int size_y, char spr[]);
void controlCharacter(plane* pPlane);
void gameOver();
void gameStart();
void gameClear();
void gameReset();
void BulletShoot();
void EnemyCreate();
void EnemyMove();
void EnemyAttack();
void ShowOption();
void showCredits();

int main(void) {
    srand(time(NULL));
    SetConsole();
    myPlane.x = MAP_X_MAX / 2 + 1;
    myPlane.y = MAP_Y_MAX / 3 * 2 + 1;
    myPlane.size_x = 3;
    myPlane.size_y = 2;
    strncpy(myPlane.sprite, "_^_^^^", sizeof(myPlane.sprite));
    int count = 0;

    while (1) {
        while (gameStatus == 0) {
            gameStart();
            printf("%s", mapData);
        }
        while (gameStatus == 1) {
            textcolor(7);
            FillMap(mapData, ' ', MAX_VALUE);
            controlCharacter(&myPlane);
            DrawSprite(myPlane.x, myPlane.y, myPlane.size_x, myPlane.size_y, myPlane.sprite);
            BulletShoot(&Bullet_info, &myPlane, &Enemy_info);
            EnemyCreate(&Enemy_info, count);
            EnemyMove(&Enemy_info);
            EnemyAttack(&Enemy_info, &Enemy_Bullet, &myPlane);
            ShowOption();
            printf("%s", mapData);
            Sleep(50);
            count++;
            score++;
        }
        if (gameStatus == 2) {
            FillMap(mapData, ' ', MAX_VALUE);
            printf("%s", mapData);
            while (gameStatus == 2) {
                gameOver();
                printf("%s", mapData);
                count = 0;
            }
        }
        while (gameStatus == 3) {
            gameClear();
            printf("%s", mapData);
            exit(1);
        }
        while (gameStatus == 4) {
            textcolor(6);
            showCredits();
            exit(1);
        }
    }
    return 0;
}

void SetConsole() {
    system("SMU shooting Game");
    system("mode con:cols=80 lines=40");
    CONSOLE_CURSOR_INFO ConsoleCursor;
    ConsoleCursor.bVisible = 0;
    ConsoleCursor.dwSize = 1;
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorInfo(consoleHandle, &ConsoleCursor);
}

void textcolor(int color_number) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color_number);
}

void gotoxy(int x, int y) {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD Cur;
    Cur.X = x;
    Cur.Y = y;
    SetConsoleCursorPosition(consoleHandle, Cur);
}

void FillMap(char str[], char str_s, int max_value) {
    for (int i = 0; i < max_value; i++) {
        str[i] = str_s;
    }
}

void EditMap(int x, int y, char str) {
    if (x > 0 && y > 0 && x - 1 < MAP_X_MAX - 1 && y - 1 < MAP_Y_MAX - 1)
        mapData[(y - 1) * MAP_X_MAX + x - 1] = str;
}

void DrawSprite(int x, int y, int size_x, int size_y, char spr[]) {
    for (int i = 0; i < size_y; i++) {
        for (int j = 0; j < size_x; j++) {
            EditMap(x + j, y + i, spr[i * size_x + j]);
        }
    }
}

void controlCharacter(plane* pPlane) {
    if (GetAsyncKeyState(VK_UP) & 0x8000) {
        if (pPlane->y > 1)
            pPlane->y--;
    }
    if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
        if (pPlane->y < MAP_Y_MAX - 5)
            pPlane->y++;
    }
    if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        if (pPlane->x > 1)
            pPlane->x -= 2;
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        if (pPlane->x < MAP_X_MAX - 3)
            pPlane->x += 2;
    }
}

void gameStart() {
    FillMap(mapData, ' ', MAX_VALUE);
    textcolor(11);
    DrawSprite(25, 4, 30, 7, "����������������������������������������������������������������������������������������������������������");
    DrawSprite(36, 16, 11, 1, "1. ���ӽ���");
    DrawSprite(36, 21, 7, 1, "2. ����");
    DrawSprite(36, 26, 7, 1, "3. ����");
    if (_getch() == '1') {
        gameStatus = 1;
    }
    else if (_getch() == '2') {
        gameStatus = 4;
    }
    else if (_getch() == '3') {
        textcolor(11);
        exit(1);
    }
}

void gameOver() {
    FillMap(mapData, ' ', MAX_VALUE);
    textcolor(12);
    DrawSprite(35, 5, 12, 1, "GAME OVER !!");
    DrawSprite(36, 16, 9, 1, "1. �絵��");
    DrawSprite(36, 21, 7, 1, "2. ����");
    DrawSprite(36, 26, 7, 1, "3. ����");
    if (_getch() == '1') {
        gameReset(&Enemy_Bullet, &Enemy_info, &Bullet_info, &myPlane);
        gameStatus = 1;
    }
    else if (_getch() == '2') {
        gameStatus = 4;
    }
    else if (_getch() == '3') {
        textcolor(11);
        exit(1);
    }
}

void gameClear() {
    FillMap(mapData, ' ', MAX_VALUE);
    textcolor(6);
    DrawSprite(24, 10, 34, 19, "������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������");
    DrawSprite(36, 35, 9, 1, "VICTORY!!");
}

void BulletShoot(Bullet* pBullet, plane* pPlane, Bullet* Enemy) {
    for (int i = 0; i < MAX_BULLET; i++) {
        if (pBullet[i].active) {
            pBullet[i].y--;
            mapData[pBullet[i].y * MAP_X_MAX + pBullet[i].x] = '*';

            if (pBullet[i].y < 1) {
                pBullet[i].active = false;
            }

            for (int j = 0; j < 10; j++) {
                if (Enemy[j].active && Enemy[j].x == pBullet[i].x && Enemy[j].y == pBullet[i].y) {
                    Enemy[j].active = false;
                    pBullet[i].active = false;
                    score += 1000;
                }
            }
        }
    }
    int a = pPlane->x;
    int b = pPlane->y;

    if (GetAsyncKeyState(VK_SPACE)) {
        if (b_index >= 9) b_index = 0;
        pBullet[b_index].x = a;
        pBullet[b_index].y = b;
        pBullet[b_index].active = true;
        b_index++;
    }

    if (GetAsyncKeyState(VK_CONTROL)) {
        for (int i = 0; i < 10; i++) {
            pBullet[b_index].active = FALSE;
        }
        b_index = 0;
    }
}

void EnemyCreate(Enemy* Enemy, int count) {
    int a = rand() % 50 + 10;
    int b = rand() % 7 + 5;
    if (count % 200 == 0) {
        Enemy[i_index].x = a;
        Enemy[i_index].y = b;
        Enemy[i_index].active = TRUE;
        i_index++;
        if (Enemy[10].active == TRUE) {
            return gameStatus = 3;
        }
    }
    for (int i = 0; i < 11; i++) {
        if (Enemy[i].active)
            mapData[Enemy[i].y * MAP_X_MAX + Enemy[i].x - 1] = '@';
    }
}

void EnemyMove(Enemy* Enemy) {
    for (int i = 0; i < 11; i++) {
        if (Enemy[i].active == TRUE) {
            if (Enemy[i].x < 2) Enemy[i].x++;
            else if (Enemy[i].x > 78) Enemy[i].x--;
            else if (Enemy[i].y < 2) Enemy[i].y++;
            else if (Enemy[i].y > 17) Enemy[i].y--;
            else {
                int move = rand() % 4;
                if (move == 0) {
                    Enemy[i].x++;
                }
                else if (move == 1) {
                    Enemy[i].x--;
                }
                else if (move == 2) {
                    Enemy[i].y++;
                }
                else if (move == 3) {
                    Enemy[i].y--;
                }
            }
        }
    }
}

void EnemyAttack(Enemy* enemy, Bullet* Enemy_Bullet, plane* Plane) {
    for (int i = 0; i < MAX_BULLET * 100; i++) {
        if (Enemy_Bullet[i].active) {
            Enemy_Bullet[i].y++;
            mapData[Enemy_Bullet[i].y * MAP_X_MAX + Enemy_Bullet[i].x - 1] = '!';

            if (Enemy_Bullet[i].y >= 35) {
                Enemy_Bullet[i].active = false;
            }

            if (Plane->x == Enemy_Bullet[i].x && Plane->y == Enemy_Bullet[i].y) {
                gameStatus = 2;
            }
            if (Plane->x + 1 == Enemy_Bullet[i].x && Plane->y == Enemy_Bullet[i].y) {
                gameStatus = 2;
            }
            if (Plane->x + 2 == Enemy_Bullet[i].x && Plane->y == Enemy_Bullet[i].y) {
                gameStatus = 2;
            }
        }
    }

    if (rand() % 10 != 0) return;
    if (eb_index == MAX_BULLET * 100 - 1) eb_index = 0;
    for (int i = 0; i < MAX_BULLET; i++) {
        if (enemy[i].active) {
            Enemy_Bullet[eb_index].x = enemy[i].x;
            Enemy_Bullet[eb_index].y = enemy[i].y + 1;
            Enemy_Bullet[eb_index].active = true;
            eb_index++;
        }
    }
}

void gameReset(Bullet* Enemy_Bullet, Enemy* Enemy, Bullet* pBullet, plane* pPlane) {
    i_index = 0;
    b_index = 0;
    eb_index = 0;
    score = 0;
    pPlane->x = MAP_X_MAX / 2 + 1;
    pPlane->y = MAP_Y_MAX / 3 * 2 + 1;
    for (int i = 0; i < 10; i++) {
        Enemy[i].active = FALSE;
        pBullet[i].active = FALSE;
        for (int j = 0; j < 1000; j++) {
            Enemy_Bullet[j * 10 + i].active = FALSE;
        }
    }
}

void ShowOption() {
    int score_num = score;
    DrawSprite(1, 39, 7, 1, "�Ѿ� : ");
    for (int i = 0; i <= 10; i++) {
        if (b_index - 1 >= i)
            EditMap(10 + i, 39, 'X');
        else
            EditMap(10 + i, 39, 'O');
    }

    DrawSprite(60, 37, 18, 1, "���� : ��,��,��,��");
    DrawSprite(60, 38, 12, 1, "���� : Space");
    DrawSprite(60, 39, 13, 1, "������ : Ctrl");
    DrawSprite(30, 1, 8, 1, "SCORE : ");
    for (int j = 0; j <= 10; j++) {
        char str = score_num % 10 + 48;
        EditMap(48 - j, 1, str);
        score_num = score_num / 10;
    }
}

void showCredits() {
    system("mode con: cols=80 lines=40"); // �ܼ� â ũ�� ����
    system("title ������"); // �ܼ� â ���� ����

    FillMap(mapData, ' ', MAX_VALUE);
    textcolor(10);

    // ������ ���� ���
    printf("������:\n");
    printf("������\n");
    printf("������\n");
    printf("������\n");
    printf("���ÿ�\n");
    printf("�̻��\n");

    printf("\n�ƹ���ư�� ������ ���� ������ �������ϴ�\n");
    _getch(); // ����� �Է� ���

    FillMap(mapData, ' ', MAX_VALUE);
}






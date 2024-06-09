#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAP_X_MAX 80 // 맵의 가로 크기
#define MAP_Y_MAX 40 // 맵의 세로 크기
#define MAX_VALUE MAP_X_MAX * MAP_Y_MAX // 맵의 전체 크기
#define MAX_BULLET 10 // 최대 총알 개수
#define _CRT_SECURE_NO_WARNINGS

// 게임의 상태를 나타내는 변수들
int b_index = 0; // 플레이어 총알 인덱스
int i_index = 0; // 적 인덱스
int eb_index = 0; // 적 총알 인덱스
int r_index = 0; // 렌더링 인덱스
int gameStatus = 0; // 게임 상태 (0: 시작 화면, 1: 게임 진행 중, 2: 게임 오버, 3: 게임 클리어, 4: 제작진 화면)
int score = 0; // 플레이어의 점수

char mapData[MAX_VALUE]; // 맵 데이터를 저장하는 배열

// 캐릭터 구조체 정의
typedef struct Character {
    int x, y, size_x, size_y; // 위치와 크기
    char sprite[6]; // 캐릭터 스프라이트 (모양)
} plane;

plane myPlane; // 플레이어 비행기

// 총알 구조체 정의
typedef struct bullet {
    int x, y; // 위치
    bool active; // 활성화 여부
} Bullet;

Bullet Bullet_info[MAX_BULLET]; // 플레이어 총알 정보 배열
Bullet Enemy_Bullet[MAX_BULLET * 10000]; // 적 총알 정보 배열

// 적 구조체 정의
typedef struct enemy {
    int x, y; // 위치
    bool active; // 활성화 여부
    Bullet Enemy_Bullet; // 적의 총알
} Enemy;

Enemy Enemy_info[11]; // 적 정보 배열

// 함수 선언
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

// 메인 함수
int main(void) {
    srand(time(NULL)); // 랜덤 시드 초기화
    SetConsole(); // 콘솔 설정
    myPlane.x = MAP_X_MAX / 2 + 1; // 플레이어 비행기 초기 위치 설정
    myPlane.y = MAP_Y_MAX / 3 * 2 + 1; // 플레이어 비행기 초기 위치 설정
    myPlane.size_x = 3; // 플레이어 비행기 크기 설정
    myPlane.size_y = 2; // 플레이어 비행기 크기 설정
    strncpy(myPlane.sprite, "_^_^^^", sizeof(myPlane.sprite)); // 플레이어 비행기 모양 설정
    int count = 0; // 적 생성 주기 카운트

    while (1) {
        while (gameStatus == 0) { // 게임 시작 화면
            gameStart();
            printf("%s", mapData);
        }
        while (gameStatus == 1) { // 게임 진행 중
            textcolor(7); // 텍스트 색상 설정
            FillMap(mapData, ' ', MAX_VALUE); // 맵 초기화
            controlCharacter(&myPlane); // 플레이어 캐릭터 조작
            DrawSprite(myPlane.x, myPlane.y, myPlane.size_x, myPlane.size_y, myPlane.sprite); // 플레이어 비행기 그리기
            BulletShoot(&Bullet_info, &myPlane, &Enemy_info); // 플레이어 총알 발사
            EnemyCreate(&Enemy_info, count); // 적 생성
            EnemyMove(&Enemy_info); // 적 이동
            EnemyAttack(&Enemy_info, &Enemy_Bullet, &myPlane); // 적 공격
            ShowOption(); // 옵션 표시
            printf("%s", mapData); // 맵 출력
            Sleep(50); // 게임 루프 딜레이
            count++; // 카운트 증가
            score++; // 점수 증가
        }
        if (gameStatus == 2) { // 게임 오버 화면
            FillMap(mapData, ' ', MAX_VALUE); // 맵 초기화
            printf("%s", mapData);
            while (gameStatus == 2) {
                gameOver(); // 게임 오버 처리
                printf("%s", mapData);
                count = 0;
            }
        }
        while (gameStatus == 3) { // 게임 클리어 화면
            gameClear(); // 게임 클리어 처리
            printf("%s", mapData);
            exit(1); // 게임 종료
        }
        while (gameStatus == 4) { // 제작진 화면
            textcolor(6); // 텍스트 색상 설정
            showCredits(); // 제작진 정보 표시
            exit(1); // 게임 종료
        }
    }
    return 0;
}

// 콘솔 설정 함수
void SetConsole() {
    system("SMU shooting Game");
    system("mode con:cols=80 lines=40"); // 콘솔 크기 설정
    CONSOLE_CURSOR_INFO ConsoleCursor; // 커서 정보 구조체
    ConsoleCursor.bVisible = 0; // 커서 숨김
    ConsoleCursor.dwSize = 1; // 커서 크기 설정
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE); // 콘솔 핸들 가져오기
    SetConsoleCursorInfo(consoleHandle, &ConsoleCursor); // 콘솔 커서 정보 설정
}

// 텍스트 색상 설정 함수
void textcolor(int color_number) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color_number);
}

// 커서 위치 설정 함수
void gotoxy(int x, int y) {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD Cur;
    Cur.X = x;
    Cur.Y = y;
    SetConsoleCursorPosition(consoleHandle, Cur);
}

// 맵 초기화 함수
void FillMap(char str[], char str_s, int max_value) {
    for (int i = 0; i < max_value; i++) {
        str[i] = str_s;
    }
}

// 맵 수정 함수
void EditMap(int x, int y, char str) {
    if (x > 0 && y > 0 && x - 1 < MAP_X_MAX - 1 && y - 1 < MAP_Y_MAX - 1)
        mapData[(y - 1) * MAP_X_MAX + x - 1] = str;
}

// 스프라이트 그리기 함수
void DrawSprite(int x, int y, int size_x, int size_y, char spr[]) {
    for (int i = 0; i < size_y; i++) {
        for (int j = 0; j < size_x; j++) {
            EditMap(x + j, y + i, spr[i * size_x + j]);
        }
    }
}

// 플레이어 캐릭터 조작 함수
void controlCharacter(plane* pPlane) {
    if (GetAsyncKeyState(VK_UP) & 0x8000) { // 위쪽 화살표 키 입력
        if (pPlane->y > 1)
            pPlane->y--;
    }
    if (GetAsyncKeyState(VK_DOWN) & 0x8000) { // 아래쪽 화살표 키 입력
        if (pPlane->y < MAP_Y_MAX - 5)
            pPlane->y++;
    }
    if (GetAsyncKeyState(VK_LEFT) & 0x8000) { // 왼쪽 화살표 키 입력
        if (pPlane->x > 1)
            pPlane->x -= 2;
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) { // 오른쪽 화살표 키 입력
        if (pPlane->x < MAP_X_MAX - 3)
            pPlane->x += 2;
    }
}

// 게임 시작 함수
void gameStart() {
    FillMap(mapData, ' ', MAX_VALUE); // 맵 초기화
    textcolor(11); // 텍스트 색상 설정
    DrawSprite(25, 4, 30, 7, "■■■■■■■■■■■■■■■■□□□■□■■■□■□■□■■□■■■□□■□□■□■□■■□□□■□■□■□■□■□■■■■□■□■■■□■□■□■■□□□■□■■■□■■□■■■■■■■■■■■■■■■■■"); // 타이틀 스프라이트 그리기
    DrawSprite(36, 16, 11, 1, "1. 게임시작"); // 게임 시작 옵션
    DrawSprite(36, 21, 7, 1, "2. 제작"); // 제작 옵션
    DrawSprite(36, 26, 7, 1, "3. 종료"); // 종료 옵션
    if (_getch() == '1') {
        gameStatus = 1; // 게임 상태를 게임 진행 중으로 변경
    }
    else if (_getch() == '2') {
        gameStatus = 4; // 게임 상태를 제작진 화면으로 변경
    }
    else if (_getch() == '3') {
        textcolor(11);
        exit(1); // 게임 종료
    }
}

// 게임 오버 함수
void gameOver() {
    FillMap(mapData, ' ', MAX_VALUE); // 맵 초기화
    textcolor(12); // 텍스트 색상 설정
    DrawSprite(35, 5, 12, 1, "GAME OVER !!"); // 게임 오버 메시지
    DrawSprite(36, 16, 9, 1, "1. 재도전"); // 재도전 옵션
    DrawSprite(36, 21, 7, 1, "2. 제작"); // 제작 옵션
    DrawSprite(36, 26, 7, 1, "3. 종료"); // 종료 옵션
    if (_getch() == '1') {
        gameReset(&Enemy_Bullet, &Enemy_info, &Bullet_info, &myPlane); // 게임 리셋
        gameStatus = 1; // 게임 상태를 게임 진행 중으로 변경
    }
    else if (_getch() == '2') {
        gameStatus = 4; // 게임 상태를 제작진 화면으로 변경
    }
    else if (_getch() == '3') {
        textcolor(11);
        exit(1); // 게임 종료
    }
}

// 게임 클리어 함수
void gameClear() {
    FillMap(mapData, ' ', MAX_VALUE); // 맵 초기화
    textcolor(6); // 텍스트 색상 설정
    DrawSprite(24, 10, 34, 19, "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■□□□□□□□□□□□□□■■■■□■■□□□□□□□■■□■■■■□■■□□□□□□□■■□■■■■■□□□□□□□□□□□■■■■■■■■□□□□□□□■■■■■■■■■■■□□□□□■■■■■■■■■■■■■□□□■■■■■■■■■■■■■■■□■■■■■■■■■■■■■■■■□■■■■■■■■■■■■■■■■□■■■■■■■■■■■■■■■■□■■■■■■■■■■■■■■□□□□□■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■"); // 클리어 메시지 스프라이트
    DrawSprite(36, 35, 9, 1, "VICTORY!!"); // 승리 메시지
}

// 플레이어 총알 발사 함수
void BulletShoot(Bullet* pBullet, plane* pPlane, Bullet* Enemy) {
    for (int i = 0; i < MAX_BULLET; i++) {
        if (pBullet[i].active) {
            pBullet[i].y--; // 총알 이동
            mapData[pBullet[i].y * MAP_X_MAX + pBullet[i].x] = '*'; // 총알 그리기

            if (pBullet[i].y < 1) { // 총알이 맵 밖으로 나가면 비활성화
                pBullet[i].active = false;
            }

            for (int j = 0; j < 10; j++) {
                if (Enemy[j].active && Enemy[j].x == pBullet[i].x && Enemy[j].y == pBullet[i].y) { // 총알이 적과 충돌했을 때
                    Enemy[j].active = false;
                    pBullet[i].active = false;
                    score += 1000; // 점수 증가
                }
            }
        }
    }
    int a = pPlane->x; // 플레이어 위치
    int b = pPlane->y; // 플레이어 위치

    if (GetAsyncKeyState(VK_SPACE)) { // 스페이스바 입력 시 총알 발사
        if (b_index >= 9) b_index = 0;
        pBullet[b_index].x = a;
        pBullet[b_index].y = b;
        pBullet[b_index].active = true;
        b_index++;
    }

    if (GetAsyncKeyState(VK_CONTROL)) { // 컨트롤 키 입력 시 총알 재장전
        for (int i = 0; i < 10; i++) {
            pBullet[b_index].active = FALSE;
        }
        b_index = 0;
    }
}

// 적 생성 함수
void EnemyCreate(Enemy* Enemy, int count) {
    int a = rand() % 50 + 10; // 랜덤 위치
    int b = rand() % 7 + 5; // 랜덤 위치
    if (count % 200 == 0) { // 주기적으로 적 생성
        Enemy[i_index].x = a;
        Enemy[i_index].y = b;
        Enemy[i_index].active = TRUE;
        i_index++;
        if (Enemy[10].active == TRUE) { // 적이 10개 이상이면 게임 클리어
            return gameStatus = 3;
        }
    }
    for (int i = 0; i < 11; i++) {
        if (Enemy[i].active)
            mapData[Enemy[i].y * MAP_X_MAX + Enemy[i].x - 1] = '@'; // 적 그리기
    }
}

// 적 이동 함수
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

// 적 공격 함수
void EnemyAttack(Enemy* enemy, Bullet* Enemy_Bullet, plane* Plane) {
    for (int i = 0; i < MAX_BULLET * 100; i++) {
        if (Enemy_Bullet[i].active) {
            Enemy_Bullet[i].y++;
            mapData[Enemy_Bullet[i].y * MAP_X_MAX + Enemy_Bullet[i].x - 1] = '!'; // 적 총알 그리기

            if (Enemy_Bullet[i].y >= 35) {
                Enemy_Bullet[i].active = false;
            }

            if (Plane->x == Enemy_Bullet[i].x && Plane->y == Enemy_Bullet[i].y) { // 적 총알이 플레이어와 충돌했을 때
                gameStatus = 2; // 게임 오버
            }
            if (Plane->x + 1 == Enemy_Bullet[i].x && Plane->y == Enemy_Bullet[i].y) {
                gameStatus = 2; // 게임 오버
            }
            if (Plane->x + 2 == Enemy_Bullet[i].x && Plane->y == Enemy_Bullet[i].y) {
                gameStatus = 2; // 게임 오버
            }
        }
    }

    if (rand() % 10 != 0) return; // 적의 공격 주기
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

// 게임 리셋 함수
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

// 게임 옵션 표시 함수
void ShowOption() {
    int score_num = score;
    DrawSprite(1, 39, 7, 1, "총알 : ");
    for (int i = 0; i <= 10; i++) {
        if (b_index - 1 >= i)
            EditMap(10 + i, 39, 'X');
        else
            EditMap(10 + i, 39, 'O');
    }

    DrawSprite(60, 37, 18, 1, "조작 : ↑,↓,←,→");
    DrawSprite(60, 38, 12, 1, "공격 : Space");
    DrawSprite(60, 39, 13, 1, "재장전 : Ctrl");
    DrawSprite(30, 1, 8, 1, "SCORE : ");
    for (int j = 0; j <= 10; j++) {
        char str = score_num % 10 + 48;
        EditMap(48 - j, 1, str);
        score_num = score_num / 10;
    }
}

// 제작진 정보 표시 함수
void showCredits() {
    system("mode con: cols=80 lines=40"); // 콘솔 창 크기 조절
    system("title 제작진"); // 콘솔 창 제목 변경

    FillMap(mapData, ' ', MAX_VALUE); // 맵 초기화
    textcolor(10); // 텍스트 색상 설정

    // 제작자 정보 출력
    printf("제작진:\n");
    printf("조정훈\n");
    printf("조재윤\n");
    printf("원가영\n");
    printf("전시영\n");
    printf("이상규\n");

    printf("\n아무버튼을 누르면 게임 밖으로 나가집니다\n");
    _getch(); // 사용자 입력 대기

    FillMap(mapData, ' ', MAX_VALUE); // 맵 초기화
}

/*
    Tetris by 1z3r0 ver.1
*/

#include <stdio.h>
#include <time.h>
#include <windows.h>

// MACROS
// _DEBUG가 필요 없을 경우에는 주석 처리 합니다.
#define _DEBUG

#define UP 72
#define LEFT 75
#define RIGHT 77
#define DOWN 80
#define ESC 27
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define BOARD_START_X 5
#define BOARD_START_Y 3
#define BLOCK_SHAPE_SIZE 3
#define BOARD_FRAME 0
#define BLOCK 1
#define EMPTY 2
#define NEXT_SPACE 2
#define BLOCK_NUMBER 2

// GLOBAL VARIABLES
typedef struct {
    int x;
    int y;
} Point;

#ifdef _DEBUG
Point block_shape_coordinate[][4][4] = {
     { {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2}, {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2} },
     { {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1} },};
     /*{ {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1}, {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1} },
     { {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1}, {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1} }, };
     */
#else
Point block_shape_coordinate[][4][4] = {
     { {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2}, {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2} },
     { {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1} },
     { {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1}, {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1} },
     { {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1}, {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1} }, };

#endif // _DEBUG
int shape;
int next_shape;
int rotation;
int coordinate;
int board[BOARD_WIDTH+2][BOARD_HEIGHT+2];
char block_shape[][BLOCK_SHAPE_SIZE] = {"▩", "▣", "  "};
int x, y, i;
int block_x, block_y;
int frame_time, stay_time;
int score;


// FUNCTION PROTOTYPE
typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE; // setcursortype func enum
void setCursorType(CURSOR_TYPE c); // cursor on/off func
void gotoXY(int x, int y); // x,y - coordinate change func

void initializeBoard(void);
void drawCompleteBoard(void);
void drawBoard(void);
void showBlock(BOOL show);
void showNextBlock(BOOL show);
BOOL moveBlock(void);
BOOL checkSpace(int x, int y, int rotation);
void testFull(void);
BOOL keyInput(void);
void gameStart(void);
void showStartMenu(void);
void showGameOver(void);
void refreshSideBoard(void);

// main start
int main(void)
{
    srand(time(NULL));
    setCursorType(NOCURSOR);

    showStartMenu();
    while(TRUE) {
        gameStart();
        showGameOver();
    }
    return 0;
}
// main end

// FUNCTION DEFINITION
void refreshSideBoard(void)
{
    int nx, ny;

    nx = 33;
    ny = 4;

    gotoXY(nx+2,ny);   printf("                ");
    gotoXY(nx+2,ny);   printf("Score: %6d", score);
}
void showNextBlock(BOOL show)
{
    int nx, ny;
    nx = 38;
    ny = 10;
    if (show == TRUE) {
        for (i = 0; i < 4; i++) {
            gotoXY(nx + block_shape_coordinate[next_shape][0][i].x*(BLOCK_SHAPE_SIZE-1), ny + block_shape_coordinate[next_shape][0][i].y);
            printf("%s", block_shape[BLOCK]);
        }
    } else if (show == FALSE) {
        for (i = 0; i < 4; i++) {
            gotoXY(nx + block_shape_coordinate[next_shape][0][i].x*(BLOCK_SHAPE_SIZE-1), ny + block_shape_coordinate[next_shape][0][i].y);
            printf("%s", block_shape[EMPTY]);
        }
    }


}
void showStartMenu(void)
{
    int x,y,frame_time;

    frame_time = 100;
    x = 5;
    y = 3;

    while (TRUE) {
        if (kbhit()) {
            break;
        }
        gotoXY(x,y);   printf("#######   #######   #######   #######   ###   #######");
        Sleep(frame_time);
        gotoXY(x,y+1); printf("   #      #            #      #     #    #    #");
        Sleep(frame_time);
        gotoXY(x,y+2); printf("   #      #######      #      #######    #    #######");
        Sleep(frame_time);
        gotoXY(x,y+3); printf("   #      #            #      #     #    #          #");
        Sleep(frame_time);
        gotoXY(x,y+4); printf("   #      #######      #      #     #   ###   #######");
        Sleep(frame_time);
        for (;;) {
            if (kbhit()) {
                getch();
                system("cls");
                return;
            }
            gotoXY(x+15,y+12); printf("PRESS ANY KEY TO START");
            Sleep(frame_time);
            gotoXY(x+15,y+12); printf("                      ");
            Sleep(frame_time);
        }
    }
}
void showGameOver(void)
{
    int x, y;

    x = 3;
    y = 7;
    gotoXY(x,y);   printf("+---------------------------+");
    gotoXY(x,y+1); printf("|     Your Score : %6d   |");
    gotoXY(x,y+2); printf("|                           |");
    gotoXY(x,y+3); printf("|        GAME OVER          |");
    gotoXY(x,y+4); printf("|                           |");
    gotoXY(x,y+5); printf("| Press any key to restart! |");
    gotoXY(x,y+6); printf("|                           |");
    gotoXY(x,y+7); printf("+---------------------------+");

    for (;;) {
        if (kbhit()) {
            getch();
            break;
        }
    }
}
void gameStart(void)
{
    frame_time = 30;
    shape = rand() % BLOCK_NUMBER;
    system("cls");

    // 보드판을 만든다
    initializeBoard();
    // 보드 화면을 그린다
    drawCompleteBoard();
    // 무한루프
    for (;;) {
        // 블럭 모양 설정
        next_shape = rand() % BLOCK_NUMBER;
        rotation = 0;
        block_x = 10;
        block_y = 4;
        // 블럭 프린트
        showBlock(TRUE);
        showNextBlock(TRUE);
        // 블럭이 생성될 공간이 없다 -> break
        if (checkSpace(block_x, block_y, rotation)) {
            break;
        }

        stay_time = frame_time;
        // 무한루프
        for (;;) {
            // 블럭 한칸씩 아래로 이동
            if (--stay_time == 0) {
                stay_time = frame_time;
                if (moveBlock()) { // 이동 중 더이상 내려갈 공간 없다 -> break
                    break;
                }
            }

            // 키보드 입력 읽기
            if (keyInput()) { // 입력 도중 더이상 내려갈 공간 없다 -> break
                break;
            }
            Sleep(40);
        }
        showNextBlock(FALSE);
        shape = next_shape;
    }
}
void setCursorType(CURSOR_TYPE c)
{
     CONSOLE_CURSOR_INFO CurInfo;

     switch (c) {
     case NOCURSOR:
          CurInfo.dwSize=1;
          CurInfo.bVisible=FALSE;
          break;
     case SOLIDCURSOR:
          CurInfo.dwSize=100;
          CurInfo.bVisible=TRUE;
          break;
     case NORMALCURSOR:
          CurInfo.dwSize=20;
          CurInfo.bVisible=TRUE;
          break;
     }
     SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&CurInfo);
}

void gotoXY(int x, int y)
{
     COORD Cur;
     Cur.X=x;
     Cur.Y=y;
     SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),Cur);
}

void initializeBoard(void)
{
     for (x = 0; x < BOARD_WIDTH+2; x++) {
        for (y = 0; y <  BOARD_HEIGHT+2; y++) {
            if (y == 0 || x == 0 || y == BOARD_HEIGHT+1 || x == BOARD_WIDTH+1) {
                board[x][y] = BOARD_FRAME;
            } else {
                board[x][y] = EMPTY;
            }
        }
     }
}

void drawCompleteBoard(void)
{
    int nx, ny;
    nx = 33;
    ny = 4;

    for (x = 0; x < BOARD_WIDTH+2; x++) {
        for (y = 0; y <  BOARD_HEIGHT+2; y++) {
            if (board[x][y] == BOARD_FRAME) {
                gotoXY(BOARD_START_X+x*(BLOCK_SHAPE_SIZE-1),BOARD_START_Y+y);
                printf("%s", block_shape[BOARD_FRAME]);
            }
        }
    }

    gotoXY(nx+2,ny);   printf("Score: %6d", score);
    gotoXY(nx+2,ny+2); printf("Next Shape");
    gotoXY(nx,ny+3);   printf("+============+");
    gotoXY(nx,ny+4);   printf("|            |");
    gotoXY(nx,ny+5);   printf("|            |");
    gotoXY(nx,ny+6);   printf("|            |");
    gotoXY(nx,ny+7);   printf("|            |");
    gotoXY(nx,ny+8);   printf("+============+");
    gotoXY(nx,ny+10);  printf("  ▲    : Rotate          Space : Hard Drop");
    gotoXY(nx,ny+11);  printf("◀  ▶ : Left / Right"  );
    gotoXY(nx,ny+12);  printf("  ▼    : Soft Drop       ESC : Quit");
}
void drawBoard(void)
{
    for (x = 1; x < BOARD_WIDTH+1; x++) {
        for (y = 1; y <  BOARD_HEIGHT+1; y++) {
            if (board[x][y] == BLOCK) {
                gotoXY(BOARD_START_X+x*(BLOCK_SHAPE_SIZE-1),BOARD_START_Y+y);
                printf("%s", block_shape[BLOCK]);
            }
            else if (board[x][y] == EMPTY) {
                gotoXY(BOARD_START_X+x*(BLOCK_SHAPE_SIZE-1),BOARD_START_Y+y);
                printf("%s", block_shape[EMPTY]);
            }
        }
    }
}
void showBlock(BOOL show)
{
    if (show == TRUE) {
        for (i = 0; i < 4; i++) {
            gotoXY(BOARD_START_X + block_shape_coordinate[shape][rotation][i].x*(BLOCK_SHAPE_SIZE-1) + block_x, BOARD_START_Y +block_shape_coordinate[shape][rotation][i].y + block_y);
            printf("%s", block_shape[BLOCK]);
        }
    } else if (show == FALSE) {
        for (i = 0; i < 4; i++) {
            gotoXY(BOARD_START_X + block_shape_coordinate[shape][rotation][i].x*(BLOCK_SHAPE_SIZE-1) + block_x, BOARD_START_Y +block_shape_coordinate[shape][rotation][i].y + block_y);
            printf("%s", block_shape[EMPTY]);
        }
    }
}
BOOL checkSpace(int x, int y, int rotation)
{
    int block_temp_x = x;
    int block_temp_y = y;
    int temp_rotation = rotation;
    for (i = 0; i < 4; i++) {
        if (board[(block_shape_coordinate[shape][temp_rotation][i].x*(BLOCK_SHAPE_SIZE-1)+block_temp_x)/2][block_shape_coordinate[shape][temp_rotation][i].y + block_temp_y] != EMPTY) {
            if (block_temp_y > block_y) {
                testFull();
            }
            return TRUE;
        }
    }
    return FALSE;
}
BOOL moveBlock(void)
{
    if (checkSpace(block_x, block_y+1, rotation)) {
        return TRUE;
    }
    showBlock(FALSE);
    block_y += 1;
    showBlock(TRUE);

    return FALSE;
}
void testFull(void)
{
     // 한번에 두개의 라인이 클리어가 되지 않는다 수정 필요 (2017-08-16)
     int temp_y;

     int combo = 1;
     for (i = 0; i < 4; i++) {
        board[(block_shape_coordinate[shape][rotation][i].x*(BLOCK_SHAPE_SIZE-1)+block_x)/2][block_shape_coordinate[shape][rotation][i].y + block_y] = BLOCK;
     }

     for (y = BOARD_HEIGHT; y > 1; y--) {
        for (x = 1; x < BOARD_WIDTH + 1; x++) {
            if (board[x][y] == EMPTY) {
                break;
            }
        }
        if (x == BOARD_WIDTH + 1) {
            score += combo * 100;
            combo++;
            for (temp_y = y; temp_y > 1; temp_y--) {
                for (x = 1; x < BOARD_WIDTH + 1; x++) {
                    board[x][temp_y] = board[x][temp_y-1];
                }
            }
            y++;
        }
     }
     refreshSideBoard();
     drawBoard();
}

BOOL keyInput(void)
{
    int key;
    int temp_rotation;
    if (kbhit()) {
        key = getch();
        if (key == 224 || key == 0) {
            key = getch();
            switch (key) {
                case UP:
                    if (rotation + 1 > 3) {
                        temp_rotation = 0;
                    } else {
                        temp_rotation = rotation + 1;
                    }
                    if (checkSpace(block_x, block_y, temp_rotation) == FALSE) {
                        showBlock(FALSE);
                        rotation = temp_rotation;
                        showBlock(TRUE);
                    }
                    break;
                case LEFT:
                    if (checkSpace(block_x-NEXT_SPACE, block_y, rotation) == FALSE) {
                        showBlock(FALSE);
                        block_x -= 2;
                        showBlock(TRUE);
                    }
                    break;
                case RIGHT:
                    if (checkSpace(block_x+NEXT_SPACE, block_y, rotation) == FALSE) {
                        showBlock(FALSE);
                        block_x += 2;
                        showBlock(TRUE);
                    }
                    break;
                case DOWN:
                    if (checkSpace(block_x, block_y+1, rotation) == FALSE) {
                        moveBlock();
                    }
                    break;
                default:
                    break;
            }
        } else {
            switch(key) {
                case ' ':
                    while(moveBlock() == FALSE) {
                    }
                    return TRUE;
                    break;
                case ESC:
                    system("cls");
                    exit(0);
                    break;
                default:
                    break;
            }
        }
    }
    return FALSE;
}

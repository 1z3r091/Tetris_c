#include <stdio.h>
#include <time.h>
#include <windows.h>

// MACROS
#define UP 72
#define LEFT 75
#define RIGHT 77
#define DOWN 80
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define BOARD_START_X 5
#define BOARD_START_Y 3
#define BLOCK_SHAPE_SIZE 3
#define BOARD_FRAME 0
#define BLOCK 1
#define EMPTY 2
#define NEXT_SPACE 2

// GLOBAL VARIABLES
typedef struct {
    int x;
    int y;
} Point;

Point block_shape_coordinate[][4][4] = {
     { {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2}, {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2} },
     { {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1} },
     { {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1}, {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1} },
     { {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1}, {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1} }, };

int shape;
int rotation;
int coordinate;
int board[BOARD_WIDTH+2][BOARD_HEIGHT+2];
char block_shape[][BLOCK_SHAPE_SIZE] = {"▩", "▣", "  "};
int x, y, i;
int block_x, block_y;
int frame_time, stay_time;

// FUNCTION PROTOTYPE
typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE; // setcursortype func enum
void setCursorType(CURSOR_TYPE c); // cursor on/off func
void gotoXY(int x, int y); // x,y - coordinate change func

void initializeBoard(void);
void drawCompleteBoard(void);
void drawBoard(void);
void showBlock(BOOL show);
BOOL moveBlock(void);
BOOL checkSpace(int x, int y, int rotation);
void testFull(void);
BOOL keyInput(void);

// main start
int main(void)
{

    srand(time(NULL));
    setCursorType(NOCURSOR);
    frame_time = 30;
    // 보드판을 만든다
    initializeBoard();
    // 보드 화면을 그린다
    drawCompleteBoard();
    // 무한루프
    for (;;) {
        // 블럭 모양 설정
        shape = rand() % 4;
        rotation = 0;
        block_x = 10;
        block_y = 4;
        // 블럭 프린트
        showBlock(TRUE);
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
    }
    system("cls");
    return 0;
}
// main end

// FUNCTION DEFINITION
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
    for (x = 0; x < BOARD_WIDTH+2; x++) {
        for (y = 0; y <  BOARD_HEIGHT+2; y++) {
            if (board[x][y] == BOARD_FRAME) {
                gotoXY(BOARD_START_X+x*(BLOCK_SHAPE_SIZE-1),BOARD_START_Y+y);
                printf("%s", block_shape[BOARD_FRAME]);
            }
        }
    }

    gotoXY(33,3);
    printf("TETRIS");
    gotoXY(33,5);
    printf("HAVE FUN~");
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
            for (; y > 1; y--) {
                for (x = 1; x < BOARD_WIDTH + 1; x++) {
                    board[x][y] = board[x][y-1];
                }
            }
        }
     }

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
                default:
                    break;
            }
        }
    }
    return FALSE;
}

#include <stdio.h>
#include <time.h>
#include <windows.h>

// MACROS
// _DEBUG가 필요 없을 경우에는 주석 처리 합니다
#define _DEBUG

#define UP 72
#define LEFT 75
#define RIGHT 77
#define DOWN 80
#define ESC 27
#define BOARD_START_X 5
#define BOARD_START_Y 3
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define BLOCK_SHAPE_SIZE 3
#define BOARD_FRAME 0
#define BLOCK 1
#define EMPTY 2
#define FILLED_BLOCK 3
#define NEXT_SPACE 2
#define BLOCK_NUMBER 2
#define ONE 0
#define TWO 1
#define X_DISTANCE 66
#define PLAYER_NUM 2

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
typedef struct {
    int shape;
    int next_shape;
    int rotation;
    int coordinate;
    int game_board[BOARD_WIDTH+2][BOARD_HEIGHT+2];
    int copy_game_board[BOARD_WIDTH+2][BOARD_HEIGHT+2];
    int block_x, block_y;
    int frame_time, stay_time;
    int score;
    int stage_level;
    int stage_clear_cnt;
    int clear_cnt;
    int board_x;
} Player;

char block_shape[][BLOCK_SHAPE_SIZE] = {"▩", "□", "  ", "■"};
int x, y, i; // for loop variables
int key_input_frame;

Player player[PLAYER_NUM];

// FUNCTION PROTOTYPE
typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE; // setcursortype func enum
void setCursorType(CURSOR_TYPE c); // cursor on/off func
void gotoXY(int x, int y); // x,y - coordinate change func

void initializeBoard(void);
void initializeGame(void);
void initializeSetting(int nplayer);
void drawCompleteBoard(int nx, int nplayer);
void drawBoard(int nx, int nplayer);
void showBlock(BOOL show, int x, int nplayer);
void showNextBlock(BOOL show, int x, int nplayer);
BOOL moveBlock(int nplayer);
BOOL checkSpace(int x, int y, int rotation,int nplayer);
void testFull(int nplayer);
BOOL keyInput(int nplayer);
void gameStart(int nplayer);
void showStartMenu(void);
void showGameOver(int nx, int ny, int nplayer);
void refreshSideBoard(int nx, int nplayer);
void showNextStage(int nx, int nplayer);
void copyGameBoard(int copy_board[][BOARD_HEIGHT+2], int board[][BOARD_HEIGHT+2]);

// main start
int main(void)
{
    srand(time(NULL));
    setCursorType(NOCURSOR);
    key_input_frame = 100;

    for (i = ONE; i <= TWO; i++) {
        player[i].stage_level = 1; // initial stage level
        player[i].stage_clear_cnt = player[i].stage_level * 1; // clear line cnt limit for each stage level
        player[i].board_x = i * 51;
    }
    showStartMenu();
    initializeGame();
    initializeSetting(ONE);
    initializeSetting(TWO);
    while(TRUE) {
        gameStart(ONE);
        gameStart(TWO);
    }
    return 0;
}
// main end

// FUNCTION DEFINITION
void copyGameBoard(int copy_board[][BOARD_HEIGHT+2], int board[][BOARD_HEIGHT+2])
{
    for (x = 1; x < BOARD_WIDTH + 1; x++) {
        for (y = 1; y < BOARD_HEIGHT + 1; y++) {
            copy_board[x][y] = board[x][y];
        }
    }
}
void showNextStage(int nx, int nplayer)
{
    // set color
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);

    gotoXY(nx+BOARD_START_X+5,BOARD_START_Y+4);  printf("+-------------+");
    gotoXY(nx+BOARD_START_X+5,BOARD_START_Y+5);  printf("|  Stage %d    |", player[nplayer].stage_level);
    gotoXY(nx+BOARD_START_X+5,BOARD_START_Y+6);  printf("+-------------+");
    Sleep(3000);
    initializeBoard();
    drawBoard(player[nplayer].board_x, nplayer);
    refreshSideBoard(player[nplayer].board_x, nplayer);
}
void refreshSideBoard(int nx, int nplayer)
{
    // set color
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);

    gotoXY(BOARD_START_X+25+nx,BOARD_START_Y);     printf("                ");
    gotoXY(BOARD_START_X+25+nx,BOARD_START_Y);     printf("Stage: %6d", player[nplayer].stage_level);
    gotoXY(BOARD_START_X+25+nx,BOARD_START_Y+1);   printf("                ");
    gotoXY(BOARD_START_X+25+nx,BOARD_START_Y+1);   printf("Score: %6d", player[nplayer].score);
}
void showNextBlock(BOOL show, int x, int nplayer)
{
    int nx, ny;
    nx = BOARD_START_X+30;
    ny = BOARD_START_Y+5;
    if (show == TRUE) {
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 15);
        for (i = 0; i < 4; i++) {
            gotoXY(x + nx + block_shape_coordinate[player[nplayer].next_shape][0][i].x*(BLOCK_SHAPE_SIZE-1), ny + block_shape_coordinate[player[nplayer].next_shape][0][i].y);
            printf("%s", block_shape[FILLED_BLOCK]);
        }
    } else if (show == FALSE) {
        for (i = 0; i < 4; i++) {
            gotoXY(x + nx + block_shape_coordinate[player[nplayer].next_shape][0][i].x*(BLOCK_SHAPE_SIZE-1), ny + block_shape_coordinate[player[nplayer].next_shape][0][i].y);
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
        // set color
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), x + 5);
        gotoXY(x,y);   printf("#######   #######   #######   #######   ###   #######");
        Sleep(frame_time);
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), x + 6);
        gotoXY(x,y+1); printf("   #      #            #      #     #    #    #");
        Sleep(frame_time);
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), x + 7);
        gotoXY(x,y+2); printf("   #      #######      #      #######    #    #######");
        Sleep(frame_time);
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), x + 8);
        gotoXY(x,y+3); printf("   #      #            #      #     #    #          #");
        Sleep(frame_time);
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), x + 9);
        gotoXY(x,y+4); printf("   #      #######      #      #     #   ###   #######");
        Sleep(frame_time);
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), x - 1);
        gotoXY(x+28,y+6);printf("               PLAYER 1                   ");
        gotoXY(x+28,y+7);printf("  ▲    : Rotate          Space : Hard Drop");
        gotoXY(x+28,y+8);printf("◀  ▶  : Left / Right"  );
        gotoXY(x+28,y+9);printf("  ▼    : Soft Drop       ESC : Quit");
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), x - 2);
        gotoXY(x+28,y+11);printf("               PLAYER 2                   ");
        gotoXY(x+28,y+12);printf("  T    : Rotate          A : Hard Drop");
        gotoXY(x+28,y+13);printf("F   H  : Left / Right"  );
        gotoXY(x+28,y+14);printf("  G    : Soft Drop       ESC : Quit");
        for (;;) {
            if (kbhit()) {
                getch();
                SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);
                system("cls");
                return;
            }
            SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), x - 3);
            gotoXY(x+15,y+17); printf("PRESS ANY KEY TO START");
            Sleep(frame_time+300);
            gotoXY(x+15,y+17); printf("                      ");
            Sleep(frame_time+300);
        }
    }
}
void showGameOver(int nx, int ny, int nplayer)
{
    gotoXY(nx,ny);   printf("+---------------------------+");
    gotoXY(nx,ny+1); printf("|     Your Score : %6d   |", player[nplayer].score);
    gotoXY(nx,ny+2); printf("|                           |");
    gotoXY(nx,ny+3); printf("|        GAME OVER          |");
    gotoXY(nx,ny+4); printf("|                           |");
    gotoXY(nx,ny+5); printf("| Press any key to restart! |");
    gotoXY(nx,ny+6); printf("|                           |");
    gotoXY(nx,ny+7); printf("+---------------------------+");

    for (;;) {
        if ((GetAsyncKeyState(VK_RETURN) & 0x8000) && nplayer == ONE) {
            break;
        } else if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) && nplayer == TWO) {
            break;
        }
    }
}
void initializeGame()
{
    for (i = ONE; i <= TWO; i++) {
        player[i].frame_time = 15;
        player[i].shape = rand() % BLOCK_NUMBER;
        player[i].score = 0;
        player[i].stage_level = 0;
    }
    system("cls");

    // 보드판을 만든다
    initializeBoard();
    // 보드 화면을 그린다
    for (i = ONE; i <= TWO; i++) {
        drawCompleteBoard(player[i].board_x,i);
    }

    for (i = ONE; i <= TWO; i++) {
        copyGameBoard(player[i].copy_game_board, player[i].game_board);
    }
}
void initializeSetting(int nplayer)
{
    int i;
    // 블럭 모양 설정
    player[nplayer].next_shape = rand() % BLOCK_NUMBER;
    player[nplayer].rotation = 0;
    player[nplayer].block_x = 10;
    player[nplayer].block_y = 4;
    // 블럭 프린트
    showBlock(TRUE,player[nplayer].board_x,nplayer);
    showNextBlock(TRUE,player[nplayer].board_x, nplayer);
    // 블럭이 생성될 공간이 없다 -> break
    if (checkSpace(player[nplayer].block_x, player[nplayer].block_y, player[nplayer].rotation,nplayer)) {
        // game over -> restart
        showGameOver(player[nplayer].board_x+3,7,nplayer);
        initializeGame();
        for (i = ONE; i <= TWO; i++) {
            showNextBlock(TRUE,player[i].board_x,i);
        }
    }
    player[nplayer].stay_time = player[nplayer].frame_time;
}
void gameStart(int nplayer)
{
    // 블럭 한칸씩 아래로 이동
    if (--player[nplayer].stay_time == 0) {
        player[nplayer].stay_time = player[nplayer].frame_time;
        if (moveBlock(nplayer)) { // 이동 중 더이상 내려갈 공간 없다 -> break
            showNextBlock(FALSE,player[nplayer].board_x,nplayer);
            player[nplayer].shape = player[nplayer].next_shape;
            initializeSetting(nplayer);
        }
    }

    // 키보드 입력 읽기
    if (keyInput(nplayer)) { // 입력 도중 더이상 내려갈 공간 없다 -> break
        showNextBlock(FALSE,player[nplayer].board_x, nplayer);
        player[nplayer].shape = player[nplayer].next_shape;
        initializeSetting(nplayer);
    }
    Sleep(30);
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
     for (i = ONE; i <= TWO; i++) {
         for (x = 0; x < BOARD_WIDTH+2; x++) {
            for (y = 0; y <  BOARD_HEIGHT+2; y++) {
                if (y == 0 || x == 0 || y == BOARD_HEIGHT+1 || x == BOARD_WIDTH+1) {
                    player[i].game_board[x][y] = BOARD_FRAME;
                } else {
                    player[i].game_board[x][y] = EMPTY;
                }
            }
         }
     }
}

void drawCompleteBoard(int nx, int nplayer)
{
    for (x = 0; x < BOARD_WIDTH+2; x++) {
        for (y = 0; y <  BOARD_HEIGHT+2; y++) {
            if (player[nplayer].game_board[x][y] == BOARD_FRAME) {
                gotoXY(BOARD_START_X+x*(BLOCK_SHAPE_SIZE-1)+nx,BOARD_START_Y+y);
                printf("%s", block_shape[BOARD_FRAME]);
            }
        }
    }

    gotoXY(BOARD_START_X+25+nx,BOARD_START_Y);     printf("Stage: %6d", player[nplayer].stage_level);
    gotoXY(BOARD_START_X+25+nx,BOARD_START_Y+1);   printf("Score: %6d", player[nplayer].score);
    gotoXY(BOARD_START_X+25+nx,BOARD_START_Y+2);   printf("Next Shape");
    gotoXY(BOARD_START_X+25+nx,BOARD_START_Y+3);   printf("+============+");
    gotoXY(BOARD_START_X+25+nx,BOARD_START_Y+4);   printf("|            |");
    gotoXY(BOARD_START_X+25+nx,BOARD_START_Y+5);   printf("|            |");
    gotoXY(BOARD_START_X+25+nx,BOARD_START_Y+6);   printf("|            |");
    gotoXY(BOARD_START_X+25+nx,BOARD_START_Y+7);   printf("|            |");
    gotoXY(BOARD_START_X+25+nx,BOARD_START_Y+8);   printf("+============+");
}
void drawBoard(int nx, int nplayer)
{
    for (x = 1; x < BOARD_WIDTH+1; x++) {
        for (y = 1; y <  BOARD_HEIGHT+1; y++) {
            if (player[nplayer].game_board[x][y] == BLOCK) {
                gotoXY(nx + BOARD_START_X+x*(BLOCK_SHAPE_SIZE-1),BOARD_START_Y+y);
                printf("%s", block_shape[BLOCK]);
            } else if (player[nplayer].game_board[x][y] == FILLED_BLOCK && player[nplayer].copy_game_board[x][y] != player[nplayer].game_board[x][y]) {
                SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), player[nplayer].shape + 10);
                gotoXY(nx + BOARD_START_X+x*(BLOCK_SHAPE_SIZE-1),BOARD_START_Y+y);
                printf("%s", block_shape[FILLED_BLOCK]);
            } else if (player[nplayer].game_board[x][y] == EMPTY) {
                gotoXY(nx + BOARD_START_X+x*(BLOCK_SHAPE_SIZE-1),BOARD_START_Y+y);
                printf("%s", block_shape[EMPTY]);
            }
        }
    }
}
void showBlock(BOOL show, int x, int nplayer)
{
    if (show == TRUE) {
        for (i = 0; i < 4; i++) {
            gotoXY(x+BOARD_START_X + block_shape_coordinate[player[nplayer].shape][player[nplayer].rotation][i].x*(BLOCK_SHAPE_SIZE-1) + player[nplayer].block_x, BOARD_START_Y +block_shape_coordinate[player[nplayer].shape][player[nplayer].rotation][i].y + player[nplayer].block_y);
            // set block color
            SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), player[nplayer].shape + 10);

            printf("%s", block_shape[BLOCK]);
        }
    } else if (show == FALSE) {
        for (i = 0; i < 4; i++) {
            gotoXY(x+BOARD_START_X + block_shape_coordinate[player[nplayer].shape][player[nplayer].rotation][i].x*(BLOCK_SHAPE_SIZE-1) + player[nplayer].block_x, BOARD_START_Y +block_shape_coordinate[player[nplayer].shape][player[nplayer].rotation][i].y + player[nplayer].block_y);
            printf("%s", block_shape[EMPTY]);
        }
    }
}
BOOL checkSpace(int x, int y, int rotation, int nplayer)
{
    int block_temp_x = x;
    int block_temp_y = y;
    int temp_rotation = rotation;

    // set block color
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 15);

    for (i = 0; i < 4; i++) {
        if (player[nplayer].game_board[(block_shape_coordinate[player[nplayer].shape][temp_rotation][i].x*(BLOCK_SHAPE_SIZE-1)+block_temp_x)/2][block_shape_coordinate[player[nplayer].shape][temp_rotation][i].y + block_temp_y] != EMPTY) {
            if (block_temp_y > player[nplayer].block_y) {
                testFull(nplayer);
            }
            return TRUE;
        }
    }
    return FALSE;
}
BOOL moveBlock(int nplayer)
{
    if (checkSpace(player[nplayer].block_x, player[nplayer].block_y+1, player[nplayer].rotation,nplayer)) {
        return TRUE;
    }
    showBlock(FALSE,player[nplayer].board_x,nplayer);
    player[nplayer].block_y += 1;
    showBlock(TRUE,player[nplayer].board_x,nplayer);

    return FALSE;
}
void testFull(int nplayer)
{
     int temp_y;
     int combo = 1;

     for (i = 0; i < 4; i++) {
        player[nplayer].game_board[(block_shape_coordinate[player[nplayer].shape][player[nplayer].rotation][i].x*(BLOCK_SHAPE_SIZE-1)+player[nplayer].block_x)/2][block_shape_coordinate[player[nplayer].shape][player[nplayer].rotation][i].y + player[nplayer].block_y] = FILLED_BLOCK;
     }

     for (y = BOARD_HEIGHT; y > 1; y--) {
        for (x = 1; x < BOARD_WIDTH + 1; x++) {
            if (player[nplayer].game_board[x][y] == EMPTY) {
                break;
            }
        }
        if (x == BOARD_WIDTH + 1) {
            // score
            player[nplayer].score += combo * 100;
            combo++;

            // nextStage
            player[nplayer].clear_cnt += 1;

            for (temp_y = y; temp_y > 1; temp_y--) {
                for (x = 1; x < BOARD_WIDTH + 1; x++) {
                    player[nplayer].game_board[x][temp_y] = player[nplayer].game_board[x][temp_y-1];
                }
            }
            y++;
        }
     }
     // next Stage
     if (player[nplayer].clear_cnt >= player[nplayer].stage_clear_cnt) {
        player[nplayer].clear_cnt = 0;
        player[nplayer].stage_level += 1;
        player[nplayer].frame_time /= 2; // move speed increase
        refreshSideBoard(player[nplayer].board_x,nplayer);
        initializeBoard();
        drawBoard(player[nplayer].board_x,nplayer);
        copyGameBoard(player[nplayer].copy_game_board, player[nplayer].game_board);
        showNextStage(player[nplayer].board_x, nplayer);
        return;
     }
     refreshSideBoard(player[nplayer].board_x,nplayer);
     drawBoard(player[nplayer].board_x, nplayer);
     copyGameBoard(player[nplayer].copy_game_board, player[nplayer].game_board);
}

BOOL keyInput(int nplayer)
{
    int key;
    int temp_rotation;

    while (key_input_frame >= 0) {
        key_input_frame--;
    }
    key_input_frame = 100;
    if (nplayer == ONE) {
        if (GetAsyncKeyState(VK_UP) & 0x8000) {
            if (player[nplayer].rotation + 1 > 3) {
                temp_rotation = 0;
            } else {
                temp_rotation = player[nplayer].rotation + 1;
            }
            if (checkSpace(player[nplayer].block_x, player[nplayer].block_y, temp_rotation,nplayer) == FALSE) {
                showBlock(FALSE,player[nplayer].board_x,nplayer);
                player[nplayer].rotation = temp_rotation;
                showBlock(TRUE,player[nplayer].board_x,nplayer);
            }
        } else if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
            if (checkSpace(player[nplayer].block_x-NEXT_SPACE, player[nplayer].block_y, player[nplayer].rotation,nplayer) == FALSE) {
                showBlock(FALSE,player[nplayer].board_x,nplayer);
                player[nplayer].block_x -= 2;
                showBlock(TRUE,player[nplayer].board_x,nplayer);
            }
        } else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
            if (checkSpace(player[nplayer].block_x+NEXT_SPACE, player[nplayer].block_y, player[nplayer].rotation,nplayer) == FALSE) {
                showBlock(FALSE,player[nplayer].board_x,nplayer);
                player[nplayer].block_x += 2;
                showBlock(TRUE,player[nplayer].board_x,nplayer);
            }
        } else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
            if (checkSpace(player[nplayer].block_x, player[nplayer].block_y+1, player[nplayer].rotation,nplayer) == FALSE) {
                moveBlock(nplayer);
            }
        } else if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
            while(moveBlock(nplayer) == FALSE) {
            }
            return TRUE;
        }
    } else if (nplayer == TWO) {
        if (GetAsyncKeyState('T') & 0x8000) {
            if (player[nplayer].rotation + 1 > 3) {
                temp_rotation = 0;
            } else {
                temp_rotation = player[nplayer].rotation + 1;
            }
            if (checkSpace(player[nplayer].block_x, player[nplayer].block_y, temp_rotation,nplayer) == FALSE) {
                showBlock(FALSE,player[nplayer].board_x,nplayer);
                player[nplayer].rotation = temp_rotation;
                showBlock(TRUE,player[nplayer].board_x,nplayer);
            }
        } else if (GetAsyncKeyState('F') & 0x8000) {
            if (checkSpace(player[nplayer].block_x-NEXT_SPACE, player[nplayer].block_y, player[nplayer].rotation,nplayer) == FALSE) {
                showBlock(FALSE,player[nplayer].board_x,nplayer);
                player[nplayer].block_x -= 2;
                showBlock(TRUE,player[nplayer].board_x,nplayer);
            }
        } else if (GetAsyncKeyState('H') & 0x8000) {
            if (checkSpace(player[nplayer].block_x+NEXT_SPACE, player[nplayer].block_y, player[nplayer].rotation,nplayer) == FALSE) {
                showBlock(FALSE,player[nplayer].board_x,nplayer);
                player[nplayer].block_x += 2;
                showBlock(TRUE,player[nplayer].board_x,nplayer);
            }
        } else if (GetAsyncKeyState('G') & 0x8000) {
            if (checkSpace(player[nplayer].block_x, player[nplayer].block_y+1, player[nplayer].rotation,nplayer) == FALSE) {
                moveBlock(nplayer);
            }
        } else if (GetAsyncKeyState('A') & 0x8000) {
            while(moveBlock(nplayer) == FALSE) {
            }
            return TRUE;
        }
    }

    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
        system("cls");
        exit(0);
    }
    return FALSE;
}

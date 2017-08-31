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
// int key_input_frame;

// Player player[PLAYER_NUM];

// FUNCTION PROTOTYPE
typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE; // setcursortype func enum
void setCursorType(CURSOR_TYPE c); // cursor on/off func
void gotoXY(int x, int y); // x,y - coordinate change func

void initializeBoard(Player *player);
void drawCompleteBoard(Player *player);
void drawBoard(Player *player);
void showBlock(BOOL show, Player *player);
void showNextBlock(BOOL show, Player *player);
void moveBlock(Player *player);
BOOL checkSpace(int x, int y, int rotation, Player *player);
BOOL testFull(Player *player);
BOOL keyInput(int nplayer, Player *player);
void gameStart(int nplayer, Player *player);
void showStartMenu(void);
void showGameOver(int nplayer, Player *player);
void refreshSideBoard(Player *player);
void showNextStage(Player *player);
void copyGameBoard(Player *player);

void initializeBlockSetting(Player *player);
BOOL checkGameOver(Player *player);
void showCurrentnNextBlock(Player *player);
void initializePlayerSetting(int nplayer, Player *player);
BOOL checkNextSpace(Player *player);
BOOL checkNextStage(Player *player);
void initializeNextStage(Player *player);
void saveBlockinBoard(Player *player);
// main start
int main(void)
{
    Player player[PLAYER_NUM];

    int x, y;
    srand(time(NULL));
    setCursorType(NOCURSOR);
    // int key_input_frame = 100;
    int id = 1; // 서버 에서 받아야됨
    player[id].shape = rand() % BLOCK_NUMBER;

    showStartMenu();

    initializePlayerSetting(id, &player[id]);
    initializeBlockSetting(&player[id]);
    initializeBoard(&player[id]);

    drawCompleteBoard(&player[id]);

    copyGameBoard(&player[id]);

    showCurrentnNextBlock(&player[id]);

    // initializeSetting(TWO);
    while(TRUE) {
        gameStart(id, &player[id]);
        // gameStart(TWO);
    }

    return 0;
}
// main end

// FUNCTION DEFINITION

void initializePlayerSetting(int nplayer, Player *player)
{
    player->stage_level = 1; // initial stage level
    player->stage_clear_cnt = player->stage_level * 1; // clear line cnt limit for each stage level
    player->clear_cnt = 0;
    player->board_x = nplayer * 51;
    player->stay_time = player->frame_time = 15;
    player->score = 0;
    player->stage_level = 0;
}

// 로직
void copyGameBoard(Player *player)
{
    int x, y;
    for (x = 1; x < BOARD_WIDTH + 1; x++) {
        for (y = 1; y < BOARD_HEIGHT + 1; y++) {
            player->copy_game_board[x][y] = player->game_board[x][y];
        }
    }
}

// 뷰
void showNextStage(Player *player)
{
    // set color
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);

    gotoXY(player->board_x+BOARD_START_X+5,BOARD_START_Y+4);  printf("+-------------+");
    gotoXY(player->board_x+BOARD_START_X+5,BOARD_START_Y+5);  printf("|  Stage %d    |", player->stage_level);
    gotoXY(player->board_x+BOARD_START_X+5,BOARD_START_Y+6);  printf("+-------------+");
    Sleep(3000);
    //initializeBoard(nplayer,&player[nplayer]);
    //drawBoard(player->board_x, nplayer, &player[nplayer]);
    //refreshSideBoard(player->board_x, nplayer,&player[nplayer]);
}

// 뷰
void refreshSideBoard(Player *player)
{
    // set color
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);

    gotoXY(BOARD_START_X+25+player->board_x,BOARD_START_Y);     printf("                ");
    gotoXY(BOARD_START_X+25+player->board_x,BOARD_START_Y);     printf("Stage: %6d", player->stage_level);
    gotoXY(BOARD_START_X+25+player->board_x,BOARD_START_Y+1);   printf("                ");
    gotoXY(BOARD_START_X+25+player->board_x,BOARD_START_Y+1);   printf("Score: %6d", player->score);
}

// 뷰
void showNextBlock(BOOL show, Player *player)
{
    int nx, ny;
    int i;
    nx = BOARD_START_X+30;
    ny = BOARD_START_Y+5;
    if (show == TRUE) {
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 15);
        for (i = 0; i < 4; i++) {
            gotoXY(player->board_x + nx + block_shape_coordinate[player->next_shape][0][i].x*(BLOCK_SHAPE_SIZE-1), ny + block_shape_coordinate[player->next_shape][0][i].y);
            printf("%s", block_shape[FILLED_BLOCK]);
        }
    } else if (show == FALSE) {
        for (i = 0; i < 4; i++) {
            gotoXY(player->board_x + nx + block_shape_coordinate[player->next_shape][0][i].x*(BLOCK_SHAPE_SIZE-1), ny + block_shape_coordinate[player->next_shape][0][i].y);
            printf("%s", block_shape[EMPTY]);
        }
    }
}

// 뷰
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

// 뷰
void showGameOver(int nplayer, Player *player)
{
    int nx = player->board_x + 3;
    int ny = 7;
    gotoXY(nx,ny);   printf("+---------------------------+");
    gotoXY(nx,ny+1); printf("|     Your Score : %6d   |", player->score);
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

// 로직
void initializeBlockSetting(Player *player)
{
    // 블럭 모양 설정
    // player->shape = rand() % BLOCK_NUMBER;
    player->next_shape = rand() % BLOCK_NUMBER;
    player->rotation = 0;
    player->block_x = 10;
    player->block_y = 4;
}

// 뷰
void showCurrentnNextBlock(Player *player)
{
    showBlock(TRUE,player);
    showNextBlock(TRUE,player);
}

// 로직
BOOL checkGameOver(Player *player)
{
    if (checkSpace(player->block_x, player->block_y, player->rotation,player)==FALSE) {
        return TRUE;
    }
    return FALSE;
}

// 뷰
void gameStart(int nplayer, Player *player)
{
    BOOL check;
    // 블럭 한칸씩 아래로 이동
    if (--player->stay_time == 0) {
        player->stay_time = player->frame_time;
        // 로직
        check = checkNextSpace(player);

        if (check == FALSE) { // 이동 중 더이상 내려갈 공간 없다 -> break
            if (testFull(player) == FALSE) {
             // if next stage
             if (checkNextStage(player)) {
                initializeNextStage(player);
                initializeBoard(player);
                copyGameBoard(player);
                refreshSideBoard(player);
                drawBoard(player);
                showNextStage(player);
             }
             // if not next stage
             refreshSideBoard(player);
             drawBoard(player);
             copyGameBoard(player);
            }
            // saveBlockinBoard(nplayer, &player[nplayer]);
            showNextBlock(FALSE,player);
            player->shape = player->next_shape;
            initializeBlockSetting(player);
            showCurrentnNextBlock(player);


            if (checkGameOver(player)) {
                // 뷰
                showGameOver(nplayer, player);
                initializeBoard(player);
                drawCompleteBoard(player);
                copyGameBoard(player);
                showNextBlock(TRUE,player);
            }
        }

        moveBlock(player);
    }

    // 키보드 입력 읽기
    // 로직
    if (keyInput(nplayer, player) == FALSE) { // 입력 도중 더이상 내려갈 공간 없다 -> break
        if (testFull(player)) {
             // if next stage
             if (checkNextStage(player)) {
                initializeNextStage(player);
                refreshSideBoard(player);
                initializeBoard(player);
                drawBoard(player);
                copyGameBoard(player);
                showNextStage(player);
             }
             // if not next stage
             refreshSideBoard(player);
             drawBoard(player);
             copyGameBoard(player);
        }
        showNextBlock(FALSE, player);
        player->shape = player->next_shape;
        initializeBlockSetting(player);
        showCurrentnNextBlock(player);

        if(checkGameOver(player)) {
            // 뷰
            showGameOver(nplayer, player);
            initializeBoard(player);
            drawCompleteBoard(player);
            copyGameBoard(player);
            // 뷰
            showNextBlock(TRUE,player);
            player->stay_time = player->frame_time;
        }
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

// 로직
void initializeBoard(Player *player)
{
    int x,y;

     for (x = 0; x < BOARD_WIDTH+2; x++) {
        for (y = 0; y <  BOARD_HEIGHT+2; y++) {
            if (y == 0 || x == 0 || y == BOARD_HEIGHT+1 || x == BOARD_WIDTH+1) {
                player->game_board[x][y] = BOARD_FRAME;
            } else {
                player->game_board[x][y] = EMPTY;
            }
        }
     }

}

// 뷰
void drawCompleteBoard(Player *player)
{
    int x, y;
    system("cls");
    for (x = 0; x < BOARD_WIDTH+2; x++) {
        for (y = 0; y <  BOARD_HEIGHT+2; y++) {
            if (player->game_board[x][y] == BOARD_FRAME) {
                gotoXY(BOARD_START_X+x*(BLOCK_SHAPE_SIZE-1)+player->board_x,BOARD_START_Y+y);
                printf("%s", block_shape[BOARD_FRAME]);
            }
        }
    }

    gotoXY(BOARD_START_X+25+player->board_x,BOARD_START_Y);     printf("Stage: %6d", player->stage_level);
    gotoXY(BOARD_START_X+25+player->board_x,BOARD_START_Y+1);   printf("Score: %6d", player->score);
    gotoXY(BOARD_START_X+25+player->board_x,BOARD_START_Y+2);   printf("Next Shape");
    gotoXY(BOARD_START_X+25+player->board_x,BOARD_START_Y+3);   printf("+============+");
    gotoXY(BOARD_START_X+25+player->board_x,BOARD_START_Y+4);   printf("|            |");
    gotoXY(BOARD_START_X+25+player->board_x,BOARD_START_Y+5);   printf("|            |");
    gotoXY(BOARD_START_X+25+player->board_x,BOARD_START_Y+6);   printf("|            |");
    gotoXY(BOARD_START_X+25+player->board_x,BOARD_START_Y+7);   printf("|            |");
    gotoXY(BOARD_START_X+25+player->board_x,BOARD_START_Y+8);   printf("+============+");
}

// 뷰
void drawBoard(Player *player)
{
    int x, y;
    for (x = 1; x < BOARD_WIDTH+1; x++) {
        for (y = 1; y <  BOARD_HEIGHT+1; y++) {
            if (player->game_board[x][y] == BLOCK) {
                gotoXY(player->board_x + BOARD_START_X+x*(BLOCK_SHAPE_SIZE-1),BOARD_START_Y+y);
                printf("%s", block_shape[BLOCK]);
            } else if (player->game_board[x][y] == FILLED_BLOCK && player->copy_game_board[x][y] != player->game_board[x][y]) {
                SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), player->shape + 10);
                gotoXY(player->board_x + BOARD_START_X+x*(BLOCK_SHAPE_SIZE-1),BOARD_START_Y+y);
                printf("%s", block_shape[FILLED_BLOCK]);
            } else if (player->game_board[x][y] == EMPTY) {
                gotoXY(player->board_x + BOARD_START_X+x*(BLOCK_SHAPE_SIZE-1),BOARD_START_Y+y);
                printf("%s", block_shape[EMPTY]);
            }
        }
    }
}
// 뷰

void showBlock(BOOL show, Player *player)
{
    int i;
    if (show == TRUE) {
        for (i = 0; i < 4; i++) {
            gotoXY(player->board_x+BOARD_START_X + block_shape_coordinate[player->shape][player->rotation][i].x*(BLOCK_SHAPE_SIZE-1) + player->block_x, BOARD_START_Y +block_shape_coordinate[player->shape][player->rotation][i].y + player->block_y);
            // set block color
            SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), player->shape + 10);

            printf("%s", block_shape[BLOCK]);
        }
    } else if (show == FALSE) {
        for (i = 0; i < 4; i++) {
            gotoXY(player->board_x+BOARD_START_X + block_shape_coordinate[player->shape][player->rotation][i].x*(BLOCK_SHAPE_SIZE-1) + player->block_x, BOARD_START_Y +block_shape_coordinate[player->shape][player->rotation][i].y + player->block_y);
            printf("%s", block_shape[EMPTY]);
        }
    }
}

// 로직
BOOL checkNextStage(Player *player)
{
    if (player->clear_cnt >= player->stage_clear_cnt) {
        return TRUE;
    }
    return FALSE;
}

// 로직
void initializeNextStage(Player *player)
{
    player->clear_cnt = 0;
    player->stage_level += 1;
    player->frame_time /= 2; // move speed increase
}

// 로직
BOOL checkSpace(int x, int y, int rotation, Player *player)
{
    int i;
    int block_temp_x = x;
    int block_temp_y = y;
    int temp_rotation = rotation;

    // set block color
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 15);

    for (i = 0; i < 4; i++) {
        if (player->game_board[(block_shape_coordinate[player->shape][temp_rotation][i].x*(BLOCK_SHAPE_SIZE-1)+block_temp_x)/2][block_shape_coordinate[player->shape][temp_rotation][i].y + block_temp_y] != EMPTY) {
            return FALSE;
        }
    }
    return TRUE;
}

// 로직
BOOL checkNextSpace(Player *player)
{
    if (checkSpace(player->block_x, player->block_y+1, player->rotation, player) == FALSE) {
        return FALSE;
    }
    return TRUE;
}

// 뷰
void moveBlock(Player *player)
{
    showBlock(FALSE,player);
    player->block_y += 1;
    showBlock(TRUE, player);
}

void saveBlockinBoard(Player *player)
{
    int i;

     for (i = 0; i < 4; i++) {
        player->game_board[(block_shape_coordinate[player->shape][player->rotation][i].x*(BLOCK_SHAPE_SIZE-1)+player->block_x)/2][block_shape_coordinate[player->shape][player->rotation][i].y + player->block_y] = FILLED_BLOCK;
     }
}
// 로직
BOOL testFull(Player *player)
{
     int i,x,y;
     int temp_y;
     int combo = 1;

     saveBlockinBoard(player);

     for (y = BOARD_HEIGHT; y > 1; y--) {
        for (x = 1; x < BOARD_WIDTH + 1; x++) {
            if (player->game_board[x][y] == EMPTY) {
                return TRUE;
            }
        }
        if (x == BOARD_WIDTH + 1) {
            // score
            player->score += combo * 100;
            combo++;

            // nextStage
            player->clear_cnt += 1;

            for (temp_y = y; temp_y > 1; temp_y--) {
                for (x = 1; x < BOARD_WIDTH + 1; x++) {
                    player->game_board[x][temp_y] = player->game_board[x][temp_y-1];
                }
            }
            y++;
        }
     }
     return FALSE;
}

BOOL keyInput(int nplayer, Player *player)
{
    int key;
    int temp_rotation;
    int key_input_frame;

    while (key_input_frame >= 0) {
        key_input_frame--;
    }
    key_input_frame = 100;
    if (nplayer == ONE) {
        if (GetAsyncKeyState(VK_UP) & 0x8000) {
            if (player->rotation + 1 > 3) {
                temp_rotation = 0;
            } else {
                temp_rotation = player->rotation + 1;
            }
            if (checkSpace(player->block_x, player->block_y, temp_rotation, player)) {
                showBlock(FALSE,player);
                player->rotation = temp_rotation;
                showBlock(TRUE,player);
            }
        } else if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
            if (checkSpace(player->block_x-NEXT_SPACE, player->block_y, player->rotation, player)) {
                showBlock(FALSE,player);
                player->block_x -= 2;
                showBlock(TRUE,player);
            }
        } else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
            if (checkSpace(player->block_x+NEXT_SPACE, player->block_y, player->rotation, player)) {
                showBlock(FALSE,player);
                player->block_x += 2;
                showBlock(TRUE,player);
            }
        } else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
            if (checkNextSpace(player)) {//checkSpace(player->block_x, player->block_y+1, player->rotation,nplayer,&player[nplayer])) {
                moveBlock(player);
            } else {
                return FALSE;
            }
        } else if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
            while(checkNextSpace(player)) {
                moveBlock(player);
            }
            return FALSE;
        }
    } else if (nplayer == TWO) {
        if (GetAsyncKeyState('T') & 0x8000) {
            if (player->rotation + 1 > 3) {
                temp_rotation = 0;
            } else {
                temp_rotation = player->rotation + 1;
            }
            if (checkSpace(player->block_x, player->block_y, temp_rotation,player)) {
                showBlock(FALSE,player);
                player->rotation = temp_rotation;
                showBlock(TRUE,player);
            }
        } else if (GetAsyncKeyState('F') & 0x8000) {
            if (checkSpace(player->block_x-NEXT_SPACE, player->block_y, player->rotation,player)) {
                showBlock(FALSE,player);
                player->block_x -= 2;
                showBlock(TRUE,player);
            }
        } else if (GetAsyncKeyState('H') & 0x8000) {
            if (checkSpace(player->block_x+NEXT_SPACE, player->block_y, player->rotation,player)) {
                showBlock(FALSE,player);
                player->block_x += 2;
                showBlock(TRUE,player);
            }
        } else if (GetAsyncKeyState('G') & 0x8000) {
            if (checkNextSpace(player)) {//checkSpace(player->block_x, player->block_y+1, player->rotation,nplayer,&player[nplayer])) {
                moveBlock(player);
            } else {
                return FALSE;
            }
        } else if (GetAsyncKeyState('A') & 0x8000) {
            while(checkNextSpace(player)) {
                moveBlock(player);
            }
            return FALSE;
        }
    }

    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
        system("cls");
        exit(0);
    }
    return TRUE;
}


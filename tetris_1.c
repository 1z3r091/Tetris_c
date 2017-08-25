#include <stdio.h>
#include <time.h>
#include <windows.h>

// Keyboard Button ASCII Code MACRO
#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define ESC 27

#define BX 5
#define BY 1
#define BW 10 // MAX board width
#define BH 20 // MAX board height

typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE; // setcursortype func enum
void setcursortype(CURSOR_TYPE c); // cursor on/off func
void gotoxy(int x, int y); // x,y - coordinate change func

void DrawScreen(); // draw board screen with control interface
void DrawBoard(); // draw board screen
BOOL ProcessKey(); // key input process handling
void PrintBrick(BOOL Show); // show brick on the board
int GetAround(int x,int y,int b,int r); // check EMPTY space around brick (returns EMPTY, BRICK, WALL)
BOOL MoveDown(); // move BRICK down (y-coordinate increases)
void TestFull(); // check whether line is full of BRICKs

// brick x,y - coordinate structure
struct Point {
     int x,y;
};

// brick shape structure [7][4][4] - 7 shapes, 4 rotated shapes, 4 x,y coordinates
struct Point Shape[][4][4]={
     { {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2}, {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2} },
     { {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1} },
     { {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1}, {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1} },
     { {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1}, {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1} },
     { {0,0,-1,0,1,0,-1,-1}, {0,0,0,-1,0,1,-1,1}, {0,0,-1,0,1,0,1,1}, {0,0,0,-1,0,1,1,-1} },
     { {0,0,1,0,-1,0,1,-1}, {0,0,0,1,0,-1,-1,-1}, {0,0,1,0,-1,0,-1,1}, {0,0,0,-1,0,1,1,1} },
     { {0,0,-1,0,1,0,0,1}, {0,0,0,-1,0,1,1,0}, {0,0,-1,0,1,0,0,-1}, {0,0,-1,0,0,-1,0,1} },
};

enum { EMPTY, BRICK, WALL }; // EMPTY = 0, BRICK = 1, WALL = 2
char *arTile[]={". ","¡á","¡à"}; // Tile shapes ( EMPTY -> . , BRICK -> ¡á, WALL -> ¡à)
int board[BW+2][BH+2]; // max width/height + 2 because of the board edges
int nx,ny; // current x,y - coordinate of brick
int brick,rot; // brick = shape, rot = rotated shape


void main()
{
     int nFrame, nStay; // used for controlling the frame and speed
     int x,y; // for loop variables

     setcursortype(NOCURSOR); // hide CURSOR
     srand((unsigned)time(NULL)); // random func initialize
     system("cls"); // clear DOS screen

     for (x=0;x<BW+2;x++) {
          for (y=0;y<BH+2;y++) {
              board[x][y] = (y==0 || y==BH+1 || x==0 || x==BW+1) ? WALL:EMPTY; // if x, y == 0 or BW/BH + 1, then WALL
          }
     }
     DrawScreen(); // draw board screen
     nFrame=20; // frame setting

     for (;1;) { // infinite loop
          brick= rand() % ((sizeof(Shape)/sizeof(Shape[0]))); // get a shape randomly
          nx=BW/2; // initial brick x - coordinate
          ny=3; // initial brick y - coordinate
          rot=0; // initial rotated shape
          PrintBrick(TRUE); // show brick on the board

          if (GetAround(nx,ny,brick,rot) != EMPTY) break; // if space around a brick has no EMPTY space then break
          nStay=nFrame; // time variable for how long a brick stays on a coordinate
          for (;2;) { // infinite loop
              if (--nStay == 0) { // if the time variable equals 0 (=if the staying time is over)
                   nStay=nFrame; // set time variable to frame value
                   if (MoveDown()) break; // move down the brick
              }
              if (ProcessKey()) break; // read key input
              Sleep(1000/20);
          }
     }
     system("cls");
     gotoxy(30,12);puts("G A M E  O V E R");
     setcursortype(NORMALCURSOR);
}

void setcursortype(CURSOR_TYPE c)
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

void gotoxy(int x, int y)
{
     COORD Cur;
     Cur.X=x;
     Cur.Y=y;
     SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),Cur);
}

void DrawScreen()
{
     int x,y;

     for (x=0;x<BW+2;x++) {
          for (y=0;y<BH+2;y++) {
              gotoxy(BX+x*2,BY+y);
              puts(arTile[board[x][y]]);
          }
     }

     gotoxy(50,3);puts("Tetris Ver 1.0");
     gotoxy(50,5);puts("ì¢Œìš°:ì´ë™, ìœ„:íšŒì „, ì•„ëž˜:ë‚´ë¦¼");
     gotoxy(50,6);puts("ê³µë°±:ì „ë¶€ ë‚´ë¦¼");
}

void DrawBoard()
{
     int x,y;

     for (x=1;x<BW+1;x++) {
          for (y=1;y<BH+1;y++) {
              gotoxy(BX+x*2,BY+y);
              puts(arTile[board[x][y]]);
          }
     }
}

BOOL ProcessKey()
{
     int ch,trot;

     if (kbhit()) {
          ch=getch();
          if (ch == 0xE0 || ch == 0) {
              ch=getch();
              switch (ch) {
              case LEFT:
                   if (GetAround(nx-1,ny,brick,rot) == EMPTY) {
                        PrintBrick(FALSE);
                        nx--;
                        PrintBrick(TRUE);
                   }
                   break;
              case RIGHT:
                   if (GetAround(nx+1,ny,brick,rot) == EMPTY) {
                        PrintBrick(FALSE);
                        nx++;
                        PrintBrick(TRUE);
                   }
                   break;
              case UP:
                   trot=(rot == 3 ? 0:rot+1);
                   if (GetAround(nx,ny,brick,trot) == EMPTY) {
                        PrintBrick(FALSE);
                        rot=trot;
                        PrintBrick(TRUE);
                   }
                   break;
              case DOWN:
                   if (MoveDown()) {
                        return TRUE;
                   }
                   break;
              }
          } else {
              switch (ch) {
              case ' ':
                   while(MoveDown()==FALSE) {;}
                   return TRUE;
              }
          }
     }
     return FALSE;
}

void PrintBrick(BOOL Show)
{
     int i;

     for (i=0;i<4;i++) {
          gotoxy(BX+(Shape[brick][rot][i].x+nx)*2,BY+Shape[brick][rot][i].y+ny);
          puts(arTile[Show ? BRICK:EMPTY]);
     }
}

int GetAround(int x,int y,int b,int r)
{
     int i,k=EMPTY;

     for (i=0;i<4;i++) {
          k=max(k,board[x+Shape[b][r][i].x][y+Shape[b][r][i].y]);
     }
     return k;
}

BOOL MoveDown()
{
     if (GetAround(nx,ny+1,brick,rot) != EMPTY) {
          TestFull();
          return TRUE;
     }
     PrintBrick(FALSE);
     ny++;
     PrintBrick(TRUE);
     return FALSE;
}

void TestFull()
{
     int i,x,y,ty;

     for (i=0;i<4;i++) {
          board[nx+Shape[brick][rot][i].x][ny+Shape[brick][rot][i].y]=BRICK;
     }

     for (y=1;y<BH+1;y++) {
          for (x=1;x<BW+1;x++) {
              if (board[x][y] != BRICK) break;
          }
          if (x == BW+1) {
              for (ty=y;ty>1;ty--) {
                   for (x=1;x<BW+1;x++) {
                        board[x][ty]=board[x][ty-1];
                   }
              }
              DrawBoard();
              Sleep(200);
          }
     }
}

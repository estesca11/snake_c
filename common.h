//main 및 ui 공통 헤더
#include<stdio.h>
#include<windows.h>
#include<conio.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>
#define LEFT 75 //방향키의 ASCII CODE 
#define RIGHT 77
#define UP 72
#define DOWN 80
#define WALL_START_X 0 //map SIZE 매크로 선언
#define WALL_START_Y 0
#define WALL_LAST_X 70
#define WALL_LAST_Y 28
void SetConsoleView(int option);
void gotoxy(int x, int y, int printEnabled, char* s);
void Cursor_Hide();
void drawMap(char* s);
void title();
void gameOVER(int x, int y);
void setColor(int color_index);
void score(int devmode, int score_c, int score_h, int level, int elapsedTime);
void status(int preX, int preY, int dir, int dev, int Length, int(*snakePos)[2]);


//ui rendering
#include"common.h"
extern int devMode;
extern int score_high;
extern FILE* score_saved;
int adminLogin();

void SetConsoleView(int option) { //콘솔 창 크기 및 제목 설정
	switch (option) {
	case 1:
		system("mode con:cols=72 lines=30");
		system("title Snake by choi");
		break;
	case 2:
		system("mode con:cols=72 lines=35");
		system("title Snake by choi");
		break;
	}
}

void Cursor_Hide() //커서 숨기기 
{
	CONSOLE_CURSOR_INFO cinfo = { 0, }; /* CONSOLE 앞에는 struct가 생략되어 있음, cinfo라는 이름으로 CONSOLE_CURSOR_INFO 구조체 선언 및
	{0,}으로 모든 멤버 -dwSize, bVisible- 를 0으로 초기화*/
	cinfo.dwSize = 1;
	cinfo.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cinfo);
}

void setColor(int color_index) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color_index);
}
void gotoxy(int x, int y, int printEnabled, char* s) { // winapi를 통한 커서 이동 및 문자열 포인터를 인자로 받아 해당 위치에서 출력
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
	if (printEnabled == 1)printf("%s", s);
}

void drawMap(char* s) { // 테두리 그리기
	system("cls");
	setColor(9);
	for (int i = WALL_START_Y; i <= WALL_LAST_Y; i++) {
		if (i == WALL_START_Y || i == WALL_LAST_Y) {
			for (int j = WALL_START_X; j <= WALL_LAST_X / 2; j++) {
				printf("%s", s);
			}

		}
		else {
			for (int j = WALL_START_X; j < WALL_LAST_X; j++) {
				if (j == WALL_START_X || j == WALL_LAST_X - 1)printf("%s", s);
				else printf(" ");
			}
		}
		printf("\n");
	}
	setColor(7);
}

void status(int preX, int preY, int dir, int dev, int Length, int(*snakePos)[2]) { //좌표 출력부
	char direction[4] = { 'U','D','L','R' };
	setColor(3);
	gotoxy(WALL_START_X + 40, WALL_LAST_Y + 2, 1, "-------DEBUG MODE:");
	if (dev == 1)printf("ENABLED"); else printf("DISABLED");
	gotoxy(WALL_START_X + 40, WALL_LAST_Y + 3, 1, "                              ");
	gotoxy(WALL_START_X + 40, WALL_LAST_Y + 3, 0, "");
	printf("head=(%d, %d) tail=(%d, %d)", snakePos[0][0], snakePos[0][1], snakePos[Length - 1][0], snakePos[Length - 1][1]);
	gotoxy(WALL_START_X + 40, WALL_LAST_Y + 4, 0, "");
	printf("length=%d  dir:%c  pre=(%d, %d)", Length, direction[dir], preX, preY);
	setColor(15);
}

void score(int devmode, int score_c, int score_h, int level, int elapsedTime) { //점수출력부
	setColor(15);
	gotoxy(WALL_START_X + 22, WALL_LAST_Y + 2, 1, "ESC → Main");
	setColor(4);
	gotoxy(WALL_START_X, WALL_LAST_Y + 2, 1, "===ⓛⓔⓥⓔⓛ:");
	printf("%d", level - 1);
	setColor(14);
	gotoxy(WALL_START_X, WALL_LAST_Y + 3, 1, "===ⓢⓒⓞⓡⓔ:");
	printf("%d", 10 * score_c);
	setColor(11);
	gotoxy(WALL_START_X, WALL_LAST_Y + 4, 1, "===ⓗⓘⓖⓗ:");
	printf("%d", 10 * score_h);
	setColor(10);
	gotoxy(WALL_START_X, WALL_LAST_Y + 5, 1, "===ⓣⓘⓜⓔ:");
	printf("%d", elapsedTime / 1000);
}

void title() { //게임 시작 전까지 메뉴 표시
	SetConsoleView(1); //콘솔창 설정
	system("cls");
	drawMap("■");
	int x = 8, y = 4; //타이틀 위치 설정
	int cursorIndex = 0, escapeloop = 0;
	int cusorPos[4][2] = { {x + 20,y + 11},{x + 20,y + 13},{x + 20,y + 15},{x + 20,y + 17} };
	setColor(12);
	gotoxy(x, y - 1, 1, "========================================================");
	setColor(14);
	gotoxy(x, y, 1, "$$$$$$$     $$    $         $         $   $$     $$$$$$$\n");
	gotoxy(x, y + 1, 1, "$           $ $   $        $ $        $  $$      $\n");
	gotoxy(x, y + 2, 1, "$$$$$$$     $  $  $       $$$$$       $$$        $$$$$$$\n");
	gotoxy(x, y + 3, 1, "      $     $   $ $      $     $      $  $$      $\n");
	gotoxy(x, y + 4, 1, "$$$$$$$     $    $$     $       $     $   $$     $$$$$$$\n");
	setColor(12);
	gotoxy(x, y + 5, 1, "========================================================");
	setColor(5);
	gotoxy(x + 23, y + 11, 1, "GAME  START");
	gotoxy(x + 23, y + 13, 1, "SAVE  SCORE");
	gotoxy(x + 23, y + 15, 1, "DEBUG  MODE");
	gotoxy(x + 23, y + 17, 1, "EXIT   GAME");
	setColor(15);
	gotoxy(cusorPos[cursorIndex][0], cusorPos[cursorIndex][1], 1, "▶");
	while (escapeloop == 0) { //ENTER 입력까지 반복
		if (_kbhit()) {
			int keyDown = _getch();
			gotoxy(cusorPos[cursorIndex][0], cusorPos[cursorIndex][1], 1, "  ");
			switch (keyDown) { //이중 switch 문으로 cursorIndex가 위치하는 곳의 명령 실행
			case 13:
				switch (cursorIndex) {
				case 0:
					devMode = 0;
					escapeloop = 1;
					break;
				case 1:
					fprintf_s(score_saved, "%d", score_high);
					gotoxy(x + 23, y + 5, 1, "SCORE SAVED");
					break;
				case 2:
					if (adminLogin() == 1) {
						devMode = 1;
						escapeloop = 1;
						break;
					}
					else {
						setColor(0);
						gotoxy(24, 12, 1, "                                       ");
						setColor(138);
						gotoxy(28, 12, 1, "WRONG PASSWORD!!");
						setColor(5);
						gotoxy(x + 20, y + 15, 1, "                                         ");
						gotoxy(x + 23, y + 15, 1, "DEBUG  MODE");
					}
					break;
				case 3:
					exit(0);
					break;
				}
				break;
			case UP:
				if (cursorIndex > 0)
					cursorIndex--;
				else cursorIndex = 3;
				break;
			case DOWN:
				if (cursorIndex < 3)
					cursorIndex++;
				else cursorIndex = 0;
				break;
			default:
				break;
			}
		}
		setColor(15);
		gotoxy(cusorPos[cursorIndex][0], cusorPos[cursorIndex][1], 1, "▶");
		Sleep(1);
	}
	SetConsoleView(2);
	system("cls");
}

int adminLogin() {
	char pw[8] = "1234567";
	char pwinput[8];
	int i = 0, key = 0;
	setColor(138);
	gotoxy(24, 12, 1, "INPUT ADMIN PASSWORD : ");
	for (int i = 0; i < 8; i++) {
		if (i == 7)pwinput[i] = '\0';
		else {
			pwinput[i] = _getch();
			putchar('*');
		}
	}
	if (strcmp(pw, pwinput) == 0)
		return 1;
	return 0;
}

void gameOVER(int x, int y) { //게임오버 팝업 출력
	int reset = 0;
	setColor(4);
	gotoxy(x, y, 1, "▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣");
	gotoxy(x, y + 1, 1, "▣       ---------------        ▣");
	gotoxy(x, y + 2, 1, "▣       - GAME   OVER -        ▣");
	gotoxy(x, y + 3, 1, "▣       ---------------        ▣");
	gotoxy(x, y + 4, 1, "▣        Q : Main              ▣");
	gotoxy(x, y + 5, 1, "▣        R : Restart           ▣");
	gotoxy(x, y + 6, 1, "▣                              ▣");
	gotoxy(x, y + 7, 1, "▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣");
	setColor(7);
	while (reset == 0) { //reset 변수를 이용하여 while 탈출
		switch (_getch()) {
		case 82:
		case 114:
			reset = 1;
			break;
		case 81:
		case 113:
			title();
			reset = 1;
			break;
		}
	}
}


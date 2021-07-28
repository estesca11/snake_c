#include"common.h"
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
//main 지역 함수 선언
void snakeMove(int(*snakePos)[2], int headDirection, int stage, int* crash);
void setDirection(int key, int* headDirection, int(*snakePos)[2]);
void prey(int(*snakePos)[2], int level);
int Input(int* dir, int(*snake)[2]);
//main 지역 변수 선언
int dirWeight[4][2] = { {0,-1},{0,1},{-2,0},{2,0} }; //뱀의 이동방향 조절을 위한 가중치값, 인덱스 순서대로 상하좌우, ■는 공백 2개의 크기이므로 x축 가중치는 2로 적용 
int preyPos[100][2]; //먹이의 위치를 저장할 배열
int snakeLength; //뱀의 길이
int score_high = 0;
int score_current = 0;
int SPEED = 50;
int devMode = 0;
int preyNum = 1;
FILE* score_saved;
int main(void) {
	errno_t fileerr = fopen_s(&score_saved, "score.txt", "r+"); //읽기&쓰기 모드로 점수 불러오기
	if (fileerr == 0) {
		fscanf_s(score_saved, "%d", &score_high); //최고점수 파일스트림 포인터에 저장
		fseek(score_saved, 0, SEEK_SET); //값을 덮어쓰기 위해 포인터 시작점으로 이동
	}
	else fopen_s(&score_saved, "score.txt", "w"); //score.txt 없으면 생성
	Cursor_Hide(); //커서 숨기기
	title(); //ui.c에서 title 출력
	for (;;) { //게임 초기화를 위한 루프
		drawMap("▩"); //ui.c에서 맵 테두리 출력
		clock_t previous = 0, init_T = clock(); //지연 없는 입력을 위한 시간 비교
		int headDirection = 3; //시작 시 머리의 방향은 오른쪽
		score_current = 0;
		int crash = 0; //충돌 판정
		int stage = 1; //단계
		int snake[200][2] = { {0,} }; //뱀의 위치 정보
		int timer = 0;
		snakeLength = 5; //뱀의 초기 길이 설정
		for (int i = 0; i < snakeLength; i++) { //뱀의 초기 위치 설정 
			snake[i][0] = 30 - 2 * i;
			snake[i][1] = 14;
		}
		for (int i = 0; i < preyNum; i++) { //먹이 배열의 초기화
			preyPos[i][0] = 0;
			preyPos[i][1] = 0;
		}
		// 게임 루프의 시작
		while (1) {
			clock_t current = clock();
			timer = current - init_T;
			int preySum = 0; //먹이 좌표의 합
			for (int i = 0; i < 1; i++) { //먹이를 다 먹었는지 검사하기 위해 먹이좌표의 합을 계산
				preySum += preyPos[i][0] + preyPos[i][1];
			}
			if (preySum == 0) { //맵에 먹이가 남아있는지 확인 후 없으면 생성
				prey(snake, stage);
				stage++;
			}
			if (Input(&headDirection, snake) == 1)break;; //유저 입력 처리
			if (current - previous > SPEED) { //시간 차가 SPEED 이상일때 출력
				snakeMove(snake, headDirection, stage, &crash); //초기값이나 입력값을 반영하여 움직임 시작, 참조에 의한 전달로 현재 위치의 변화를 수정함
				previous = current;
			}
			if (score_current >= score_high)score_high = score_current; //최고점수 갱신
			score(devMode, score_current, score_high, stage, timer); //ui.c에서 점수 출력
			if (crash == 1 && devMode == 0) {
				gameOVER(20, 8); //(x, y)에 gameOVER 팝업 표시
				break;
			}
			Sleep(1); //딜레이를 통한 루프 연산 감소
		}
	}
	return 0;
}

int Input(int* dir, int(*snake)[2]) {
	if (_kbhit()) { //플레이어 입력 처리
		int key = _getch(); //방향키의 아스키 코드는 224 72 의 형태로 두개의 값이 입력되기 때문에 한번 더 값을 가져옴 
		if (key == 224) {
			key = _getch();
			setDirection(key, dir, snake); //머리의 방향 설정
			return 0;
		}
		else if (key == 27) {
			title();
			return 1;
		}
	}
	return 0;
}

void snakeMove(int(*snakePos)[2], int headDirection, int stage, int* crash) { //뱀의 움직임, 참조에 의한 위치 전달 
	int headpreX = snakePos[0][0] + dirWeight[headDirection][0]; //이동방향에 따른 한 칸 후의 머리 좌표
	int headpreY = snakePos[0][1] + dirWeight[headDirection][1];
	status(headpreX, headpreY, headDirection, devMode, snakeLength, snakePos); //위치정보 출력을 위한 인자 전달
	for (int i = (snakeLength - 1); i > 0; i--) { //몸통과의 충돌 판정
		if ((headpreX == snakePos[i][0]) && (headpreY == snakePos[i][1]) && devMode == 0)
		{
			*crash = 1;
			return;
		}
	}
	//벽과의 충돌 판정
	if (headpreX <WALL_START_X + 2 || headpreX > WALL_LAST_X - 2 || headpreY < WALL_START_Y + 1 || headpreY > WALL_LAST_Y - 1) {
		*crash = 1; //crash 변수를 통해 루프 탈출
		return;
	}
	else {
		for (int i = 0; i < preyNum; i++) {  //먹이와의 충돌 확인
			if (headpreX == preyPos[i][0] && headpreY == preyPos[i][1]) {
				preyPos[i][0] = 0;
				preyPos[i][1] = 0;
				score_current++;
				snakeLength++;
				PlaySound("coin.wav", 0, SND_FILENAME | SND_ASYNC);  //효과음 재생	
			}
			setColor(2);
			gotoxy(snakePos[0][0], snakePos[0][1], 1, "●"); //몸통 출력
			for (int i = snakeLength; i > 0; i--) { //Queue 구조, 몸통의 좌표=머리부터 한 칸씩 전의 좌표
				snakePos[i][0] = snakePos[i - 1][0];
				snakePos[i][1] = snakePos[i - 1][1];
			}
			snakePos[0][0] = headpreX;
			snakePos[0][1] = headpreY;
			gotoxy(snakePos[0][0], snakePos[0][1], 1, "◎"); //머리 출력
			gotoxy(snakePos[snakeLength][0], snakePos[snakeLength][1], 1, "  "); //몸통 끝을 지움
		}
		setColor(15);
	}
}

void setDirection(int key, int* headDirection, int(*snakePos)[2]) { //키 입력에 따른 진행 방향의 지정
	int headX, headY; //이동방향으로 진행할 경우 머리의 좌표 예상
	//180도 방향 전환 방지, 가중치가 반영되기 전에 반대 방향으로 변경되는 것 방지
	switch (key) {
	case UP:
		headY = snakePos[0][1] + dirWeight[0][1];
		if (headY != snakePos[1][1])
			*headDirection = 0;
		break;
	case DOWN:
		headY = snakePos[0][1] + dirWeight[1][1];
		if (headY != snakePos[1][1])
			*headDirection = 1;
		break;
	case LEFT:
		headX = snakePos[0][0] + dirWeight[2][0];
		if (headX != snakePos[1][0])
			*headDirection = 2;
		break;
	case RIGHT:
		headX = snakePos[0][0] + dirWeight[3][0];
		if (headX != snakePos[1][0])
			*headDirection = 3;
		break;
	default:
		break;
	}
}

void prey(int(*snakePos)[2], int level) { //먹이 생성
	setColor(6);
	while (1) {
		int preyonBody = 0; //몸통 위에 먹이 생성 여부 확인
		srand(clock()); //rand 함수의 seed를 초기화
		for (int i = 0; i < preyNum; i++) { //preyNum 만큼의 먹이 생성
			preyPos[i][0] = (2 * ((rand() % ((WALL_LAST_X - 2) / 2)) + 1));
			preyPos[i][1] = ((rand() % (WALL_LAST_Y - 1)) + 1);
			for (int j = 0; j <= snakeLength; j++) { //생성된 먹이의 좌표가 뱀의 좌표와 겹치는지 확인
				if ((snakePos[j][0] == preyPos[i][0]) && (snakePos[j][1] == preyPos[i][1])) {
					preyonBody = 1;
				}
			}
		}
		if (preyonBody == 1)continue; //뱀과 먹이의 좌표가 겹칠 경우 먹이좌표 재생성
		for (int i = 0; i < preyNum; i++) {
			gotoxy(preyPos[i][0], preyPos[i][1], 1, "★");
		}
		break;
	}
	setColor(7);
}


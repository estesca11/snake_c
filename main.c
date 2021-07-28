#include"common.h"
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
//main ���� �Լ� ����
void snakeMove(int(*snakePos)[2], int headDirection, int stage, int* crash);
void setDirection(int key, int* headDirection, int(*snakePos)[2]);
void prey(int(*snakePos)[2], int level);
int Input(int* dir, int(*snake)[2]);
//main ���� ���� ����
int dirWeight[4][2] = { {0,-1},{0,1},{-2,0},{2,0} }; //���� �̵����� ������ ���� ����ġ��, �ε��� ������� �����¿�, ��� ���� 2���� ũ���̹Ƿ� x�� ����ġ�� 2�� ���� 
int preyPos[100][2]; //������ ��ġ�� ������ �迭
int snakeLength; //���� ����
int score_high = 0;
int score_current = 0;
int SPEED = 50;
int devMode = 0;
int preyNum = 1;
FILE* score_saved;
int main(void) {
	errno_t fileerr = fopen_s(&score_saved, "score.txt", "r+"); //�б�&���� ���� ���� �ҷ�����
	if (fileerr == 0) {
		fscanf_s(score_saved, "%d", &score_high); //�ְ����� ���Ͻ�Ʈ�� �����Ϳ� ����
		fseek(score_saved, 0, SEEK_SET); //���� ����� ���� ������ ���������� �̵�
	}
	else fopen_s(&score_saved, "score.txt", "w"); //score.txt ������ ����
	Cursor_Hide(); //Ŀ�� �����
	title(); //ui.c���� title ���
	for (;;) { //���� �ʱ�ȭ�� ���� ����
		drawMap("��"); //ui.c���� �� �׵θ� ���
		clock_t previous = 0, init_T = clock(); //���� ���� �Է��� ���� �ð� ��
		int headDirection = 3; //���� �� �Ӹ��� ������ ������
		score_current = 0;
		int crash = 0; //�浹 ����
		int stage = 1; //�ܰ�
		int snake[200][2] = { {0,} }; //���� ��ġ ����
		int timer = 0;
		snakeLength = 5; //���� �ʱ� ���� ����
		for (int i = 0; i < snakeLength; i++) { //���� �ʱ� ��ġ ���� 
			snake[i][0] = 30 - 2 * i;
			snake[i][1] = 14;
		}
		for (int i = 0; i < preyNum; i++) { //���� �迭�� �ʱ�ȭ
			preyPos[i][0] = 0;
			preyPos[i][1] = 0;
		}
		// ���� ������ ����
		while (1) {
			clock_t current = clock();
			timer = current - init_T;
			int preySum = 0; //���� ��ǥ�� ��
			for (int i = 0; i < 1; i++) { //���̸� �� �Ծ����� �˻��ϱ� ���� ������ǥ�� ���� ���
				preySum += preyPos[i][0] + preyPos[i][1];
			}
			if (preySum == 0) { //�ʿ� ���̰� �����ִ��� Ȯ�� �� ������ ����
				prey(snake, stage);
				stage++;
			}
			if (Input(&headDirection, snake) == 1)break;; //���� �Է� ó��
			if (current - previous > SPEED) { //�ð� ���� SPEED �̻��϶� ���
				snakeMove(snake, headDirection, stage, &crash); //�ʱⰪ�̳� �Է°��� �ݿ��Ͽ� ������ ����, ������ ���� ���޷� ���� ��ġ�� ��ȭ�� ������
				previous = current;
			}
			if (score_current >= score_high)score_high = score_current; //�ְ����� ����
			score(devMode, score_current, score_high, stage, timer); //ui.c���� ���� ���
			if (crash == 1 && devMode == 0) {
				gameOVER(20, 8); //(x, y)�� gameOVER �˾� ǥ��
				break;
			}
			Sleep(1); //�����̸� ���� ���� ���� ����
		}
	}
	return 0;
}

int Input(int* dir, int(*snake)[2]) {
	if (_kbhit()) { //�÷��̾� �Է� ó��
		int key = _getch(); //����Ű�� �ƽ�Ű �ڵ�� 224 72 �� ���·� �ΰ��� ���� �ԷµǱ� ������ �ѹ� �� ���� ������ 
		if (key == 224) {
			key = _getch();
			setDirection(key, dir, snake); //�Ӹ��� ���� ����
			return 0;
		}
		else if (key == 27) {
			title();
			return 1;
		}
	}
	return 0;
}

void snakeMove(int(*snakePos)[2], int headDirection, int stage, int* crash) { //���� ������, ������ ���� ��ġ ���� 
	int headpreX = snakePos[0][0] + dirWeight[headDirection][0]; //�̵����⿡ ���� �� ĭ ���� �Ӹ� ��ǥ
	int headpreY = snakePos[0][1] + dirWeight[headDirection][1];
	status(headpreX, headpreY, headDirection, devMode, snakeLength, snakePos); //��ġ���� ����� ���� ���� ����
	for (int i = (snakeLength - 1); i > 0; i--) { //������� �浹 ����
		if ((headpreX == snakePos[i][0]) && (headpreY == snakePos[i][1]) && devMode == 0)
		{
			*crash = 1;
			return;
		}
	}
	//������ �浹 ����
	if (headpreX <WALL_START_X + 2 || headpreX > WALL_LAST_X - 2 || headpreY < WALL_START_Y + 1 || headpreY > WALL_LAST_Y - 1) {
		*crash = 1; //crash ������ ���� ���� Ż��
		return;
	}
	else {
		for (int i = 0; i < preyNum; i++) {  //���̿��� �浹 Ȯ��
			if (headpreX == preyPos[i][0] && headpreY == preyPos[i][1]) {
				preyPos[i][0] = 0;
				preyPos[i][1] = 0;
				score_current++;
				snakeLength++;
				PlaySound("coin.wav", 0, SND_FILENAME | SND_ASYNC);  //ȿ���� ���	
			}
			setColor(2);
			gotoxy(snakePos[0][0], snakePos[0][1], 1, "��"); //���� ���
			for (int i = snakeLength; i > 0; i--) { //Queue ����, ������ ��ǥ=�Ӹ����� �� ĭ�� ���� ��ǥ
				snakePos[i][0] = snakePos[i - 1][0];
				snakePos[i][1] = snakePos[i - 1][1];
			}
			snakePos[0][0] = headpreX;
			snakePos[0][1] = headpreY;
			gotoxy(snakePos[0][0], snakePos[0][1], 1, "��"); //�Ӹ� ���
			gotoxy(snakePos[snakeLength][0], snakePos[snakeLength][1], 1, "  "); //���� ���� ����
		}
		setColor(15);
	}
}

void setDirection(int key, int* headDirection, int(*snakePos)[2]) { //Ű �Է¿� ���� ���� ������ ����
	int headX, headY; //�̵��������� ������ ��� �Ӹ��� ��ǥ ����
	//180�� ���� ��ȯ ����, ����ġ�� �ݿ��Ǳ� ���� �ݴ� �������� ����Ǵ� �� ����
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

void prey(int(*snakePos)[2], int level) { //���� ����
	setColor(6);
	while (1) {
		int preyonBody = 0; //���� ���� ���� ���� ���� Ȯ��
		srand(clock()); //rand �Լ��� seed�� �ʱ�ȭ
		for (int i = 0; i < preyNum; i++) { //preyNum ��ŭ�� ���� ����
			preyPos[i][0] = (2 * ((rand() % ((WALL_LAST_X - 2) / 2)) + 1));
			preyPos[i][1] = ((rand() % (WALL_LAST_Y - 1)) + 1);
			for (int j = 0; j <= snakeLength; j++) { //������ ������ ��ǥ�� ���� ��ǥ�� ��ġ���� Ȯ��
				if ((snakePos[j][0] == preyPos[i][0]) && (snakePos[j][1] == preyPos[i][1])) {
					preyonBody = 1;
				}
			}
		}
		if (preyonBody == 1)continue; //��� ������ ��ǥ�� ��ĥ ��� ������ǥ �����
		for (int i = 0; i < preyNum; i++) {
			gotoxy(preyPos[i][0], preyPos[i][1], 1, "��");
		}
		break;
	}
	setColor(7);
}


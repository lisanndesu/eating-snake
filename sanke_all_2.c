#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <Windows.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#define WALL '|'
#define BODY '.'
#define FOOD 'o'

#define WALLX 58
#define WALLY 27

#define KEY_PRESS(VK)  ((GetAsyncKeyState(VK) & 0x1) ? 1 : 0)

enum GAME_STATUS {
	OK = 1,
	ESC,
	KILL_BY_WALL,
	KILL_BY_SELF,
};

enum DIRECTION {
	UP = 1,
	DOWN,
	RIGHT,
	LEFT
};

typedef struct SnakeNode {
	int x;
	int y;
	struct SnakeNode *next;
} SnakeNode, *pSnakeNode;

typedef struct Snake {
	struct SnakeNode *phead;
	struct SnakeNode *food_position;
	int food_weight;
	int sleep_time;
	int score;
	enum GAME_STATUS game_status;
	enum DIRECTION direction;
} Snake, *pSnake;

void InitSnake(struct Snake *snake);

void GamePrepare(Snake *psnake);

void InitSnakeBody(Snake *psnake);

SnakeNode *CreatFood(Snake *psnake);

int is_food_position(int x, int y, struct Snake *psnake);

void SetPos(int x, int y);

pSnakeNode CreatFood(Snake *psnake);

int is_dead(pSnake psnake);

void test();

//SnakeNode *CreatFood(Snake *psnake)



//extern void InitSnake(pSnake);


void Print(pSnake psnake) {
	//打印蛇

	SetPos(70, 10);
	printf("score: %d", psnake->score);
	SetPos(70, 11);
	printf("food_weight: %d", psnake->food_weight);
	SnakeNode *phead = psnake->phead;
	while (phead) {
		SetPos(phead->x, phead->y);
		printf("%c", BODY);
		phead = phead->next;
	}

	//打印食物
	SetPos(psnake->food_position->x, psnake->food_position->y);
	printf("%c", FOOD);

}

void pause() {
	while (!(KEY_PRESS(VK_SPACE))) {
		Sleep(200);
	}
}

void GetKey(pSnake ps) {
	if (KEY_PRESS(VK_UP) && ps->direction != DOWN) {
		ps->direction = UP;
	} else if (KEY_PRESS(VK_DOWN) && ps->direction != UP) {
		ps->direction = DOWN;
	} else if (KEY_PRESS(VK_LEFT) && ps->direction != RIGHT) {
		ps->direction = LEFT;
	} else if (KEY_PRESS(VK_RIGHT) && ps->direction != LEFT) {
		ps->direction = RIGHT;
	} else if (KEY_PRESS(VK_ESCAPE)) {
		ps->game_status = ESC;
	} else if (KEY_PRESS(VK_SPACE)) {
		//游戏要暂定
		pause();//暂定和回复暂定
	} else if (KEY_PRESS(VK_F3)) {
		if (ps->sleep_time >= 80) {
			ps->sleep_time -= 30;
			ps->food_weight += 2;
		}
	} else if (KEY_PRESS(VK_F4)) {
		if (ps->food_weight > 2) {
			ps->sleep_time += 30;
			ps->food_weight -= 2;
		}
	}
}

int NextX(pSnake psnake) {
	int add_x = 0;
	if (psnake->direction == LEFT) {
		add_x = -1;
	} else if (psnake->direction == RIGHT) {
		add_x = 1;
	}
	return psnake->phead->x + add_x;
}

int NextY(pSnake psnake) {
	int add_y = 0;
	if (psnake->direction == DOWN) {
		add_y = 1;
	} else if (psnake->direction == UP) {
		add_y = -1;
	}
	return psnake->phead->y + add_y;
}

void EatFood(pSnake psnake) {
	psnake->food_position->next = psnake->phead;
	psnake->phead = psnake->food_position;
	pSnakeNode new_food = CreatFood(psnake);
	psnake->food_position = new_food;
	psnake->score += psnake->food_weight;
}

void NotEatFood(pSnake psnake, pSnakeNode pnext) {
	pSnakeNode pcur = pnext;
	pnext->next = psnake->phead;
	psnake->phead = pnext;
	while (pcur->next->next) {
		pcur = pcur->next;
	}
	SetPos(pcur->next->x, pcur->next->y);
	printf("%c", ' ');
	free(pcur->next);
	pcur->next = NULL;
}

void Move(pSnake psnake) {

	pSnakeNode food = psnake->food_position;
	pSnakeNode pnext = (pSnakeNode)malloc(sizeof(SnakeNode));
	assert(pnext);
	pnext->next = NULL;
	pnext->x = NextX(psnake);
	pnext->y = NextY(psnake);
	if (pnext->x == food->x && pnext->y == food->y) {
		EatFood(psnake);
	} else {
		NotEatFood(psnake, pnext);
	}
}

void GameRun(pSnake psnake) {
	while (psnake->game_status == OK) {
		Print(psnake);		//加上总分与食物重量

		GetKey(psnake);

		Move(psnake);

		if (is_dead(psnake))
			return;

		Sleep(psnake->sleep_time);
	}

}











//#include "snake.h"

void CreatMap() {
	int i = 0;
	//上
	for (i = 0; i < WALLX; i += 2) {
		SetPos(0 + i, 0);
		printf("%s", "--");
	}
	//下
	SetPos(0, WALLY - 1);
	for (i = 0; i < WALLX; i += 2) {
		SetPos(0 + i, WALLY);
		printf("%s", "--");
	}
	//左
	for (i = 0; i < WALLY; i++) {
		SetPos(0, i);
		printf("%c", WALL);
	}
	//右
	for (i = 0; i < WALLY; i++) {
		SetPos(WALLX - 1, i);
		printf("%c", WALL);
	}

}

void SetPos(int x, int y) {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos = {x, y};
	SetConsoleCursorPosition(handle, pos);
}

void GamePrepare(Snake *psnake) {
	system("mode con cols=100 lines=30");

	//闂呮劘妫岄崗澶嬬垼
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(handle, &CursorInfo);

	CursorInfo.bVisible = false;
	SetConsoleCursorInfo(handle, &CursorInfo);

	system("title 贪吃蛇");
	SetPos(35, 10);
	printf("欢迎来到贪吃蛇小游戏\n");
	SetPos(38, 20);
	system("pause");
	system("cls");

	SetPos(15, 10);
	printf("你可以通过↑ ↓ ← → 控制贪吃蛇 \n");
	SetPos(15, 11);
	printf("F3加速，F4减速\n");
	printf("空格暂停\n");
	SetPos(38, 20);
	system("pause");
	system("cls");

	CreatMap();
}

void InitSnakeBody(Snake *psnake) {

	for (int i = 0; i < 5; i++) {
		pSnakeNode new_node = (pSnakeNode)malloc(sizeof(SnakeNode));
		new_node->x = 10 + i * 2;
		new_node->y = 10;
		new_node->next = NULL;
		if (i == 0) {
			psnake->phead = new_node;
		} else {
			new_node->next = psnake->phead;
			psnake->phead = new_node;
		}
	}
}

int is_food_position(int x, int y, struct Snake *psnake) {
	SnakeNode *phead = psnake->phead;
	while (phead) {
		if (phead->x == x && phead->y == y) {
			return 1;
		}
		phead = phead->next;
	}
	return 0;
}

SnakeNode *CreatFood(Snake *psnake) {
	int x, y;
	srand((unsigned)time(NULL));
	do {
		x = rand() % (WALLX - 2) + 1;
		y = rand() % (WALLY - 2) + 1;
	} while (is_food_position(x, y, psnake));
	SnakeNode *food = (SnakeNode *)malloc(sizeof(SnakeNode));
	assert(food);
	food->next = NULL;
	food->x = x;
	food->y = y;
	return food;
}

void InitSnake(Snake *psnake) {
	psnake->direction = RIGHT;
	InitSnakeBody(psnake);
	psnake->food_position = CreatFood(psnake);
	psnake->sleep_time = 200;
	psnake->food_weight = 10;
	psnake->score = 0;
	psnake->game_status = OK;
}

void Destory(pSnakeNode phead)
{
	pSnakeNode pcur = phead;
	while (pcur->next)
	{
		pSnakeNode del = pcur->next;
		pcur->next = del->next;
		free(del);
		del = NULL;
		pcur = pcur->next;
	}
}

void GameAfter(pSnake psnake) {
	char ch;
	char *reason = (char *)malloc(20 * sizeof(char));
	if (psnake->game_status == KILL_BY_WALL) {
		reason = "撞到墙";
	} else if (psnake->game_status == KILL_BY_SELF) {
		reason = "咬到自己";
	} else {
		reason = "摁下ESC";
	}

	SetPos(70, 20);
	printf("游戏结束，你因%s而死\t", reason);
	SetPos(70, 21);
	printf("得分：%d", psnake->score);
	SetPos(70, 22);
	system("pause");
	SetPos(70, 23);
	printf("摁下Y再来一局，其他键退出\n");
	SetPos(70, 24);
	scanf("%c", ch);
	if (ch == "Y")
//		goto again;
	Destory(psnake->phead);
}

int is_dead(pSnake psnake)
{
	pSnakeNode phead = psnake->phead;
	pSnakeNode pcur = phead ->next;
	if (phead->x >= WALLX || phead->x <= 0 || phead->y <= 0 || phead->y >= WALLY)
	{

		psnake->game_status = KILL_BY_WALL;
		return 1;
	}
	while (pcur)
	{
		if (pcur->x == phead->x && pcur->y == phead->y)
		{
			psnake->game_status = KILL_BY_SELF;
			return 1;
		}
		pcur = pcur->next;
	}
	return 0;
}

void test() {

	Snake snake;
	InitSnake(&snake);

	GamePrepare(&snake);

	GameRun(&snake);

	GameAfter(&snake);

	system("cls");
}



int main() {
	setlocale(LC_ALL, "");
again:
	test();
////	getchar();
//	SetPos(70, 20);
//	printf("gameover");
	return 0;
}

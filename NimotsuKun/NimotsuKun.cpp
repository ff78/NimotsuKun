// NimotsuKun.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "stdio.h"
#include <conio.h>
#include <cstdlib>
#include <sstream>
#include <ctime>
#include "NimotsuKun.h"

using namespace std;

#define random(a,b) (rand()%(b-a)+a)

enum MOVE_DIR
{
	DIR_NONE,
	DIR_UP,
	DIR_RIGHT,
	DIR_DOWN,
	DIR_LEFT,
	DIR_MAX
};

enum BLOCK_LOGIC_VALUE
{
	BLOCK_NONE,
	BLOCK_WALL,
	BLOCK_BOX,
	BLOCK_PLAYER,
	BLOCK_FLAG,
	BLOCK_PLAYER_ARRIVE,
	BLOCK_BOX_ARRIVE,
	BLOCK_MAX
};

MOVE_DIR stepDir;

int moveOffsetX[DIR_MAX] = {0,0,1,0,-1};
int moveOffsetY[DIR_MAX] = {0,-1,0,1,0};

typedef struct ActorPos
{
	int x = 0;
	int y = 0;
}ActorPos;

const int BoxCount = 2;
const int AreaSize = 12;

ActorPos playerPos;
ActorPos flagPos[BoxCount];
ActorPos boxPos[BoxCount];

int mapLogicValue[AreaSize][AreaSize];

int main()
{
	initMap();
	while (true)
	{
		updateGame();
		draw();
		getInput();
	}
	return 0;
}

void getInput()
{
	char c;
	c = _getch();
	//cout << "Input char is " << c << endl;
	// 输入与逻辑方向对应关系
	switch (c)
	{
	case 'a':
		stepDir = DIR_LEFT;
		break;
	case 'A':
		stepDir = DIR_LEFT;
		break;
	case 'w':
		stepDir = DIR_UP;
		break;
	case 'W':
		stepDir = DIR_UP;
		break;
	case 'd':
		stepDir = DIR_RIGHT;
		break;
	case 'D':
		stepDir = DIR_RIGHT;
		break;
	case 's':
		stepDir = DIR_DOWN;
		break;
	case 'S':
		stepDir = DIR_DOWN;
		break;
	default:
		stepDir = DIR_NONE;
		break;
	}

	//cout << "player move dir : " << stepDir << endl;
}

void updateGame()
{
	cout << "player move dir : " << stepDir << endl;

}

void draw()
{
	//刷新
	system("cls");
	// string tempLine = "";
	//全部重绘
	for (size_t i = 0; i < AreaSize; i++)
	{
		std::stringstream ss;
		for (size_t j = 0; j < AreaSize; j++)
		{
			char gridChar;
			//逻辑表现对应关系
			switch (mapLogicValue[i][j])
			{
			case BLOCK_WALL:
				gridChar = '#';
				break;
			case BLOCK_FLAG:
				gridChar = '.';
				break;
			case BLOCK_BOX:
				gridChar = 'o';
				break;
			case BLOCK_BOX_ARRIVE:
				gridChar = 'O';
				break;
			case BLOCK_PLAYER:
				gridChar = 'p';
				break;
			case BLOCK_PLAYER_ARRIVE:
				gridChar = 'P';
				break;
			
			default:
				gridChar = ' ';
				break;
			}
			ss << gridChar;
			// tempLine = ss.str();
		}
		cout << ss.str() << endl;
	}
	
}

void initMap()
{
	// 随机地图
	srand((int)time(0));
	// 刷围墙
	for (size_t i = 0; i < AreaSize; i++)
	{
		mapLogicValue[0][i] = BLOCK_WALL;
		mapLogicValue[AreaSize-1][i] = BLOCK_WALL;
		mapLogicValue[i][0] = BLOCK_WALL;
		mapLogicValue[i][AreaSize - 1] = BLOCK_WALL;
		for (size_t j = 1; j < AreaSize-1; j++)
		{
			if (i == 0 || i == AreaSize -1)
			{	
				break;
			}

			mapLogicValue[i][j] = BLOCK_NONE;
			
		}
		
	}

	int innerSize = AreaSize - 2;

	int x = 0;
	int y = 0;
	// 放目标点
	for (size_t i = 0; i < BoxCount; i++)
	{
		do
		{
			int randX = random(0, innerSize)+1;
			int randY = random(0, innerSize)+1;
			
			if(!checkFlag(randX, randY)) {
				x = randX;
				y = randY;
				break;
			}

		} while (true);
		
		flagPos[i].x = x;
		flagPos[i].y = y;
		mapLogicValue[y][x] = BLOCK_FLAG;
	}

	x = 0;
	y = 0;
	// 放箱子
	for (size_t i = 0; i < BoxCount; i++)
	{
		do
		{
			// 边缘留一条过道，避免开始时箱子靠边的死局
			int randX = random(1, innerSize-1)+1;
			int randY = random(1, innerSize-1)+1;
			if(!checkFlag(randX, randY) && !checkBox(randX, randY)) {
				x = randX;
				y = randY;
				break;
			}

		} while (true);
		
		boxPos[i].x = x;
		boxPos[i].y = y;
		mapLogicValue[y][x] = BLOCK_BOX;
	}

	// 避开箱子放玩家
	do
	{
		int randX = random(0, innerSize)+1;
		int randY = random(0, innerSize)+1;
		if(!checkBox(randX, randY)) {
			x = randX;
			y = randY;
			break;
		}

	} while (true);
	playerPos.x = x;
	playerPos.y = y;
	
	// 玩家可以出生在目标点上
	mapLogicValue[y][x] = checkFlag(x, y)?BLOCK_PLAYER_ARRIVE:BLOCK_PLAYER;

}

bool checkFlag(int checkPosX, int checkPosY)
{
	for (size_t i = 0; i < BoxCount; i++)
	{
		if(flagPos[i].x == checkPosX && flagPos[i].y == checkPosY)
		{
			return true;
		}
	}
	
	return false;
}

bool checkBox(int checkPosX, int checkPosY)
{
	for (size_t i = 0; i < BoxCount; i++)
	{
		if(boxPos[i].x == checkPosX && boxPos[i].y == checkPosY)
		{
			return true;
		}
	}
	
	return false;
}

bool checkWall(int checkPosX, int checkPosY)
{

	if (checkPosX == 0 || checkPosX == AreaSize)
	{
		return true;
	}

	if (checkPosY == 0 || checkPosY == AreaSize)
	{
		return true;
	}
	
	return false;
}

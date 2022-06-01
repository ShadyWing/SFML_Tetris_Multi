#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream>
using namespace std;
using namespace sf;

#define GRIDSIZE			35
#define STAGE_WIDTH			10
#define STAGE_HEIGHT		20

#define P1_STAGE_CORNER_X	156
#define P1_STAGE_CORNER_Y	174
#define P2_STAGE_CORNER_X	844
#define P2_STAGE_CORNER_Y	174

#define P1_NEXT_CORNER_X	587
#define P1_NEXT_CORNER_Y	125
#define P2_NEXT_CORNER_X	702
#define P2_NEXT_CORNER_Y	125

#define HOLD_CORNER_X		660
#define HOLD_CORNER_Y		275

#define DELAYVALUE			0.5


typedef enum PLAYROLE 
{
	roleNONE,
	rolePLAYER1,
	rolePLAYER2
};

typedef enum gridShape 
{
	shapeI,
	shapeS,
	shapeZ,
	shapeT,
	shapeL,
	shapej,
	shapeO
};

class Tetris
{
public:
	Tetris();
	~Tetris();


	RenderWindow* window;

	// 绘制区域
	Vector2i mCornPoint,
		nextSquareCornPoint,
		holdSquareCornPoint;

	// hold区域
	static int holdColorNum, holdShapeNum;
	static Vector2i holdSquare[4];	// hold区域的方块

	int gridsize;
	int imgBGno, imgSkinNo;
	// 方块
	Texture* tTiles;
	Sprite sTiles;
	Texture tBackground, tButtons, tNum, tTimer, tCounter, tGameover;
	Sprite sBackground, sButtons, sNum, sTimer, sCounter, sGameover;

	int Field[STAGE_HEIGHT][STAGE_WIDTH] = { 0 };

	Vector2i currentSquare[4],	// 当前四个方块
		nextSquare[4],			// next区域的方块
		shadowSquare[4],		// 阴影
		tempSquare[4];			// 临时保存四个方块

	// 形状
	int Figures[7][4] =
	{
		1,3,5,7,	// I
		2,4,5,7,	// S
		3,4,5,6,	// Z
		3,4,5,7,	// T
		2,3,5,7,	// L
		3,5,6,7,	// J
		2,3,4,5,	// O
	};

	int dx;
	bool rotate, hold, hardDrop, newShapeFlag, animationFlag, gameover, dropNoSlowdown;
	int colorNum, nextColorNum, tempColorNum;
	int currentShapeNum, nextShapeNum, tempShapeNum;

	int b7array[7], b7Int;
	int Bag7();

	int animationRow[4];
	float animationCtrlValue;

	float timer, delay;
	int role;
	int score;



	// 方格纹理
	void Initial(Texture* tex);
	void Input(Event event);
	void Logic();
	void basicLogic();
	void Draw(RenderWindow* window);
	

	// 方块碰撞，撞为0，没撞为1；
	bool hitTest();
	void isWin();
	// 遍历检查、消行
	void checkLine();
	void clearLine();

	// hold区交换
	void holdFunc();
	// 重新生成方块
	void newShapeFunc();
	void animationFunc(int i);

	void xMove();
	void yMove();
	void Rotate();
	// 速降
	void hardDropFunc();

	// 把currentSquare恢复到backup
	void backupRecovery();

	// 下落缓冲
	void slowLanding();
	// 画影子
	void updateShadow();
};
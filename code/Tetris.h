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

	// ��������
	Vector2i mCornPoint,
		nextSquareCornPoint,
		holdSquareCornPoint;

	// hold����
	static int holdColorNum, holdShapeNum;
	static Vector2i holdSquare[4];	// hold����ķ���

	int gridsize;
	int imgBGno, imgSkinNo;
	// ����
	Texture* tTiles;
	Sprite sTiles;
	Texture tBackground, tButtons, tNum, tTimer, tCounter, tGameover;
	Sprite sBackground, sButtons, sNum, sTimer, sCounter, sGameover;

	int Field[STAGE_HEIGHT][STAGE_WIDTH] = { 0 };

	Vector2i currentSquare[4],	// ��ǰ�ĸ�����
		nextSquare[4],			// next����ķ���
		shadowSquare[4],		// ��Ӱ
		tempSquare[4];			// ��ʱ�����ĸ�����

	// ��״
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



	// ��������
	void Initial(Texture* tex);
	void Input(Event event);
	void Logic();
	void basicLogic();
	void Draw(RenderWindow* window);
	

	// ������ײ��ײΪ0��ûײΪ1��
	bool hitTest();
	void isWin();
	// ������顢����
	void checkLine();
	void clearLine();

	// hold������
	void holdFunc();
	// �������ɷ���
	void newShapeFunc();
	void animationFunc(int i);

	void xMove();
	void yMove();
	void Rotate();
	// �ٽ�
	void hardDropFunc();

	// ��currentSquare�ָ���backup
	void backupRecovery();

	// ���仺��
	void slowLanding();
	// ��Ӱ��
	void updateShadow();
};
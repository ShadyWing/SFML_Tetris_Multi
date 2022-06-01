#include "Tetris.h"
using namespace sf;

int Tetris::holdColorNum = 0, Tetris::holdShapeNum = 0;
Vector2i Tetris::holdSquare[4] = { {0,0},{0,0} ,{0,0} ,{0,0} };

Tetris::Tetris()
{
	dx = 0;
	rotate = false;
	colorNum = 1;
	timer = 0;
	delay = DELAYVALUE;
	b7Int = 0;
	dropNoSlowdown = false;
}

Tetris::~Tetris() {}


void Tetris::Initial(Texture* tex)
{
	tTiles = tex;
	dx = 0;
	rotate = 0;
	colorNum = 1;
	hardDrop = false;
	timer = 0;
	delay = DELAYVALUE;
	hold = false;
	holdSquareCornPoint = { HOLD_CORNER_X,HOLD_CORNER_Y };
	animationFlag = true;
	animationCtrlValue = 1.0;

	if (role == rolePLAYER1)
	{
		mCornPoint = { P1_STAGE_CORNER_X, P1_STAGE_CORNER_Y };
		nextSquareCornPoint = { P1_NEXT_CORNER_X ,P1_NEXT_CORNER_Y };
	}
	if (role == rolePLAYER2)
	{
		mCornPoint = { P2_STAGE_CORNER_X,P2_STAGE_CORNER_Y };
		nextSquareCornPoint = { P2_NEXT_CORNER_X ,P2_NEXT_CORNER_Y };
	}

	sTiles.setTexture(*tTiles);

	colorNum = rand() % 7 + 1;
	currentShapeNum = rand() % 7;
	newShapeFunc();

	holdShapeNum = -1;

	memset(b7array, 0, sizeof(int));

	for (int i = 0; i < 4; i++)
	{
		currentSquare[i].x = Figures[currentShapeNum][i] % 2 + STAGE_WIDTH / 2;
		currentSquare[i].y = Figures[currentShapeNum][i] / 2;
		nextSquare[i].x = Figures[nextShapeNum][i] % 2;
		nextSquare[i].y = Figures[nextShapeNum][i] / 2;

		animationRow[i] = -1;// -1为异常值
	}

	for (int i = 0; i < STAGE_HEIGHT; i++)
	{
		for (int j = 0; j < STAGE_WIDTH; j++)
		{
			Field[i][j] = 0;
		}
	}
}

void Tetris::Logic()
{
	if (!animationFlag)
		basicLogic();
	// 新方块生成请求
	if (newShapeFlag)
	{
		// 先判定是否有动画播放，再 new 新的方块
		if (animationFlag == false)
		{
			checkLine();
			if (animationFlag == false)
			{
				newShapeFunc();
				isWin();
			}
		}
		else
		{
			animationCtrlValue -= 0.1;
			if (animationCtrlValue < 0)
			{
				animationFlag = false;
				animationCtrlValue = 1.0;
				for (int i = 0; i < 4; i++)
					animationRow[i] = -1;
				clearLine();
				newShapeFunc();
			}
		}
	}
}

void Tetris::basicLogic()
{
	// hold
	if (hold)
	{
		holdFunc();
		hold = 0;
	}

	// 水平
	xMove();

	// 下落
	if (timer > delay)
	{
		yMove();
		timer = 0;
	}

	// 旋转
	if (rotate)
	{
		Rotate();
		rotate = 0;
	}

	updateShadow();

	// hardDrop与否的缓冲情况
	if (!dropNoSlowdown)
		slowLanding();
	else
	{
		delay = 0;
		dropNoSlowdown = false;
	}

	if (hardDrop)
	{
		hardDropFunc();
		hardDrop = 0;
		dropNoSlowdown = true;
	}

	// 遍历判断
	checkLine();
}

bool Tetris::hitTest()
{
	for (int i = 0; i < 4; i++)
	{
		if (currentSquare[i].x < 0 || currentSquare[i].x >= STAGE_WIDTH
			|| currentSquare[i].y >= STAGE_HEIGHT)
			return false;
		// 撞上固定块
		else if (Field[currentSquare[i].y][currentSquare[i].x])
			return false;
	}
	return true;
}

void Tetris::Input(Event event)
{
	if (role == rolePLAYER1)
	{
		if (event.type == Event::KeyPressed)
		{
			if (event.key.code == Keyboard::W)
				if (currentShapeNum != shapeO)
					rotate = 1;
			if (event.key.code == Keyboard::A)
				dx = -1;
			if (event.key.code == Keyboard::D)
				dx = 1;
			if (event.key.code == Keyboard::S);
				//delay = DELAYVALUE / 10;
		}
		if (event.type == Event::KeyReleased)
		{
			if (event.key.code == Keyboard::LControl)
				hold = true;
			if (event.key.code == Keyboard::Space)
				hardDrop = true;
			if (event.key.code == Keyboard::A || event.key.code == Keyboard::D)
				dx = 0;
			if (event.key.code == Keyboard::S)
				hardDrop = true;
				//delay = DELAYVALUE;
		}
	}
	else if (role == rolePLAYER2)
	{
		if (event.type == Event::KeyPressed)
		{
			if (event.key.code == Keyboard::Up)
				if (currentShapeNum != shapeO)
					rotate = 1;
			if (event.key.code == Keyboard::Left)
				dx = -1;
			if (event.key.code == Keyboard::Right)
				dx = 1;
			if (event.key.code == Keyboard::Down);
				//delay = DELAYVALUE;
		}
		if (event.type == Event::KeyReleased)
		{
			if (event.key.code == Keyboard::RControl)
				hold = true;
			if (event.key.code == Keyboard::Enter)
				hardDrop = true;
			if (event.key.code == Keyboard::Left || event.key.code == Keyboard::Right)
				dx = 0;
			if (event.key.code == Keyboard::Down)
				hardDrop = true;
				//delay = DELAYVALUE;
		}
	}
}

void Tetris::yMove()
{
	for (int i = 0; i < 4; i++)
	{
		tempSquare[i] = currentSquare[i];
		currentSquare[i].y++;
	}

	// 如果撞底
	if (!hitTest())
	{
		for (int i = 0; i < 4; i++)
			Field[tempSquare[i].y][tempSquare[i].x] = colorNum;
		newShapeFlag = 1;
	}
}

void Tetris::Rotate()
{
	int originalHeight = currentSquare[0].y;
	// wall kick 中心偏移遍历
	for (int j = 0; j < 4; j++)
	{
		// 旋转中心点
		Vector2i p = currentSquare[j];
		// 顺时针 90 度
		for (int i = 0; i < 4; i++)
		{
			int x = currentSquare[i].y - p.y;
			int y = currentSquare[i].x - p.x;
			currentSquare[i].x = p.x - x;
			currentSquare[i].y = p.y + y;
		}
		// 如果没撞上
		if (hitTest())
		{
			// 新老重心的高度差
			int detaY = currentSquare[0].y - originalHeight;
			// 对高度差进行修正
			if (detaY != 0)
				for (int i = 0; i < 4; i++)
					currentSquare[i].y -= detaY;
			// 如果撞上了
			if (!hitTest())
				backupRecovery();
			else
				break;
		}
		else
			backupRecovery();
	}
	rotate = 0;
}

void Tetris::checkLine()
{
	int k = STAGE_HEIGHT - 1;
	int yCount = 0;
	for (int i = STAGE_HEIGHT - 1; i > 0; i--)
	{
		int count = 0;
		for (int j = 0; j < STAGE_WIDTH; j++)
		{
			if (Field[i][j])
				count++;
		}
		if (count < STAGE_WIDTH)
			k--;
		else
		{
			// 播放动画的行数
			animationRow[yCount] = i;
			yCount++;
			animationFlag = true;
		}
	}
	switch (yCount)
	{
	case 1:
		score += 10;
		break;
	case 2:
		score += 30;
		break;
	case 3:
		score += 60;
		break;
	case 4:
		score += 100;
		break;
	}
}

void Tetris::Draw(RenderWindow* w)
{
	window = w;

	if (animationFlag == false)
	{
		// shadow 绘制
		for (int i = 0; i < 4; i++)
		{
			sTiles.setTextureRect(IntRect(colorNum * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
			sTiles.setPosition(shadowSquare[i].x * GRIDSIZE, shadowSquare[i].y * GRIDSIZE);
			sTiles.setColor(Color(50, 50, 50, 255));
			sTiles.move(mCornPoint.x, mCornPoint.y);
			window->draw(sTiles);
			sTiles.setColor(Color(255, 255, 255, 255));
		}
		// 绘制活动方块
		for (int i = 0; i < 4; i++)
		{
			sTiles.setTextureRect(IntRect(colorNum * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
			sTiles.setPosition(currentSquare[i].x * GRIDSIZE, currentSquare[i].y * GRIDSIZE);
			sTiles.move(mCornPoint.x, mCornPoint.y);
			window->draw(sTiles);
		}
	}
	// 绘制固定方块
	for (int i = 0; i < STAGE_HEIGHT; i++)
		if (i == animationRow[0] || i == animationRow[1] || i == animationRow[2] || i == animationRow[3])
			animationFunc(i);
		else
			for (int j = 0; j < STAGE_WIDTH; j++)
			{
				if (Field[i][j] == 0)
					continue;
				sTiles.setTextureRect(IntRect(Field[i][j] * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
				sTiles.setPosition(j * GRIDSIZE, i * GRIDSIZE);
				sTiles.move(mCornPoint.x, mCornPoint.y); //offset
				window->draw(sTiles);
			}

	// next区域 绘制
	for (int i = 0; i < 4; i++)
	{
		sTiles.setTextureRect(IntRect(nextColorNum * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
		sTiles.setPosition(nextSquare[i].x * GRIDSIZE, nextSquare[i].y * GRIDSIZE);
		sTiles.move(nextSquareCornPoint.x, nextSquareCornPoint.y);
		window->draw(sTiles);
	}

	// hold区域 绘制
	if (holdShapeNum > -1)
		for (int i = 0; i < 4; i++)
		{
			sTiles.setTextureRect(IntRect(holdColorNum * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
			sTiles.setPosition(holdSquare[i].x * GRIDSIZE, holdSquare[i].y * GRIDSIZE);
			sTiles.move(holdSquareCornPoint.x, holdSquareCornPoint.y);
			window->draw(sTiles);
		}
}

int Tetris::Bag7()
{
	int num;
	num = rand() % 7;
	for (int i = 0; i < b7Int; i++)
	{
		if (b7array[i] == num)
		{
			i = -1;
			num = rand() % 7;
		}
	}
	b7array[b7Int] = num;
	b7Int++;
	if (b7Int == 7)
	{
		b7Int = 0;
		for (int i = 0; i < 7; i++)
		{
			b7array[i] = 0;
		}
	}
	
	return	num;
}

void Tetris::holdFunc()
{
	Vector2i backUpSquare[4];
	tempColorNum = holdColorNum;
	tempShapeNum = holdShapeNum;

	holdColorNum = colorNum;
	holdShapeNum = currentShapeNum;

	for (int i = 0; i < 4; i++)
	{
		holdSquare[i].x = Figures[holdShapeNum][i] % 2;
		holdSquare[i].y = Figures[holdShapeNum][i] / 2;
		tempSquare[i].x = Figures[tempShapeNum][i] % 2;
		tempSquare[i].y = Figures[tempShapeNum][i] / 2;
		backUpSquare[i] = currentSquare[i];
	}
	if (tempShapeNum < 0)
		newShapeFunc();
	// 发生交换
	else
	{
		colorNum = tempColorNum;
		currentShapeNum = tempShapeNum;
		// 从 hold 区置换过来的方块图形，在舞台上坐标的计算
		int minCurrentX = currentSquare[0].x,
			minCurrentY = currentSquare[0].y,
			minTempX = tempSquare[0].x,
			minTempY = tempSquare[0].y;
		int dx, dy;
		for (int i = 1; i < 4; i++)
		{
			if (currentSquare[i].x < minCurrentX)
				minCurrentX = currentSquare[i].x;
			if (currentSquare[i].y < minCurrentY)
				minCurrentY = currentSquare[i].y;
			if (tempSquare[i].x < minTempX)
				minTempX = tempSquare[i].x;
			if (tempSquare[i].y < minTempY)
				minTempY = tempSquare[i].y;
		}
		dx = minCurrentX - minTempX;
		dy = minCurrentY - minTempY;
		for (int i = 0; i < 4; i++)
		{
			currentSquare[i].x = tempSquare[i].x + dx;
			currentSquare[i].y = tempSquare[i].y + dy;
			holdSquare[i].x = Figures[holdShapeNum][i] % 2;
			holdSquare[i].y = Figures[holdShapeNum][i] / 2;
		}

	}
	// 如果撞上了
	if (!hitTest())
	{
		colorNum = holdColorNum;
		holdColorNum = tempColorNum;
		holdShapeNum = tempShapeNum;
		for (int i = 0; i < 4; i++)
		{
			currentSquare[i] = backUpSquare[i];
			holdSquare[i].x = Figures[holdShapeNum][i] % 2;
			holdSquare[i].y = Figures[holdShapeNum][i] / 2;
		}
	}
}

void Tetris::xMove()
{
	for (int i = 0; i < 4; i++)
	{
		tempSquare[i] = currentSquare[i];
		currentSquare[i].x += dx;
	}
	// 撞上 恢复备份
	if (!hitTest())
		backupRecovery();
}

void Tetris::backupRecovery()
{
	for (int i = 0; i < 4; i++)
		currentSquare[i] = tempSquare[i];
}

void Tetris::slowLanding()
{
	for (int i = 0; i < 4; i++)
	{
		tempSquare[i] = currentSquare[i];
		currentSquare[i].y += 1;
	}
	if (!hitTest())
		delay = DELAYVALUE * 2;
	else
		delay = DELAYVALUE;
	backupRecovery();
}

void Tetris::hardDropFunc()
{
	for (int i = 0; i < 4; i++)
		currentSquare[i] = shadowSquare[i];
}

void Tetris::updateShadow()
{
	for (int i = 0; i < 4; i++)
		shadowSquare[i] = currentSquare[i];
	for (int i = 0; i < STAGE_HEIGHT; i++)
	{
		for (int j = 0; j < 4; j++)
			currentSquare[j].y += 1;
		if (!hitTest()) {
			for (int k = 0; k < 4; k++)
				currentSquare[k].y -= 1;
			break;
		}
	}
	for (int i = 0; i < 4; i++)
	{
		tempSquare[i] = currentSquare[i];
		currentSquare[i] = shadowSquare[i];
		shadowSquare[i] = tempSquare[i];
	}
}

void Tetris::clearLine()
{
	int k = STAGE_HEIGHT - 1;
	for (int i = STAGE_HEIGHT - 1; i > 0; i--)
	{
		int xCount = 0;
		for (int j = 0; j < STAGE_WIDTH; j++)
		{
			if (Field[i][j])
				xCount++;
			Field[k][j] = Field[i][j];
		}
		if (xCount < STAGE_WIDTH)
			k--;
	}
}

void Tetris::newShapeFunc()
{
	// 取下个方块图形
	colorNum = nextColorNum;
	currentShapeNum = nextShapeNum;

	// 更新下个方块图形
	nextColorNum = 1 + rand() % 7;
	nextShapeNum = Bag7();

	for (int i = 0; i < 4; i++)
	{
		currentSquare[i] = nextSquare[i];
		currentSquare[i].x = currentSquare[i].x + STAGE_WIDTH / 2;
		nextSquare[i].x = Figures[nextShapeNum][i] % 2;
		nextSquare[i].y = Figures[nextShapeNum][i] / 2;
	}

	// 生成形状时即更新影子，否则新形状的影子会先在老位置出现
	updateShadow();

	newShapeFlag = false;
}

void Tetris::animationFunc(int i)
{
	Vector2f p;
	sTiles.scale(animationCtrlValue, animationCtrlValue);
	p = sTiles.getOrigin();
	sTiles.setOrigin(GRIDSIZE / 2, GRIDSIZE / 2);
	sTiles.rotate(360 * animationCtrlValue);
	for (int j = 0; j < STAGE_WIDTH; j++)
	{
		sTiles.setTextureRect(IntRect(Field[i][j] * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
		sTiles.setPosition(j * GRIDSIZE, i * GRIDSIZE);
		sTiles.move(mCornPoint.x + GRIDSIZE / 2, mCornPoint.y + GRIDSIZE / 2); //offset
		window->draw(sTiles);
	}
	sTiles.scale(1.0 / animationCtrlValue, 1.0 / animationCtrlValue);
	sTiles.rotate(-360 * animationCtrlValue);
	sTiles.setOrigin(p);
}

void Tetris::isWin()
{
	if (Field[2][5] || Field[2][6])
		gameover = true;
}
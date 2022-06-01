#include "Game.h"

Game::Game()
{
	srand(time(NULL));
	imgBGno = 2;
	imgSkinNo = 3;
	imgSetNo = 1;
	window.create(VideoMode(Window_width, Window_height), L"俄罗斯方块 by PJJ");
	window.setFramerateLimit(15);
	window.setKeyRepeatEnabled(0);	//按键按下只响应一次
}

Game::~Game() {}


void Game::GameRun()
{
	while (!gameQuit && window.isOpen())
	{
		GameInitial();

		while (!gameOver && window.isOpen())
		{
			GameInput();

			if (isGameBegin == true)
				GameLogic();
			else
				GameInitial();

			GameDraw();
		}
	}
}

void Game::GameInitial()
{
	LoadMediaData();

	isGameBegin = false;
	isGameHold = false;
	ButtonState_Start = Start_Dark;
	ButtonState_Hold = Hold_Dark;

	player1.role = rolePLAYER1;
	player2.role = rolePLAYER2;
	player1.Initial(&tTiles);
	player2.Initial(&tTiles);
}

void Game::LoadMediaData()
{
	stringstream ss;
	ss << "data/images/bg" << imgSetNo << ".jpg";

	if (!tBackground.loadFromFile(ss.str()))
		cout << "BKimg未找到" << endl;

	ss.str("");
	ss << "data/images/tiles" << imgSetNo << ".jpg";
	if (!tTiles.loadFromFile(ss.str()))
		cout << "tiles.jpg未找到" << endl;

	if (!tFrame.loadFromFile("data/images/frame.png"))
		cout << "frame.png未找到" << endl;
	if (!tCover.loadFromFile("data/images/cover.png"))
		cout << "cover.png未找到" << endl;
	if (!tGameOver.loadFromFile("data/images/end.png"))
		cout << "end.png未找到" << endl;
	if (!tButtons.loadFromFile("data/images/button.png"))
		cout << "button.png 没有找到" << endl;
	if (!tSwitcher.loadFromFile("data/images/bgSwitch.png"))
		cout << "bgSwap.png 没有找到" << endl;

	sBackground.setTexture(tBackground);
	sFrame.setTexture(tFrame);
	sCover.setTexture(tCover);
	sGameOver.setTexture(tGameOver);
	sButtons.setTexture(tButtons);
	sSwitcher.setTexture(tSwitcher);
	sSwitcher.setOrigin(sSwitcher.getLocalBounds().width / 2.0, sSwitcher.getLocalBounds().height / 2.0);

	sGameOver.setScale(0.6, 0.6);

	if (!font.loadFromFile("data/Fonts/simsun.ttc"))
		std::cout << "字体没有找到" << std::endl;
	text.setFont(font);
}

void Game::GameInput()
{
	Event event;
	while (window.pollEvent(event))
	{
		if (event.type == Event::Closed)
		{
			window.close();
			gameQuit = 1;
		}

		if (event.type == Event::EventType::KeyReleased && event.key.code == Keyboard::Escape)
		{
			window.close();
			gameQuit = true;
		}
		if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Left)
		{
			if (ButtonRectStart.contains(event.mouseButton.x, event.mouseButton.y))
			{
				if (isGameBegin == false)
				{
					isGameBegin = true;
					// 初始化动画的状态
					player1.animationFlag = false;
					player2.animationFlag = false;
					ButtonState_Start = Close_Light;
				}
				else
				{
					isGameBegin = false;
					ButtonState_Start = Start_Light;

					player1.score = 0;
					player2.score = 0;
					player1.gameover = false;
					player2.gameover = false;
				}
			}
				
			if (ButtonRectHold.contains(event.mouseButton.x, event.mouseButton.y))
			{
				if (isGameHold == false)
				{
					isGameHold = true;
					ButtonState_Hold = Continue_Light;
				}
				else
				{
					isGameHold = false;
					ButtonState_Hold = Hold_Light;
				}
			}

			if (ButtonRectLeft.contains(event.mouseButton.x, event.mouseButton.y))
			{
				imgSetNo--;
				if (imgSetNo < 1)
					imgSetNo = 4;
				LoadMediaData();
			}

			if (ButtonRectRight.contains(event.mouseButton.x, event.mouseButton.y))
			{
				imgSetNo++;
				if (imgSetNo > 4)
					imgSetNo = 1;
				LoadMediaData();
			}
		}
		if (event.type == Event::MouseMoved)
		{
			if (ButtonRectStart.contains(event.mouseMove.x, event.mouseMove.y))
			{
				if (isGameBegin == false)
					ButtonState_Start = Start_Light;
				else
					ButtonState_Start = Close_Light;
			}
			else
			{
				if (isGameBegin == false)
					ButtonState_Start = Start_Dark;
				else
					ButtonState_Start = Close_Dark;
			}
				

			if (ButtonRectHold.contains(event.mouseMove.x, event.mouseMove.y))
			{
				if (isGameHold == false)
					ButtonState_Hold = Hold_Light;
				else
					ButtonState_Hold = Continue_Light;
			}
			else
			{
				if (isGameHold == false)
					ButtonState_Hold = Hold_Dark;
				else
					ButtonState_Hold = Continue_Dark;
			}
				
		}

		player1.Input(event);
		player2.Input(event);
	}
}

void Game::GameLogic()
{
	if (isGameHold == 1)
		return;
	float time = clock.getElapsedTime().asSeconds();
	clock.restart();
	player1.timer += time;
	player2.timer += time;

	if (player1.gameover == false)
		player1.Logic();
	if (player2.gameover == false)
		player2.Logic();
}

void Game::GameDraw()
{
	window.clear();

	window.draw(sBackground);
	window.draw(sFrame);
	player1.Draw(&window);
	player2.Draw(&window);

	sCover.setPosition(P1_STAGE_CORNER_X, P1_STAGE_CORNER_Y);
	window.draw(sCover);
	sCover.setPosition(P2_STAGE_CORNER_X, P2_STAGE_CORNER_Y);
	window.draw(sCover);

	DrawButton();

	DrawText();
	
	DrawResults();

	window.display();
}

void Game::DrawButton()
{
	int ButtonWidth, ButtonHeight;
	ButtonWidth = 110;
	ButtonHeight = sButtons.getLocalBounds().height;

	//ButtonRectStart
	sButtons.setTextureRect(IntRect(ButtonState_Start * ButtonWidth, 0, ButtonWidth, ButtonHeight));//读取按钮的纹理区域
	sButtons.setPosition(B_START_CORNER_X, B_START_CORNER_Y);
	ButtonRectStart.left = B_START_CORNER_X;
	ButtonRectStart.top = B_START_CORNER_Y;
	ButtonRectStart.width = ButtonWidth;
	ButtonRectStart.height = ButtonHeight;
	window.draw(sButtons);

	//ButtonRectHold
	sButtons.setTextureRect(IntRect(ButtonState_Hold * ButtonWidth, 0, ButtonWidth, ButtonHeight));//读取按钮的纹理区域
	sButtons.setPosition(B_HOLD_CORNER_X, B_HOLD_CORNER_Y);
	ButtonRectHold.left = B_HOLD_CORNER_X;
	ButtonRectHold.top = B_HOLD_CORNER_Y;
	ButtonRectHold.width = ButtonWidth;
	ButtonRectHold.height = ButtonHeight;
	window.draw(sButtons);

	//背景素材切换
	ButtonWidth = sSwitcher.getLocalBounds().width;
	ButtonHeight = sSwitcher.getLocalBounds().height;

	//ButtonRectLeft
	sSwitcher.setPosition(B_LEFT_CORNER_X, B_LEFT_CORNER_Y);
	ButtonRectLeft.left = B_LEFT_CORNER_X - ButtonWidth / 2;
	ButtonRectLeft.top = B_LEFT_CORNER_Y - ButtonHeight / 2;
	ButtonRectLeft.width = ButtonWidth;
	ButtonRectLeft.height = ButtonHeight;
	window.draw(sSwitcher);

	//ButtonRectRight
	sSwitcher.setPosition(B_RIGHT_CORNER_X, B_RIGHT_CORNER_Y);
	ButtonRectRight.left = B_RIGHT_CORNER_X - ButtonWidth / 2;
	ButtonRectRight.top = B_RIGHT_CORNER_Y - ButtonHeight / 2;
	ButtonRectRight.width = ButtonWidth;
	ButtonRectRight.height = ButtonHeight;
	sSwitcher.rotate(180);//只有一个箭头素材，需代码生成另一个
	window.draw(sSwitcher);
	sSwitcher.rotate(180);//还原角度
}

void Game::DrawResults()
{
	int ButtonWidth, ButtonHeight;
	ButtonWidth = 250;
	ButtonHeight = sGameOver.getLocalBounds().height;

	if (player1.gameover || player2.gameover)
	{
		sGameOver.setTextureRect(IntRect(player1.gameover * ButtonWidth, 0, ButtonWidth, ButtonHeight));
		sGameOver.setPosition(P1_STAGE_CORNER_X + GRIDSIZE * 3, GRIDSIZE / 2);
		window.draw(sGameOver);

		sGameOver.setTextureRect(IntRect(player2.gameover * ButtonWidth, 0, ButtonWidth, ButtonHeight));
		sGameOver.setPosition(P2_STAGE_CORNER_X + GRIDSIZE * 3, GRIDSIZE / 2);
		window.draw(sGameOver);
	}
}

void Game::DrawText()
{
	int initialX, initialY;
	int CharacterSize = 48;
	text.setCharacterSize(CharacterSize - 10);
	text.setFillColor(Color(255, 0, 0, 255));
	text.setStyle(Text::Bold);
	text.setPosition(P1_SCORE_CORNER_X, P1_SCORE_CORNER_Y + 10);
	std::stringstream ss;
	ss << player1.score;
	text.setString(ss.str()); window.draw(text);

	text.setPosition(P2_SCORE_CORNER_X - CharacterSize * 3, P2_SCORE_CORNER_Y + 10);
	ss.str("");
	ss << player2.score;
	text.setString(ss.str()); window.draw(text);
	CharacterSize = 18;
	text.setCharacterSize(CharacterSize);
	text.setFillColor(Color(255, 255, 255, 255));
	text.setStyle(Text::Regular || Text::Italic);
	initialY = P1_STAGE_CORNER_Y + STAGE_HEIGHT * GRIDSIZE;

	CharacterSize = 24;
	text.setCharacterSize(CharacterSize);
	text.setFillColor(Color(255, 255, 255, 255));
	text.setStyle(Text::Bold); 
	initialY = INFO_CORNER_Y;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L"玩家 1:"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" 方向键：WASD"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" 速降： Space 键"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" 交换： 左 Ctrl 键"); window.draw(text);
	initialY += CharacterSize * 1.5;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L"玩家 2:"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" 方向键：↑←↓→"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" 速降： Enter 键"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" 交换： 右 Ctrl 键"); window.draw(text);
	initialY += 2 * CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L"自带底部缓冲功能"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L"退出： Esc 键"); window.draw(text);
}
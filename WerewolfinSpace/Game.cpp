#include "Game.h"


Game::Game(DxHandler* dxHandler, InputHandler* inputHandler, Camera* camera)
{
	
	this->inputHandler = inputHandler;
	this->dxHandler = dxHandler;
	this->camera = camera;
	D3DX10CreateFont(dxHandler->getID3D10Device(), 16, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS,
									ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Verdana", &font);


	sh = new SoundHandler();
	initGame();
}

void Game::initGame()
{
	//create game menu
	menu = new GameMenu(dxHandler,inputHandler,this);
	inMenu = new InGameMenu(dxHandler, inputHandler, this);
	state = GMenu;

	QueryPerformanceFrequency((LARGE_INTEGER*)&secsPerCount);
	secsPerCount = 1 / secsPerCount;
	startTime = 0;
	timer = 0;
	fpsUpdate = 1;
	mapIndex = 1;

	highScore = new HighScore(menu->getFont(), menu->getSprite(), mapIndex);

	sh->initialize(*dxHandler->getHWnd(), "backgroundmusic.wav");
	sh->playLoop();

	currentLevel = NULL;
	
	//previous keystate
	keyState = inputHandler->getKeyState();
	prevKeyState = inputHandler->getPrevKeyState();

	ID3D10ShaderResourceView* mouseTex  = NULL;
	D3DX10CreateShaderResourceViewFromFile(dxHandler->getID3D10Device(), "Images/cursor.png", NULL, NULL, &mouseTex, NULL);

	ID3D10ShaderResourceView* cursorAlpha = NULL;
	D3DX10CreateShaderResourceViewFromFile(dxHandler->getID3D10Device(), "Images/cursorAlpha.png", NULL, NULL, &cursorAlpha, NULL);

	mouse = new Sprite(D3DXVECTOR2(0,0),D3DXVECTOR2(50,50),mouseTex, cursorAlpha, dxHandler);
	mouse->setPos( 510, 186 );

	mouseTex = NULL;
	cursorAlpha = NULL;
}



void Game::Draw(double dt)
{
	switch(state)
	{
		case Playing: 
		{
			currentLevel->Draw(dt);
			//used for drawing the fps to the screen
			RECT t;
			t.top = 100;
			t.bottom =  150;
			t.left =100;
			t.right =200;

			std::ostringstream buffFps;
			
			buffFps << fps;
			string text;
			text += "FPS: " + buffFps.str(); 
			font->DrawTextA(NULL, text.c_str(), -1, &t, DT_LEFT | DT_CENTER , D3DXCOLOR(1, 1, 1, 1.0f));
			break;
		}
		case GMenu:
		{
			menu->Draw(highScore);
			mouse->Draw(dxHandler);
			
			
			break;
		}
		case LevelCompleted:
		{
			inMenu->Draw(highScore);
			mouse->Draw(dxHandler);
			break;
		}
	}
	
	

	dxHandler->getID3D10Device()->OMSetBlendState(NULL, 0, 0xffffffff);
}
double prevDt = 0;
void Game::Update(double dt)
{	
	
	fpsUpdate += dt;
	if(fpsUpdate >= 1)
	{
		if(dt > 1)
			dt = prevDt;
		fps = (int)(1.0 / dt + 0.5);
		fpsUpdate = 0;
	}

	switch(state)
	{
		case Playing:
		{

			currentLevel->Update(dt);

			if(currentLevel->levelOver)
				state = LevelCompleted;
				

			if(keyState[DIK_ESCAPE] & 0x80 && !prevKeyState[DIK_ESCAPE])
			{
				state = GMenu;
				menu->setToMainState();
			}

			break;
		}
		case GMenu:
		{
			menu->Update(highScore);
			break;
		}
		case LevelCompleted:
		{
			inMenu->Update(highScore);

			break;
		}
	}
	
	prevDt = dt;
}

void Game::setLoadingScreen()
{
	//draw the loading screen once and then do a swap chain
	ID3D10Device* device = dxHandler->getID3D10Device();

	dxHandler->clearRenderTarget();

	ID3D10ShaderResourceView* loadinTex  = NULL;
	D3DX10CreateShaderResourceViewFromFile(device,"Images/loading1.jpg",NULL,NULL,&loadinTex,NULL);

	Sprite* loadingScreen = new Sprite(D3DXVECTOR2(0,0),D3DXVECTOR2(SCREENWIDTH,_SCREENHEIGHT),loadinTex,dxHandler);
	
	loadingScreen->Draw( dxHandler );
	dxHandler->swapChain();


	device = NULL;
	SAFE_RELEASE(loadinTex);
	SAFE_DELETE( loadingScreen );
}

void Game::setPlayingState(bool nextLevel)
{
	setLoadingScreen();
	mapIndex += nextLevel ? 1 : 0;

	highScore = NULL;
	highScore = new HighScore(menu->getFont(), menu->getSprite(), mapIndex);

	if(nextLevel)
	{
		if(currentLevel)
			SAFE_DELETE(currentLevel);

		currentLevel = new Level(dxHandler, mapIndex, inputHandler, camera);
	}
	else
		currentLevel->restartLevel();

	state = Playing;
	ShowCursor(false);
}

void Game::newGame()
{
	setLoadingScreen();
	mapIndex = 1;

	//The vector of strings is preventing us from deleting the highscore.
	highScore = NULL;
	highScore = new HighScore(menu->getFont(), menu->getSprite(), mapIndex);

	if(currentLevel)
		SAFE_DELETE(currentLevel);

	currentLevel = new Level(dxHandler, mapIndex, inputHandler, camera);

	state = Playing;
	ShowCursor(false);
}
void Game::startSelectedLevel(int index)
{
	setLoadingScreen();

	if(mapIndex == index)
	{
		currentLevel->restartLevel();
		state = Playing;
		return;
	}
	else
		mapIndex = index;

	//The vector of strings is preventing us from deleting the highscore.
	highScore = NULL;
	highScore = new HighScore(menu->getFont(), menu->getSprite(), mapIndex);

	if(currentLevel)
		SAFE_DELETE(currentLevel);

	currentLevel = new Level(dxHandler, mapIndex, inputHandler, camera);

	state = Playing;
	ShowCursor(false);

}
void Game::resumeGame()
{
	state = Playing;
}

void Game::setMenuState()
{
	state = GMenu;
	ShowCursor(false);
}

Game::~Game()
{

	if( currentLevel )
		SAFE_DELETE( currentLevel );
	
	
	SAFE_DELETE( menu );
	if( inMenu )
		SAFE_DELETE( inMenu );
	
	//doesn't work   something with the vector<string> entries
	//SAFE_DELETE(highScore);
	highScore = NULL;

	dxHandler = NULL;
	inputHandler = NULL;

	SAFE_RELEASE( font );
	SAFE_DELETE( mouse );
	SAFE_DELETE ( sh );

	keyState = NULL;
	prevKeyState = NULL;
}

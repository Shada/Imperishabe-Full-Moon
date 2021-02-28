#pragma once
#include "stdafx.h"
#include "Level.h"
#include "GameMenu.h"
#include "InGameMenu.h"
#include "Player.h"
#include "Camera.h"
#include "Block.h"
#include "InputHandler.h"
#include "AABB.h"
#include "HighScore.h"

class Game
{
private:

	Level			*currentLevel;
	GameMenu		*menu;
	InGameMenu		*inMenu;
	HighScore		*highScore;

	Camera			*camera;

	DxHandler		*dxHandler;
	InputHandler	*inputHandler;
	SoundHandler	*sh;

	ID3DX10Font		*font;
	Sprite			*mouse;

	enum GameState
	{
		GMenu,
		Playing,
		LevelCompleted
	};
	GameState state;
	
	int fps, mapIndex;
	double startTime, timer, secsPerCount, fpsUpdate;
	
	char *prevKeyState, *keyState;

	void initGame();
	void setLoadingScreen();

public:
	
	Game(DxHandler* dxHandler, InputHandler* inputHandler, Camera* camera);
	~Game();

	void Update(double dt);
	void Draw(double dt);
	void newGame();
	void setPlayingState(bool nextLevel); //if false then restart same level
	// When returning to the menu, we oftenly want to return to the main state
	void setMenuState();
	void resumeGame();
	void startSelectedLevel(int index);
	
	Level* getCurrentLevel()	{ return currentLevel;	}
	Camera *getCamera()			{ return camera;		}
	Sprite* getMouse()			{ return mouse;			}

	// Could be only one function, however, the program is easier to follow this way
	int getNextLevelIndex()		{ return mapIndex + 1;	}
	int getCurrentLevelIndex()	{ return mapIndex;		}
};
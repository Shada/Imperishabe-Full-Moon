#pragma once
#include "Sprite.h"
#include "MenuButton.h"
#include "GameCredits.h"
#include "HighScore.h"
#include "DxHandler.h"
#include "InputHandler.h"

class Game;
class BaseMenu
{
protected:
	DxHandler				  *dxHandler;
	InputHandler			  *inputHandler;

	ID3D10ShaderResourceView  *background, 
							  *buttonTex;

	DIMOUSESTATE			  *mouseState, *prevMouseState;
	char					  *keyState, *prevKeyState;

	ID3D10Buffer			  *g_pVB;
	Sprite					  *sprite;

	RECT r, rRes, rGame;
	int numberOfButtons;

	vector<MenuButton*> buttons;
	// Saves the indexes of the buttons so that only the relevant buttons can be hovered
	vector<int> *hoverableButtons;
	
	Game					  *game;

	ID3DX10Sprite			  *fontSprite;
	ID3DX10Font				  *font, *highscoreFont;

	// For mouse movments
	float xMove, yMove;

	// Some buttons cannot be hovered at all times, send in those who can
	void manageHover(vector<MenuButton*> *button, vector<int> *hoverableButtons);
public:
	BaseMenu(DxHandler* dxHandler, InputHandler* inputHandler, Game* game);
	~BaseMenu();

	// Both menues have the rights and components to load highscore from another map and therefore overwrite
	// the one currently being used
	virtual void Update	(HighScore *&hs){}
	virtual void Draw	(HighScore *hs)	{}

	ID3DX10Font		*getFont()		{ return font; }
	ID3DX10Sprite	*getSprite()	{ return fontSprite; }

	int getNumberOfButtons()				{ return numberOfButtons;	}
	MenuButton* getMenuButtonAt(int index)	{ return buttons[index];	}

	// A function to convert from pixels into clip space
	inline float pixToClip( const int pixelDimension, const int pixels )
	{
		return (float)pixels / pixelDimension * 2 - 1;
	}
	
	// Convert a sprites dimensions into clip space
	inline float pixToClipDist( const int pixelDimension, const int pixels )
	{
	    return (float)pixels / pixelDimension * 2;
	}
};
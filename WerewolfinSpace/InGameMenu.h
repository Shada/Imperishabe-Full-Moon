#pragma once
#include "BaseMenu.h"

class InGameMenu :
	public BaseMenu
{
private:
	bool noMoreLevels, highScoreAlreadySet;
	int time;
	string highScoreEntry;

	D3DXVECTOR2 textFieldPos;

	Sprite *textField;
	ID3D10ShaderResourceView *textFieldTex;
public:
	InGameMenu(DxHandler* dxHandler, InputHandler* inputHandler, Game* game);
	~InGameMenu();

	void Update(HighScore *&hs);
	void Draw(HighScore *hs);
	
	enum MenuStage
	{
		UpdateEntities,
		WaitForButtonClick,
		WaitForKeys
	};
	MenuStage stage;
};
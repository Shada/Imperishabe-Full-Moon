#include "InGameMenu.h"
#include "Game.h"
HWND hEdit;
InGameMenu::InGameMenu(DxHandler* dxHandler, InputHandler* inputHandler, Game* game)
	: BaseMenu(dxHandler, inputHandler, game)
{
	SpriteVertex* pData = NULL;
	g_pVB->Map( D3D10_MAP_WRITE_DISCARD, 0, reinterpret_cast< void** >((void**)&pData) );

	sprite = new Sprite(D3DXVECTOR2(0.f, 0.f), D3DXVECTOR2(SCREENWIDTH, _SCREENHEIGHT), background, g_pVB, 0, pData);

	noMoreLevels = true;
	highScoreAlreadySet = false;
	stage = UpdateEntities;

	time = 0;

	textFieldPos = D3DXVECTOR2(SCREENWIDTH * .5f - 75, 510);

	textFieldTex = NULL;
	D3DX10CreateShaderResourceViewFromFile(dxHandler->getID3D10Device(), "Images/textfield.png", NULL, NULL, &textFieldTex, NULL);

	textField = new Sprite(textFieldPos, D3DXVECTOR2(140, 30), textFieldTex, dxHandler);

	string buttonText[3];
	buttonText[0] = "Restart Level";
	buttonText[1] = "Main Menu";
	buttonText[2] = "Next Level";

	for(int i = 1; i > -2; i--)
	{
		buttons.push_back(
				new MenuButton(buttonText[1 - i],
					D3DXVECTOR2((float)(SCREENWIDTH / 2 - 210 * i - 100), 610),
					D3DXVECTOR2(200.0f, 50.0f),
					buttonTex,
					g_pVB,
					3 - i,
					pData, 
					dxHandler));
	}

	//include all buttons so that they may be exluded later
	hoverableButtons->push_back(0);
	hoverableButtons->push_back(1);
	hoverableButtons->push_back(2);
}

void InGameMenu::Update(HighScore *&hs)
{
	Sprite* mouse = game->getMouse();
	xMove = (float)inputHandler->getDeltaX();
	yMove = (float)inputHandler->getDeltaY();
	
	if(mouse->getTopLeft().x + xMove > SCREENWIDTH - 4 || mouse->getTopLeft().x + xMove < 0) xMove = 0;
	if(mouse->getTopLeft().y + yMove > _SCREENHEIGHT - 3  || mouse->getTopLeft().y + yMove < 0) yMove = 0;
	
	if(xMove != 0 || yMove != 0)
		mouse->move(xMove, yMove);

	if(mouseState->rgbButtons[0] && stage != UpdateEntities)
	{
		if(	   (mouse->intersects(*getMenuButtonAt(2)->getBackground()) && noMoreLevels )
			||	mouse->intersects(*getMenuButtonAt(1)->getBackground())
			||  mouse->intersects(*getMenuButtonAt(0)->getBackground()))
		{
			if(!highScoreAlreadySet && stage == WaitForKeys)
				// If the user hasn't pressed enter, we still want to add the entry to the list
				hs->addEntry(highScoreEntry);

			if(mouse->intersects(*getMenuButtonAt(0)->getBackground()))
			{	// Same level
				game->setPlayingState(false);
			}

			else if(mouse->intersects(*getMenuButtonAt(1)->getBackground()))
			{	// Return to main
				game->setMenuState();
			}
		
			else if(mouse->intersects(*getMenuButtonAt(2)->getBackground()) && noMoreLevels)
			{	// Next level
				game->setPlayingState(true);
			}
			// Since we already know that one of the buttons were clicked, these general settings are made
			highScoreAlreadySet = false;
			hs->checkAgainstHighScore(-1);
			stage = UpdateEntities;
			return;
		}
	}

	switch(stage)
	{
		case UpdateEntities:
			if(!game->getCurrentLevel()->Peek(game->getNextLevelIndex()))
			{
				noMoreLevels = false;
				// There is no more levels, so the next level button should not be processed
				hoverableButtons->pop_back();
			}
			else
			{
				// Better safe than sorry, this way the button won't be processed wrongly
				hoverableButtons->clear();
				hoverableButtons->push_back(0);
				hoverableButtons->push_back(1);
				hoverableButtons->push_back(2);
			}

			// get the points from the completed turn and check whether or not it should be inserted into the highscore list
			if(hs->checkAgainstHighScore(game->getCurrentLevel()->getScore()))
				stage = WaitForKeys;
			else
				stage = WaitForButtonClick;

			break;
		case WaitForButtonClick:
			
			
			break;
		case WaitForKeys:
			// add letters to the highscore entry
			bool lowerCase = true;
			if(inputHandler->getPrevKeyState()[DIK_LSHIFT] || inputHandler->getPrevKeyState()[DIK_RSHIFT])
				lowerCase = false;


			for(int c = 1; c < 51; c++)
			{
				if((c > 15 && c < 26) || (c > 29 && c < 39) || (c > 43 && c < 51))
				{
					if(inputHandler->getKeyState()[c] & 0x80 && !inputHandler->getPrevKeyState()[c])
					{
						// Convert to ascii
						static HKL layout = GetKeyboardLayout(0);
						char temp = MapVirtualKeyEx(c, 1, layout);

						if(highScoreEntry.length() < 15)
							// Adding 32 will make the symbol into a capital letter
 							highScoreEntry += lowerCase ? temp + 32 : temp;
					}
				}
				else if(inputHandler->getKeyState()[c] & 0x80 && !inputHandler->getPrevKeyState()[c])
				{
					// Of course å,ä and ö must be useble
					if		(c == 26) highScoreEntry += lowerCase ? "å" : "Å";
					else if	(c == 39) highScoreEntry += lowerCase ? "ö" : "Ö";
					else if	(c == 40) highScoreEntry += lowerCase ? "ä" : "Ä";
				}
			}
			
			if(inputHandler->getKeyState()[DIK_SPACE] & 0x80 && !inputHandler->getPrevKeyState()[DIK_SPACE])
				highScoreEntry += " ";
			
			// Logic for backspace key, if held for more than 20 frames, then it will have the proper 
			// 'hold down to remove without lifting' function
			if(inputHandler->getKeyState()[DIK_BACKSPACE] && inputHandler->getPrevKeyState()[DIK_BACKSPACE] && time > 30 && time % 2 == 0)
				highScoreEntry = highScoreEntry.substr(0,  highScoreEntry.length() - 1);
			else if(inputHandler->getKeyState()[DIK_BACKSPACE] && !inputHandler->getPrevKeyState()[DIK_BACKSPACE])
				highScoreEntry = highScoreEntry.substr(0,  highScoreEntry.length() - 1);
			else if(!inputHandler->getKeyState()[DIK_BACKSPACE] && inputHandler->getPrevKeyState()[DIK_BACKSPACE])
				time = 0;

			if(inputHandler->getKeyState()[DIK_BACKSPACE] && inputHandler->getPrevKeyState()[DIK_BACKSPACE])
				time++;

			if(inputHandler->getKeyState()[DIK_RETURN] & 0x80)
			{
				// Add the entry
				stage = WaitForButtonClick;
				hs->addEntry(highScoreEntry);
				highScoreEntry = "";
				highScoreAlreadySet = true;
			}
			break;
	}
	manageHover(&buttons, hoverableButtons);
}

void InGameMenu::Draw(HighScore *hs)
{
	// Background image
	sprite->Draw(dxHandler);

	buttons[0]->Draw(dxHandler);
	buttons[1]->Draw(dxHandler);
	buttons[2]->Draw(dxHandler);

	RECT r;
	r.top = 80;
	r.bottom = 130;
	r.left = (long)(SCREENWIDTH * .5f) - 250;
	r.right = (long)(SCREENWIDTH * .5f) + 250;

	hs->Draw(dxHandler);

	string text = "Highscore on level ";
	text += (char)(game->getCurrentLevelIndex() + 48);

	// The game name
	highscoreFont->DrawTextA(NULL, text.c_str(), -1, &r, DT_LEFT | DT_CENTER, D3DXCOLOR(0.9216f, 0.9176f, 0.9098f, 0.95f));

	r.top += 70;
	r.bottom += 70;

	if(stage == WaitForKeys)
	{
		font->DrawTextA(NULL, "Congratulations! You made it into the highscore list!", -1, &r, DT_LEFT | DT_CENTER, D3DXCOLOR(1, 1, 1, 1.0f));
		
		textField->Draw(dxHandler);

		r.top += 20;
		r.bottom += 20;

		// Get the score and convert into a char so that it easily can be converted into a string
		int score = game->getCurrentLevel()->getScore();
		char buffer[16];
		sprintf_s(buffer, "%d", score);
		text = "Your score on this level: " + (string)buffer;

		font->DrawTextA(NULL, text.c_str(), -1, &r, DT_LEFT | DT_CENTER, D3DXCOLOR(1, 1, 1, 1.0f));
	}
	else if (stage == WaitForButtonClick && !highScoreAlreadySet)
	{
		font->DrawTextA(NULL, "Unfortunatly, your score was not enough to be added to the list.", -1, &r, DT_LEFT | DT_CENTER, D3DXCOLOR(1, 1, 1, 1.0f));
		
		r.top += 20;
		r.bottom += 20;

		// Get the score and convert into a char so that it easily can be converted into a string
		int score = game->getCurrentLevel()->getScore();
		char buffer[16];
		sprintf_s(buffer, "%d", score);
		text = "Your score on this level: " + (string)buffer;
		font->DrawTextA(NULL, text.c_str(), -1, &r, DT_LEFT | DT_CENTER, D3DXCOLOR(1, 1, 1, 1.0f));
	}

	// The text on the textfield with a rectangle set on top of the textfield
	r.top		= (long)textFieldPos.y + 6;
	r.bottom	= (long)textFieldPos.y + 24;
	r.left		= (long)textFieldPos.x + 5;
	r.right		= (long)textFieldPos.x + 140;

	font->DrawTextA(NULL, highScoreEntry.c_str(), -1, &r, DT_LEFT, D3DXCOLOR(0, 0, 0, 1.0f));

	dxHandler->getID3D10Device()->OMSetBlendState(NULL, 0, 0xffffffff);
}

InGameMenu::~InGameMenu()
{
	SAFE_RELEASE(textFieldTex);
	SAFE_DELETE(textField);
}
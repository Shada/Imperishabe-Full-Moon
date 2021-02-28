#include "GameMenu.h"
#include "Game.h"

HWND hwnd, hTextField;
GameMenu::GameMenu(DxHandler* dxHandler, InputHandler* inputHandler, Game* game)
	: BaseMenu(dxHandler, inputHandler, game)
{
	mState = Main;

	indexOfHighScore = 1;
	currentResolution = 3;
	selectMapIndex = 1;

	// 3 rectangles for the diffrent outputs on the screen
	r.top = 80;
	r.bottom = 130;
	r.left = SCREENWIDTH / 2 - 180;
	r.right = SCREENWIDTH / 2 + 180;

	rRes.top = 300;
	rRes.bottom = 350;
	rRes.left = SCREENWIDTH / 2 - 75;
	rRes.right = SCREENWIDTH / 2 + 75;

	rGame.top = 40;
	rGame.bottom = 90;
	rGame.left = SCREENWIDTH / 2 - 180;
	rGame.right = SCREENWIDTH / 2 + 180;

	// Load in all the different resolution that should be avaliable for the player (doensn't work yet though...)
	char buf[10];
	sprintf_s(buf, "%d", SCREENWIDTH);
	screenResolution = (string)buf + " x ";
	sprintf_s(buf, "%d", _SCREENHEIGHT);
	screenResolution += (string)buf;

	resolutions.push_back("800x600");
	resolutionSizes.push_back(D3DXVECTOR2(800, 600));
	resolutions.push_back("1024x768");
	resolutionSizes.push_back(D3DXVECTOR2(1024, 768));
	resolutions.push_back("1280x1024");
	resolutionSizes.push_back(D3DXVECTOR2(1280, 1024));

	resolutions.push_back("1280x720");
	resolutionSizes.push_back(D3DXVECTOR2(1280, 720));
	resolutions.push_back("1366x768");
	resolutionSizes.push_back(D3DXVECTOR2(1366, 768));
	resolutions.push_back("1920x1080");
	resolutionSizes.push_back(D3DXVECTOR2(1920, 1080));

	SpriteVertex* pData = NULL;
	g_pVB->Map( D3D10_MAP_WRITE_DISCARD, 0, reinterpret_cast< void** >((void**)&pData) );

	sprite = new Sprite(D3DXVECTOR2(0.f, 0.f), D3DXVECTOR2(SCREENWIDTH, _SCREENHEIGHT), background, g_pVB, 0, pData);

	// Add the text for the buttons in the menu
	string buttonText[9];
	buttonText[0] = "Resume";
	buttonText[1] = "New Game";
	buttonText[2] = "Select Chapter";
	buttonText[3] = "Options";
	buttonText[4] = "Highscore";
	buttonText[5] = "Credits";
	buttonText[6] = "Exit";
	buttonText[7] = "Apply Changes";
	buttonText[8] = "Back";

	for(int i = 0; i < 7; i++)
	{
		buttons.push_back(
				new MenuButton(buttonText[i],
					D3DXVECTOR2((float)(SCREENWIDTH * .5f - 100), (float)(60 * i + 160)),
					D3DXVECTOR2(200.0f, 50.0f),
					buttonTex,
					g_pVB,
					2 + i,
					pData, 
					dxHandler));

		// Start with having only the buttons visible in Main state
		hoverableButtons->push_back(i);
	}

	// We don't nessessarily want them all to be directly below the previous
	buttons.push_back(
				new MenuButton(buttonText[7],
					D3DXVECTOR2((float)(SCREENWIDTH * .5f - 100), (float)(60 * 7 + 100)),
					D3DXVECTOR2(200.0f, 50.0f), buttonTex, g_pVB, 9, pData, dxHandler));
	buttons.push_back(
				new MenuButton(buttonText[8],
					D3DXVECTOR2((float)(SCREENWIDTH * .5f - 100), (float)(60 * 8 + 100)),
					D3DXVECTOR2(200.0f, 50.0f),	buttonTex, g_pVB, 10, pData, dxHandler));

	// Create the next and previous button without text and with some alpha
	ID3D10ShaderResourceView *tex, *alpha;

	D3DX10CreateShaderResourceViewFromFile(dxHandler->getID3D10Device(), "Images/buttonNext.png", NULL, NULL, &tex, NULL);
	D3DX10CreateShaderResourceViewFromFile(dxHandler->getID3D10Device(), "Images/buttonNextAlpha.png", NULL, NULL, &alpha, NULL);
	
	//next button
	buttons.push_back(new MenuButton("", D3DXVECTOR2(SCREENWIDTH * .5f + 150, _SCREENHEIGHT - 160), D3DXVECTOR2(50, 50),
					tex, alpha, g_pVB, 11, pData, dxHandler));

	D3DX10CreateShaderResourceViewFromFile(dxHandler->getID3D10Device(), "Images/buttonNext.png", NULL, NULL, &tex, NULL);
	D3DX10CreateShaderResourceViewFromFile(dxHandler->getID3D10Device(), "Images/buttonPreviousAlpha.png", NULL, NULL, &alpha, NULL);

	//previous button
	buttons.push_back(new MenuButton("", D3DXVECTOR2(SCREENWIDTH * .5f - 200, _SCREENHEIGHT - 160), D3DXVECTOR2(50, 50),
					tex, alpha, g_pVB, 12, pData, dxHandler));

	// These two are the more graphical buttons that are linked to the levels
	D3DX10CreateShaderResourceViewFromFile(dxHandler->getID3D10Device(), "Images/map1.png", NULL, NULL, &tex, NULL);

	//map1 button
	buttons.push_back(new MenuButton("", D3DXVECTOR2(SCREENWIDTH * .5f - 200, _SCREENHEIGHT * 0.5f - 160), D3DXVECTOR2(400, 217),
					tex, g_pVB, 13, pData, dxHandler));

	D3DX10CreateShaderResourceViewFromFile(dxHandler->getID3D10Device(), "Images/map2.png", NULL, NULL, &tex, NULL);

	//map2 button
	buttons.push_back(new MenuButton("", D3DXVECTOR2(SCREENWIDTH * .5f - 200, _SCREENHEIGHT * 0.5f - 160), D3DXVECTOR2(400, 217),
					tex, g_pVB, 14, pData, dxHandler));

	g_pVB->Unmap();

	tex = NULL;
	alpha = NULL;

	credits = new GameCredits(font, fontSprite);
}

void GameMenu::Draw(HighScore *hs)
{
	// The background image is always drawn first
	sprite->Draw(dxHandler);
	
	highscoreFont->DrawTextA(NULL, "Imperishable Full Moon", -1, &rGame, DT_LEFT | DT_CENTER, D3DXCOLOR(0.9216f, 0.9176f, 0.9098f, 0.95f));

	switch(mState)
	{
		case Main:
		{
			for(int i = 0; i < 7; i++)
				buttons[i]->Draw(dxHandler);
			//Draw resume, new game, select chapter, options, highscore, credits and exit buttons
			break;
		}
		case Credits:
		{
			credits->Draw(dxHandler);
			buttons[8]->Draw(dxHandler);

			break;
		}
		case EndScreen:
		{
			// Called only once, to replace a boring loading screen with the credits when the game is shut down.
			// This takes about two, three seconds if a game has been started and not unloaded
			credits->Draw(dxHandler);
			// Do not draw the mouse inside the screen since the window no longer respondes to commands
			game->getMouse()->setPos(-100, -100);

			break;
		}
		case Highscore:
		{
			hs->Draw(dxHandler);
			buttons[8]->Draw(dxHandler);
			buttons[9]->Draw(dxHandler);
			buttons[10]->Draw(dxHandler);

			// The easiest way of solving the text telling you which level you are looking at. However, should the
			// amount of maps proceed 10 this needs to be replaced with a fprintf or a stringstream
			string text = "Highscore on level ";
			text += (char)(indexOfHighScore + 48);
			highscoreFont->DrawTextA(NULL, text.c_str(), -1, &r, DT_LEFT | DT_CENTER, D3DXCOLOR(0.9216f, 0.9176f, 0.9098f, 0.95f));
			
			break;
		}
		case Options:
		{	
			buttons[7]->Draw(dxHandler);
			buttons[8]->Draw(dxHandler);
			buttons[9]->Draw(dxHandler);
			buttons[10]->Draw(dxHandler);

			// Draw the resolutions currently selected
			font->DrawTextA(NULL, resolutions.at(currentResolution).c_str(), -1, &rRes, DT_LEFT | DT_CENTER, D3DXCOLOR(1, 1, 1, 1.0f));
			
			break;
		}
		case SelectChapter:
		{
			buttons[8]->Draw(dxHandler); // back
			buttons[9]->Draw(dxHandler); // next
			buttons[10]->Draw(dxHandler);// prev
			buttons[10 + selectMapIndex]->Draw(dxHandler);

			string text = "Level ";
			text += (char)(selectMapIndex + 48);
			highscoreFont->DrawTextA(NULL, text.c_str(), -1, &r, DT_LEFT | DT_CENTER, D3DXCOLOR(0.9216f, 0.9176f, 0.9098f, 0.95f));
			
			break;
		}
	}
}

void GameMenu::Update(HighScore *&hs)
{
	Sprite* mouse = game->getMouse();
	xMove = (float)inputHandler->getDeltaX();
	yMove = (float)inputHandler->getDeltaY();

	// Boundaries for the mouse
	if(mouse->getTopLeft().x + xMove > SCREENWIDTH - 4 || mouse->getTopLeft().x + xMove < 0) xMove = 0;
	if(mouse->getTopLeft().y + yMove > _SCREENHEIGHT - 3  || mouse->getTopLeft().y + yMove < 0) yMove = 0;
	
	if(xMove != 0 || yMove != 0)
		// The mouse wasn't moved, so no update is needed
		mouse->move(xMove, yMove);

	switch(mState)
	{
		case Main:
		{
			if(!game->getCurrentLevel() && hoverableButtons->size() == 7)
			{
				// Resume button is set to be unavilable since no game is running
				hoverableButtons->clear();
				for(int c = 1; c < 7; c++)
					hoverableButtons->push_back(c);
			}
			else if(game->getCurrentLevel() && hoverableButtons->size() == 7)
			{
				hoverableButtons->clear();
				for(int c = 0; c < 7; c++)
					hoverableButtons->push_back(c);
			}

			if(mouseState->rgbButtons[0])
			{
				// Check mouse against buttons and act accordingly
				if(mouse->intersects(*getMenuButtonAt(0)->getBackground()) && game->getCurrentLevel())
					game->resumeGame();
				else if(mouse->intersects(*getMenuButtonAt(1)->getBackground()))
					game->newGame();
				else if(mouse->intersects(*getMenuButtonAt(2)->getBackground()))
				{
					hoverableButtons->clear();
					for(int c = 7; c < 13; c++)
						hoverableButtons->push_back(c);

					selectMapIndex = 1;
					mState = SelectChapter;
				}
				else if(mouse->intersects(*getMenuButtonAt(3)->getBackground()))
				{
					hoverableButtons->clear();
					for(int c = 7; c < 11; c++)
						hoverableButtons->push_back(c);

					mState = Options;
				}
				else if(mouse->intersects(*getMenuButtonAt(4)->getBackground()))
				{
					hoverableButtons->clear();
					for(int c = 8; c < 11; c++)
						hoverableButtons->push_back(c);

					mState = Highscore;
				}
				else if(mouse->intersects(*getMenuButtonAt(5)->getBackground()))
				{
					hoverableButtons->clear();
					hoverableButtons->push_back(8);

					mState = Credits;
				}
				else if(mouse->intersects(*getMenuButtonAt(6)->getBackground()))
					{ mState = EndScreen; PostQuitMessage(0); }
			}

			break;
		}
		case Credits:
		{
			// Only thing we need is a button for going back
			if(mouseState->rgbButtons[0] && !prevMouseState->rgbButtons[0] && 
				mouse->intersects(*getMenuButtonAt(8)->getBackground()))
				setToMainState();

			break;
		}
		case Highscore:
		{
			if(mouseState->rgbButtons[0] && !prevMouseState->rgbButtons[0])
			{
				if(		 mouse->intersects(*getMenuButtonAt(8)->getBackground()))
					{ setToMainState(); break; }
				// Else, re-create the highscore to show the correct one
				else if( mouse->intersects(*getMenuButtonAt(9)->getBackground()) && !noNext )
				{
					hs = NULL;
					hs = new HighScore(font, fontSprite, ++indexOfHighScore);
				}
				else if( mouse->intersects(*getMenuButtonAt(10)->getBackground()) && !noPrev)
				{
					hs = NULL;
					hs = new HighScore(font, fontSprite, --indexOfHighScore);
				}				
			}
			else if(mouseState->rgbButtons[0])
			{
				// Check whether or not the previous and next button should be clickable
				noNext = !game->getCurrentLevel()->Peek(indexOfHighScore + 1);
				noPrev = !game->getCurrentLevel()->Peek(indexOfHighScore - 1);

				// If a button cannot be clicked, it cannot be hovered either
				if(noNext || noPrev)
				{
					int skipIndex = noNext ? 9 : 10;
					
					hoverableButtons->clear();
					for(int c = 7; c < 11; c++)
					{
						if(c != skipIndex)
							hoverableButtons->push_back(c);
					}
				}
			}

			break;
		}
		case Options:
		{
			if(mouseState->rgbButtons[0] && !prevMouseState->rgbButtons[0])
			{
				if(		 mouse->intersects(*getMenuButtonAt(8)->getBackground()))
					{ setToMainState(); break; }
				else if( mouse->intersects(*getMenuButtonAt(9)->getBackground()) && !noNext )
					currentResolution++;
				else if( mouse->intersects(*getMenuButtonAt(10)->getBackground()) && !noPrev)
					currentResolution--;
				else if( mouse->intersects(*getMenuButtonAt(7)->getBackground()))
				{
					// MoveWindow() does not only move the window, it also rezises it
					MoveWindow(*dxHandler->getHWnd(), 0, 0, (int)resolutionSizes.at(currentResolution).x,
															(int)resolutionSizes.at(currentResolution).y + 32, false);
					float ratio = currentResolution > 2 ? 0.8f : 0.6f;
					game->getCamera()->setPerspective(ratio);
					// We did not manage to change the resolution, only the size of the window and the aspectratio
				}

				// Again, check whether or not the previous and next button should be clickable
				noNext = !((unsigned)currentResolution < resolutions.size() - 1);
				noPrev = !(currentResolution > 0);

				// If a button cannot be clicked, it cannot be hovered either
				if(noNext || noPrev)
				{
					int skipIndex = noNext ? 9 : 10;
					
					hoverableButtons->clear();
					for(int c = 7; c < 11; c++)
					{
						if(c != skipIndex)
							hoverableButtons->push_back(c);
					}
				}
			}

			break;
		}
		case SelectChapter:
		{
			if(mouseState->rgbButtons[0] && !prevMouseState->rgbButtons[0])
			{
				if(		 mouse->intersects(*getMenuButtonAt(8)->getBackground()))
					setToMainState();
				else if( mouse->intersects(*getMenuButtonAt(9)->getBackground()) && !noNext )
					selectMapIndex++;
				else if( mouse->intersects(*getMenuButtonAt(10)->getBackground()) && !noPrev)
					selectMapIndex--;
				else if( mouse->intersects(*getMenuButtonAt(11)->getBackground()))
					game->startSelectedLevel(selectMapIndex);
				// selectMapIndex is an integer we use for starting the correct level. If we add additional
				// maps, there is no need for any code change
			}
			else if(mouseState->rgbButtons[0])
			{
				// Once again, check whether or not the previous and next button should be clickable
				noNext = !game->getCurrentLevel()->Peek(selectMapIndex + 1);
				noPrev = !game->getCurrentLevel()->Peek(selectMapIndex - 1);

				// If a button cannot be clicked, it cannot be hovered either
				if(noNext || noPrev)
				{
					int skipIndex = noNext ? 9 : 10;
					
					hoverableButtons->clear();
					for(int c = 7; c < 13; c++)
					{
						if(c != skipIndex)
							hoverableButtons->push_back(c);
					}
				}
			}

			break;
		}
	}
	// Only the buttons that are shown and available are checked
	manageHover(&buttons, hoverableButtons);
}

void GameMenu::setToMainState()
{
	mState = Main;

	hoverableButtons->clear();
	for(int c = 1; c < 8; c++)
		hoverableButtons->push_back(c);
}

GameMenu::~GameMenu()
{
	SAFE_DELETE(credits);
}
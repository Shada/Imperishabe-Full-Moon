#include "BaseMenu.h"
#include "Game.h"

BaseMenu::BaseMenu(DxHandler* dxHandler, InputHandler* inputHandler, Game* game)
{
	this->dxHandler = dxHandler;
	this->inputHandler = inputHandler;
	this->game = game;

	D3DX10CreateShaderResourceViewFromFile(dxHandler->getID3D10Device(), "Images/menu1.jpg", NULL, NULL, &background, NULL);
	D3DX10CreateShaderResourceViewFromFile(dxHandler->getID3D10Device(), "Images/button.png", NULL, NULL, &buttonTex, NULL);

	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof( SpriteVertex ) * 15; // The multiply factor needs to be 1 more than the amount of buttons 
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	// Pointers to keystates and mousstates, so we don't need to get the states each update
	mouseState = inputHandler->getMouseState();
	prevMouseState = inputHandler->getPrevMouseState();

	keyState = inputHandler->getKeyState();
	prevKeyState = inputHandler->getPrevKeyState();

	if( FAILED( dxHandler->getID3D10Device()->CreateBuffer( &bd, 0, &g_pVB ) ) )
		MessageBox( 0, "Unable to create Vertex Buffer", "VB Error", 0 );

	// Two fonts, one for regular text and the other for the name of the game
	D3DX10CreateFont(dxHandler->getID3D10Device(), 16, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
					OUT_TT_ONLY_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Verdana", &font);

	D3DX10CreateFont(dxHandler->getID3D10Device(), 60, 20, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
					OUT_TT_ONLY_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Gabriola", &highscoreFont);

	D3DX10CreateSprite(dxHandler->getID3D10Device(), 256, &fontSprite);

	hoverableButtons = new vector<int>();
}
void BaseMenu::manageHover(vector<MenuButton*> *buttons, vector<int> *hoverableButtons)
{
	int indexAt = 0;
	for(int c = hoverableButtons->front(); c <= hoverableButtons->back() + 1; c++)
	{
		// The next button will be last and therefore not included, so in order to mark it as not hovered
		// we need to include 1 extra button and automaticly set it to not hovered
		if(indexAt < hoverableButtons->size() && c == hoverableButtons->at(indexAt++))
			// A boolean sets to true or false and depending on the result, the pixel shader then sets the color accordingly
			buttons->at(c)->setHovered(game->getMouse()->intersects(*getMenuButtonAt(c)->getBackground()));
		else
		{
			indexAt--;
			if(c < buttons->size())
				buttons->at(c)->setHovered(false);
		}
	}
}
BaseMenu::~BaseMenu()
{
	dxHandler = NULL;
	inputHandler = NULL;

	SAFE_RELEASE( background );
	SAFE_RELEASE( buttonTex );
	
	SAFE_RELEASE( g_pVB );

	for( unsigned int i = 0; i < buttons.size(); i++ )
		SAFE_DELETE( buttons.at(i) );

	SAFE_DELETE( sprite );

	SAFE_RELEASE( fontSprite );
	SAFE_RELEASE( font );
	SAFE_RELEASE( highscoreFont );
}
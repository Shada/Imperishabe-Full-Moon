#pragma once
#include "Sprite.h"
class MenuButton
{
private:
	Sprite* background;
	ID3DX10Font* font;
	ID3DX10Sprite* sprite;

	// Each button needs a buffer offset which is sent into the constructor
	int bufferOffset;

	ID3D10Buffer* g_pVB;
	SpriteVertex* pData;

	RECT r;

	D3DXVECTOR2 anchor, dimensions;
	string text;

	void init( DxHandler *h );
public:
	// Two constructors; one for buttons without alpha, and one with
	MenuButton(	string text, D3DXVECTOR2 anchor, D3DXVECTOR2 dimensions, ID3D10ShaderResourceView* background,
				ID3D10Buffer* g_pVB, int bufferOffset, SpriteVertex* pData, DxHandler* h);
	MenuButton(	string text, D3DXVECTOR2 anchor, D3DXVECTOR2 dimensions, ID3D10ShaderResourceView* background,
				ID3D10ShaderResourceView* alpha, ID3D10Buffer* g_pVB, int bufferOffset, SpriteVertex* pData, DxHandler* h);
	~MenuButton();

	void Draw(DxHandler* h);

	Sprite* getBackground() { return background; }

	void setHovered(bool value) { background->setHovered(value); }
};
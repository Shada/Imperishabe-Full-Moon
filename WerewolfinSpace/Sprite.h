#pragma once
#include "stdafx.h"
#include "DxHandler.h"
class Sprite
{
private:
	int bufferOffset;

	ID3D10ShaderResourceView* background;
	ID3D10ShaderResourceView* alpha;

	// Hovered is sent into the shader and transparent determins which technique should be used
	bool transparent, hovered;
	ID3D10Buffer* g_pVB;

	D3DXMATRIX	mWorld;

	D3DXVECTOR3 topLeft,
				bottomRight;

	void init(D3DXVECTOR2 anchor, D3DXVECTOR2 dimensions, ID3D10ShaderResourceView* background, 
				ID3D10ShaderResourceView* alpha,ID3D10Buffer* g_pVB, int bufferOffset, SpriteVertex* pData);
	void init2(D3DXVECTOR2 anchor, D3DXVECTOR2 dimensions, ID3D10ShaderResourceView* background, 
				ID3D10ShaderResourceView* alpha, DxHandler* h);

	bool valueInRange(int value, int min, int max) { return (value >= min) && (value <= max); }

	// The bool, transparent, is used when determing which technique is sent. Called from Draw()
	void renderPasses(DxHandler* h, ID3D10EffectTechnique* technique);
public:
	//with shared buffer
	Sprite(D3DXVECTOR2 anchor, D3DXVECTOR2 dimensions, ID3D10ShaderResourceView* background, 
			ID3D10ShaderResourceView* alpha, ID3D10Buffer* g_pVB, int bufferOffset,SpriteVertex* pData);
	Sprite(D3DXVECTOR2 anchor, D3DXVECTOR2 dimensions, ID3D10ShaderResourceView* background, 
			ID3D10Buffer* g_pVB, int bufferOffset, SpriteVertex* pData);
	//without shared buffer
	Sprite(D3DXVECTOR2 anchor, D3DXVECTOR2 dimensions, ID3D10ShaderResourceView* background, DxHandler* h);
	Sprite(D3DXVECTOR2 anchor, D3DXVECTOR2 dimensions, ID3D10ShaderResourceView* background, ID3D10ShaderResourceView* alpha, DxHandler* h);

	~Sprite();

	// A function to convert from pixels into clip space
	inline float pixToClip( const int pixelDimension, const int pixels )
	{
		return (float)pixels / pixelDimension * 2 - 1;
	}
	 
	// Convert a sprites dimensions into clip space
	inline float pixToClipDist( const int pixelDimension, const int pixels )
	{
	    return (float)pixels / pixelDimension*2;
	}
	
	void Draw(DxHandler* h);
	void move(float x , float y);
	void setPos(float x, float y);
	void setHovered(bool value) { hovered = value; }

	// For intersection purposes
	D3DXVECTOR3 getTopLeft()		{ return topLeft;		}
	D3DXVECTOR3 getBottomRight()	{ return bottomRight;	}

	bool intersects(Sprite a); 
};
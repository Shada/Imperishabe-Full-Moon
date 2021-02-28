#pragma once
#include "stdafx.h"
#include "DxHandler.h"

class AABB
{
private:

	ID3D10Buffer *pVB;
	LineVertex* pData;
	int factor;
	void init(D3DXVECTOR3 pos, float angle);
protected:

	D3DXVECTOR3 min;
	D3DXVECTOR3 max;

	D3DXVECTOR3 dim;
public:

	AABB(D3DXVECTOR3 position, D3DXVECTOR3 dimensions, bool centerOrigin, float angle);
	AABB(D3DXVECTOR3 position, D3DXVECTOR3 *dimenMin, D3DXVECTOR3 *dimenMax, float scale, float angle);
	~AABB();
	
	bool intersects(AABB *box);
	bool intersectsXZ(AABB *box);
	bool intersectsY(AABB *box);

	D3DXVECTOR3 getMin() { return min; }
	D3DXVECTOR3 getMax() { return max; }

	float getBoxTopCoord() { return max.y; }
	float getBoxBottomCoord() { return min.y; }

	void updatePos(D3DXVECTOR3 position);

	void DrawHitBox( DxHandler* h );
};
#pragma once
#include "Object3D.h"

class Water :
	public Object3D
{
private:

	string image, alpha;
	float movement, t;
	D3DXVECTOR2 dim;

	void createWaterModel(ID3D10Device *device);
public:

	Water(D3DXVECTOR3 position, D3DXVECTOR2 dimensions, string image, string alpha, DxHandler *dxHandler, float top);
	~Water();

	void Draw(DxHandler *h);
	void Update(double dt, Object3D *player);
};
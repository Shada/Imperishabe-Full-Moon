#pragma once
#include "Object3D.h"
#include "stdafx.h"

class Skybox :
	public Object3D
{
private:

	ID3D10Device* g_pd3dDevice;

	vector<ID3D10ShaderResourceView*>* textures;

	D3DXVECTOR3* cameraPos;
public:

	Skybox(ID3D10EffectTechnique* tech, ID3D10Device* g_pd3dDevice, D3DXVECTOR3* cameraPos, vector<Model*> model, 
			vector<string> *images);
	~Skybox();

	void Draw(DxHandler* h);
	void Update(double dt, Object3D *player);
};
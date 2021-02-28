#pragma once
#include "powerup.h"
class Flash :
	public PowerUp
{
private:

	float magnitude, duration;

	void behaviour(Player *player);
public:

	Flash(D3DXVECTOR3 pos, float scale, vector<Model*> models, ID3D10EffectTechnique* tech, Level* lvl, D3DXVECTOR3 rotation,
			float magnitude, float duration);
	~Flash();
};
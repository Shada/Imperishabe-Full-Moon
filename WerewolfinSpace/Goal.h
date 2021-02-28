#pragma once
#include "PowerUp.h"

class Goal :
	public PowerUp
{
private:

	void behaviour(Player *player);
public:

	Goal(D3DXVECTOR3 position, float scale, vector<Model*> model, ID3D10EffectTechnique* tech, Level *lvl,D3DXVECTOR3 rotation);
	~Goal();
};
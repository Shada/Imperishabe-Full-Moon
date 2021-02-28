#pragma once
#include "PowerUp.h"

class level;
class Clock :
	public PowerUp
{
private:
	float magnitude;

	void behaviour(Player *player);

public:
	Clock(D3DXVECTOR3 position, float scale, vector<Model*> model, ID3D10EffectTechnique* tech, float magnitude, Level *lvl,D3DXVECTOR3 rotation);
	~Clock();
};
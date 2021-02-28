#include "Flash.h"
#include "Player.h"

Flash::Flash(D3DXVECTOR3 pos, float scale, vector<Model*> models, ID3D10EffectTechnique* tech, Level* lvl, D3DXVECTOR3 rotation,
			float magnitude, float duration)
	:PowerUp( pos, scale, models, tech, lvl, rotation)
{
	this->magnitude = magnitude;
	this->duration = duration;

	rotationSpeed = 5;
}

void Flash::behaviour(Player *player)
{
	// The player recieves a boost that increases the speed limit and the accelration, but not the retardationfactor
	// -> run and accelerate faster, but glide more
	player->boost(magnitude, duration);
}

Flash::~Flash()
{
}

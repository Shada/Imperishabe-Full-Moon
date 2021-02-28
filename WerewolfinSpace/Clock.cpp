#include "Clock.h"
#include "Level.h"

Clock::Clock( D3DXVECTOR3 position, float scale, vector<Model*> model, ID3D10EffectTechnique* tech, float magnitude, Level *lvl,D3DXVECTOR3 rotation )
	: PowerUp( position, scale, model, tech, lvl, rotation )
{
	this->magnitude = magnitude;
	rotationSpeed = 5;
}

void Clock::behaviour( Player *player )
{
	lvl->decreaseTimePassed( magnitude );
}

Clock::~Clock()
{
}
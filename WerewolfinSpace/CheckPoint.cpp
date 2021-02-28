#include "CheckPoint.h"
#include "Level.h"

CheckPoint::CheckPoint(D3DXVECTOR3 position, float scale, vector<Model*> model, ID3D10EffectTechnique* tech, 
					Level *lvl, D3DXVECTOR3 rotation)
	: PowerUp(position, scale, model, tech, lvl, rotation )
{
	rotationSpeed = 5;
}

void CheckPoint::behaviour(Player *player)
{
	// Next time the player dies, he gets transported to this position
	player->setCheckPoint(position);
}

CheckPoint::~CheckPoint()
{
}
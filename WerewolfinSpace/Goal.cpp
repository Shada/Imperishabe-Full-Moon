#include "Goal.h"
#include "Level.h"

Goal::Goal(D3DXVECTOR3 position, float scale, vector<Model*> model, ID3D10EffectTechnique* tech, Level *lvl,D3DXVECTOR3 rotation)
	:PowerUp(position,scale ,model, tech, lvl, rotation)
{
	rotationSpeed = 3;
	viewdistance = DEFAULT_VIEWDISTANCE;
}

void Goal::behaviour(Player *player)
{
	lvl->levelOver = true;
}

Goal::~Goal()
{
}

#pragma once
#include "CollisionObject.h"

class Level;
class PowerUp :
	public CollisionObject
{
protected:

	Level *lvl;
	float rotationSpeed;

	bool collision(Player *player);
	virtual void behaviour(Player *player) {;}
public:

	PowerUp(D3DXVECTOR3 position, float scale, vector<Model*> model, ID3D10EffectTechnique* tech, Level *lvl,D3DXVECTOR3 rotation);
	~PowerUp();

	virtual void Update(double dt, Object3D* player);
};
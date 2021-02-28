#pragma once
#include "Object3D.h"
class Player;

class CollisionObject :
	public Object3D
{
protected:

	virtual void behaviour(Player *player);
	AABB *collisionBox;
public:

	CollisionObject(D3DXVECTOR3 pos, vector<Model*> model, ID3D10EffectTechnique* tech, float scale, D3DXVECTOR3 rotation);
	~CollisionObject();
	
	virtual bool collision(Player *player);

	AABB *getCollisionBox() { return collisionBox; }

	void Draw(DxHandler* h);
	virtual void Update(double dt, Object3D* player);
};
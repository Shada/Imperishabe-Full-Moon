#include "MovingPlatform.h"


MovingPlatform::MovingPlatform(D3DXVECTOR3 pos, vector<Model*> model, ID3D10EffectTechnique* tech, float scale, D3DXVECTOR3 rotation, vector<D3DXVECTOR3> destinations, float speed)
	: CollisionObject(pos, model, tech, scale, rotation * (PI * 0.5f))
{
	SAFE_DELETE(collisionBox);

	this->speed = speed;
	this->destinations = destinations;

	collisionBox = new AABB(position, &min, &max, scale, rotation.y);
	viewdistance = PLATFORM_VIEWDISTANCE;
	currentDest = 0;
	dt = 0;

	bigMin = D3DXVECTOR3(0,0,0);
	bigMax = D3DXVECTOR3(0,0,0);
	changeDirection();
}

void MovingPlatform::Update(double dt, Object3D* player)
{
	float distance = D3DXVec3Length(&(destinations.at(currentDest)-position));
	if(distance < speed*dt)
	{
		position = destinations.at(currentDest);
		
		if((unsigned)currentDest + 1 >= destinations.size())
			currentDest = 0;
		else
			currentDest++;

		changeDirection();
	}
	else
		position += direction * dt * speed;

	collisionBox->updatePos( position );
	mWorld._41 = position.x;
	mWorld._42 = position.y;
	mWorld._43 = position.z;
	this->dt = dt;		
}

void MovingPlatform::behaviour(Player *player)
{
	player->setPos(player->getPos() + direction*dt*speed);
	player->setNotMovingState( collisionBox->getBoxTopCoord() );
}

bool MovingPlatform::collision(Player *player)
{
	if(player->getCollisionBox()->intersectsXZ(collisionBox))
	{
		if(player->getPos().y <= collisionBox->getBoxTopCoord()+abs(direction.y*dt*speed*1.01f) &&
			player->getPos().y >= collisionBox->getBoxTopCoord()-abs(direction.y*dt*speed*1.01f) &&
			!player->isJumping())
		{
			behaviour(player);
		}
		else if(player->getCollisionBox()->intersectsY(collisionBox))
		{
			if(player->getPrevYPos() >= collisionBox->getBoxTopCoord()-abs(direction.y*dt*speed*1.01f))
				behaviour(player);
			else if(player->getPrevYPos() + player->getCollisionBox()->getBoxTopCoord() - player->getCollisionBox()->getBoxBottomCoord() 
				< collisionBox->getBoxBottomCoord())
		{	
				player->setToPrevYPos();
				player->setFallSpeed(-1);
			}
			else
				player->bounceOff(collisionBox);
			return true;
		}

		return false;	
	}
	return false;
}

void MovingPlatform::buildBigBox()
{
	bigMin = D3DXVECTOR3(0,0,0);
	bigMax = D3DXVECTOR3(0,0,0);

	for(unsigned int i = 0; i < destinations.size(); i++)
	{
		if(destinations.at(i).x - min.x < bigMin.x)
			bigMin.x = min.x;
		else if(destinations.at(i).x + max.x > bigMax.x)
			bigMax.x = max.x;
		if(destinations.at(i).y - min.y < bigMin.y)
			bigMin.y = min.y;
		else if(destinations.at(i).y + max.y > bigMax.y)
			bigMax.y = max.y;
		if(destinations.at(i).z - min.z < bigMin.z)
			bigMin.z = min.z;
		else if(destinations.at(i).z + max.z > bigMax.z)
			bigMax.z = max.z;
	}
}

void MovingPlatform::changeDirection()
{
	direction = destinations.at(currentDest) - position;
	D3DXVec3Normalize(&direction,&direction);
}

MovingPlatform::~MovingPlatform()
{
}

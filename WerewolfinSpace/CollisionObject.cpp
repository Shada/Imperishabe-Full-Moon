#include "CollisionObject.h"
#include "Player.h"

CollisionObject::CollisionObject(D3DXVECTOR3 pos, vector<Model*> model, ID3D10EffectTechnique* tech, float scale,D3DXVECTOR3 rotation)
    : Object3D(-2,-2,-2,pos, model, tech, scale,rotation)
{
    collisionBox = new AABB(position, D3DXVECTOR3(1, 1, 1), true, rotation.y);
}

void CollisionObject::Update(double dt,Object3D* player)
{
}

//check if the player hits it 
bool CollisionObject::collision(Player *player)
{
    if(player->getCollisionBox()->intersects(collisionBox))
    {
        if(player->getPrevYPos() >= collisionBox->getBoxTopCoord())
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

void CollisionObject::behaviour(Player *player)
{
    return;
}

void CollisionObject::Draw( DxHandler* h )
{
    // Uncomment to reveal the hitboxes of all collidable objects
    //collisionBox->DrawHitBox(h);

    Object3D::Draw(h);
}

CollisionObject::~CollisionObject()
{
    SAFE_DELETE(collisionBox);
}

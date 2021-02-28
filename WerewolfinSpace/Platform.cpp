#include "Platform.h"
#include "Player.h"

Platform::Platform(D3DXVECTOR3 pos, vector<Model*> model, ID3D10EffectTechnique* tech, float scale, D3DXVECTOR3 rotation)
    : CollisionObject(pos, model, tech, scale, rotation * (PI * 0.5f))
{
    SAFE_DELETE(collisionBox);

    collisionBox = new AABB(position, &min, &max, scale, rotation.y);
    viewdistance = PLATFORM_VIEWDISTANCE;
}

void Platform::behaviour(Player *player)
{
    player->setNotMovingState( collisionBox->getBoxTopCoord() );
}

Platform::~Platform()
{
}
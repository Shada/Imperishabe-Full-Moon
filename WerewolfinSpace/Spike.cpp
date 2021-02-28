#include "Spike.h"
#include "Player.h"

Spike::Spike(D3DXVECTOR3 pos, vector<Model*> model, ID3D10EffectTechnique* tech, float scale, D3DXVECTOR3 rotation)
    :CollisionObject(pos,model,tech,scale,rotation)
{
    SAFE_DELETE(collisionBox);
    collisionBox = new AABB(position, &min, &max, scale, rotation.y);
}
void Spike::behaviour(Player *player)
{
    player->death();
}

Spike::~Spike()
{
}
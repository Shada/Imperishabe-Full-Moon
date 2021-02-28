#include "Tree.h"
#include "Player.h"

Tree::Tree(D3DXVECTOR3 pos, vector<Model*> model, ID3D10EffectTechnique* tech, float scale, D3DXVECTOR3 rotation)
    : CollisionObject(pos, model, tech, scale, rotation)
{
    collisionBox = new AABB(pos, D3DXVECTOR3(25,300,25) * scale, false, rotation.y);
    viewdistance = TREE_VIEWDISTANCE;
}

void Tree::behaviour(Player* player)
{
    player->bounceOff(collisionBox);
}
Tree::~Tree()
{
}

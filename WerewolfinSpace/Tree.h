#pragma once
#include "collisionobject.h"

class Tree :
    public CollisionObject
{
private:

    void behaviour(Player* player);
public:

    Tree(D3DXVECTOR3 pos, vector<Model*> model, ID3D10EffectTechnique* tech, float scale, D3DXVECTOR3 rotation);
    ~Tree();    
};
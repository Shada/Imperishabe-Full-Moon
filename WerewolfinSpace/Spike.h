#pragma once
#include "CollisionObject.h"
class Spike :
    public CollisionObject
{
private:

    void behaviour(Player *player);
public:

    Spike(D3DXVECTOR3 pos, vector<Model*> model, ID3D10EffectTechnique* tech, float scale, D3DXVECTOR3 rotation);
    ~Spike();
};
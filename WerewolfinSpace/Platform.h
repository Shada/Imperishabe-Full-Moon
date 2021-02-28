#pragma once
#include "CollisionObject.h"
class Platform :
    public CollisionObject
{
protected:

    virtual void behaviour(Player *player);
public:

    Platform(D3DXVECTOR3 pos, vector<Model*> model, ID3D10EffectTechnique* tech, float scale, D3DXVECTOR3 rotation);    
    ~Platform();    
};
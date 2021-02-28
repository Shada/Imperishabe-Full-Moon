#pragma once
#include "CollisionObject.h"

class Springboard :
    public CollisionObject
{
private:

    float magnitude;
    double timer;
    int modelIndex, nextModelIndex;

    enum MorphState
    {
        mCoiled,
        mUncoiled,
        mStayUncoiled,
        mReturn
    };
    MorphState morphState;

    void behaviour(Player *player);
public:

    Springboard(D3DXVECTOR3 pos, vector<Model*> model, ID3D10EffectTechnique* tech, float scale, float magnitude,D3DXVECTOR3 rotation);    
    ~Springboard();

    void Draw(DxHandler* h);
    void Update(double dt, Object3D *player);
};
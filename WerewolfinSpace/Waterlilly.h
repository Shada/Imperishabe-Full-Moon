#pragma once
#include "platform.h"
class Waterlilly :
    public Platform
{
private:

    double timer;
    int modelIndex, nextModelIndex;

    enum MorphState
    {
        mDefault,
        mStepped,
        mWait,
        mReturn
    };
    MorphState morphState;

    void behaviour(Player *player);
public:
    Waterlilly(D3DXVECTOR3 pos,vector<Model*> model, ID3D10EffectTechnique* tech, float scale,D3DXVECTOR3 rotation);
    ~Waterlilly();

    void Draw(DxHandler* h);
    void Update(double dt, Object3D *player);
};
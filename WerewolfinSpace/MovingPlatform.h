#pragma once
#include "CollisionObject.h"
#include "Player.h"
class MovingPlatform
    :public CollisionObject
{
private:

    float speed, dt;
    int currentDest;

    vector<D3DXVECTOR3> destinations;
    D3DXVECTOR3 direction, bigMin, bigMax;

    void buildBigBox();
    void changeDirection();
    void behaviour(Player *player);
public:

    MovingPlatform(D3DXVECTOR3 pos, vector<Model*> model, ID3D10EffectTechnique* tech, float scale, D3DXVECTOR3 rotation, vector<D3DXVECTOR3> destinations, float speed);
    ~MovingPlatform();

    void Update(double dt, Object3D* player);
    bool collision(Player *player);

    D3DXVECTOR3 getMax() { return bigMax; }
    D3DXVECTOR3 getMin() { return bigMin; }
};
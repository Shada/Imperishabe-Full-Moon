#pragma once
#include "stdafx.h"
#include "Tree.h"
#include "AABB.h"
class Terrain;
class Player;
class QuadNode : 
    public AABB
{
private:

    D3DXPLANE* lastFailed;
    vector<QuadNode> nodes;
    vector<Object3D*> objects;

    D3DXVECTOR2 area;
    D3DXVECTOR2 pos;
    Camera* camera;
    Terrain* terrain;

    int subTerrainIndex;
    bool frustumAABBintersect();
    bool planeAABBIntersect(D3DXPLANE *plane);
public:

    QuadNode(int subLevels, D3DXVECTOR2 area, D3DXVECTOR2 pos, Camera* camera);
    ~QuadNode();

    bool hitTest(Object3D* object);
    bool removeObject(Object3D* object);
    void CheckCollision(Player* player);

    void DrawTerrain(DxHandler* h);
    void Draw(DxHandler* h, D3DXVECTOR3 plPos);

    void addObject(Object3D* object);
    void addTerrain(Terrain* t, ID3D10Device *device);
};
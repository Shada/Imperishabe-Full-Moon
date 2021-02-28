#pragma once
#include "stdafx.h"
#include "object3d.h"
class Block :
    public Object3D
{
private:

    D3DXVECTOR3 dimensions;
    
    vector<D3DXVECTOR3*>* vertexPoints;
    vector<D3DXVECTOR2*>* texCoords;
    vector<D3DXVECTOR3*>* normals;
    
    vector<int> vertexIndex;
    vector<int> texCoordIndex;
    vector<int> normalIndex;
public:
    Block(D3DXVECTOR3 dimensions, D3DXVECTOR3 position, ID3D10EffectTechnique* tech, ID3D10Device* g_pDevice);
    virtual ~Block();

    void createVertices();
    void setIndices();
    
    D3DXVECTOR3 getDimensions() { return dimensions; }
};
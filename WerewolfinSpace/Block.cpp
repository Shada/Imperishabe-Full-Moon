#include "Block.h"

//the -2 values are for modelIDs and such, to be able to write new object resently added in the edit mode directly to
//the level file, this is not implemented/used yet. 
Block::Block(D3DXVECTOR3 dimensions, D3DXVECTOR3 position, ID3D10EffectTechnique* tech, ID3D10Device* g_pDevice)
    :Object3D(-2,-2,-2,position,tech, D3DXVECTOR3(0,0,0))
{
    this->dimensions = dimensions;

    vertexPoints = new vector<D3DXVECTOR3*>();
    normals = new vector<D3DXVECTOR3*>();
    texCoords = new vector<D3DXVECTOR2*>();

    vertexIndex = vector<int>();
    normalIndex = vector<int>();
    texCoordIndex = vector<int>();

    createVertices();

    //give the object a model, is a vector because there are objects that uses more than one model
    model.push_back(new Model("void"));

    vector<PolygonGroup>* groups = model.back()->getGroups();

    groups->push_back(PolygonGroup("default"));
    Material *material = new Material("default");
    groups->back().material = material;

    for(unsigned int i = 0; i < vertexIndex.size(); i++)
        groups->back().feedData(vertexPoints->at(vertexIndex[i]), texCoords->at(texCoordIndex[i]), normals->at(normalIndex[i]));

    
    model.back()->feedData();
    model.back()->createBuffer(g_pDevice);

    numberOfVertices = 36;
}

void Block::createVertices()
{
    vertexPoints->push_back(new D3DXVECTOR3(- dimensions.x * .5f, - dimensions.y * .5f, - dimensions.z * .5f));
    vertexPoints->push_back(new D3DXVECTOR3(dimensions.x * .5f, - dimensions.y * .5f, - dimensions.z * .5f));
    vertexPoints->push_back(new D3DXVECTOR3(dimensions.x * .5f, - dimensions.y * .5f, dimensions.z * .5f));  
    vertexPoints->push_back(new D3DXVECTOR3(- dimensions.x * .5f, - dimensions.y * .5f, dimensions.z * .5f));

    vertexPoints->push_back(new D3DXVECTOR3(- dimensions.x * .5f, dimensions.y * .5f, - dimensions.z * .5f));
    vertexPoints->push_back(new D3DXVECTOR3(dimensions.x * .5f, dimensions.y * .5f, - dimensions.z * .5f));
    vertexPoints->push_back(new D3DXVECTOR3(dimensions.x * .5f, dimensions.y * .5f, dimensions.z * .5f));  
    vertexPoints->push_back(new D3DXVECTOR3(- dimensions.x * .5f, dimensions.y * .5f, dimensions.z * .5f));

    
    texCoords->push_back(new D3DXVECTOR2(0, 0));
    texCoords->push_back(new D3DXVECTOR2(1, 0));
    texCoords->push_back(new D3DXVECTOR2(0, 1));
    texCoords->push_back(new D3DXVECTOR2(1, 1));

    
    normals->push_back(new D3DXVECTOR3(-1,0,0));
    normals->push_back(new D3DXVECTOR3(1,0,0));
    normals->push_back(new D3DXVECTOR3(0,-1,0));
    normals->push_back(new D3DXVECTOR3(0,1,0));
    normals->push_back(new D3DXVECTOR3(0,0,-1));
    normals->push_back(new D3DXVECTOR3(0,0,1));

    setIndices();
}

void Block::setIndices()
{
    //up
    vertexIndex.push_back(0);
    vertexIndex.push_back(3);
    vertexIndex.push_back(2);

    texCoordIndex.push_back(0);
    texCoordIndex.push_back(2);
    texCoordIndex.push_back(3);

    vertexIndex.push_back(0);
    vertexIndex.push_back(2);
    vertexIndex.push_back(1);

    texCoordIndex.push_back(0);
    texCoordIndex.push_back(3);
    texCoordIndex.push_back(1);

    for(int i = 0; i < 6; i++)
        normalIndex.push_back(3);
    
    //near
    vertexIndex.push_back(4);
    vertexIndex.push_back(0);
    vertexIndex.push_back(1);
    
    texCoordIndex.push_back(0);
    texCoordIndex.push_back(2);
    texCoordIndex.push_back(3);

    vertexIndex.push_back(4);
    vertexIndex.push_back(1);
    vertexIndex.push_back(5);
    
    texCoordIndex.push_back(0);
    texCoordIndex.push_back(3);
    texCoordIndex.push_back(1);

    for(int i = 0; i < 6; i++)
        normalIndex.push_back(5);
    
    //left
    vertexIndex.push_back(7);
    vertexIndex.push_back(3);
    vertexIndex.push_back(0);
    
    texCoordIndex.push_back(0);
    texCoordIndex.push_back(2);
    texCoordIndex.push_back(3);

    vertexIndex.push_back(7);
    vertexIndex.push_back(0);
    vertexIndex.push_back(4);
    
    texCoordIndex.push_back(0);
    texCoordIndex.push_back(3);
    texCoordIndex.push_back(1);


    for(int i = 0; i < 6; i++)
        normalIndex.push_back(1);
    
    //far
    vertexIndex.push_back(6);
    vertexIndex.push_back(2);
    vertexIndex.push_back(3);

    texCoordIndex.push_back(0);
    texCoordIndex.push_back(2);
    texCoordIndex.push_back(3);

    vertexIndex.push_back(6);
    vertexIndex.push_back(3);
    vertexIndex.push_back(7);

    texCoordIndex.push_back(0);
    texCoordIndex.push_back(3);
    texCoordIndex.push_back(1);

    for(int i = 0; i < 6; i++)
        normalIndex.push_back(4);
    
    //right
    vertexIndex.push_back(5);
    vertexIndex.push_back(1);
    vertexIndex.push_back(2);

    texCoordIndex.push_back(0);
    texCoordIndex.push_back(2);
    texCoordIndex.push_back(3);

    vertexIndex.push_back(5);
    vertexIndex.push_back(2);
    vertexIndex.push_back(6);

    texCoordIndex.push_back(0);
    texCoordIndex.push_back(3);
    texCoordIndex.push_back(1);

    for(int i = 0; i < 6; i++)
        normalIndex.push_back(0);
    
    //bottom
    vertexIndex.push_back(7);
    vertexIndex.push_back(4);
    vertexIndex.push_back(5);

    texCoordIndex.push_back(0);
    texCoordIndex.push_back(2);
    texCoordIndex.push_back(3);

    vertexIndex.push_back(7);
    vertexIndex.push_back(5);
    vertexIndex.push_back(6);

    texCoordIndex.push_back(0);
    texCoordIndex.push_back(3);
    texCoordIndex.push_back(1);

    for(int i = 0; i < 6; i++)
        normalIndex.push_back(2);
}

Block::~Block()
{
    for(unsigned int i = 0; i < vertexPoints->size(); i++)
        SAFE_DELETE(vertexPoints->at(i));
    for(unsigned int i = 0; i < normals->size(); i++)
        SAFE_DELETE(normals->at(i));
    for(unsigned int i = 0; i < texCoords->size(); i++)
        SAFE_DELETE(texCoords->at(i));
    SAFE_DELETE(normals);
    SAFE_DELETE(vertexPoints);
    SAFE_DELETE(texCoords);
}
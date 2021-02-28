#include "PolygonGroup.h"


PolygonGroup::PolygonGroup(string name)
{
    this->name = name;
    bufferIndex = 0;
    material = new Material((char*)"default");
}

void PolygonGroup::Draw( DxHandler* h, ID3D10EffectTechnique* tech )
{
    //set shader variables
    material->useShaderVariables( h->getID3D10Effect() );
    //draw vertex points at specified range from pdata
    //can use the same indices because the groups will look the same
    h->Render( tech, bufferIndex, getVertexAmount() );
}
void PolygonGroup::feedData(D3DXVECTOR3* vertex, D3DXVECTOR2* textureCoordinate, D3DXVECTOR3* normal)
{
    LineVertex ver;
    ver.pos = *vertex;
    ver.normal = *normal;
    ver.texCoord = *textureCoordinate;
    vertices.push_back(ver);
}
void PolygonGroup::feedToPData(LineVertex* pData)
{
    //saves the start position and size of this range in pdata
    //to make it more accessable
    int currentPDataPos = bufferIndex;
    for(unsigned int i = 0; i < vertices.size(); i++)
        pData[currentPDataPos++] = vertices.at(i);
}

PolygonGroup::~PolygonGroup()
{
}
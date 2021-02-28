#include "AABB.h"


AABB::AABB(D3DXVECTOR3 position, D3DXVECTOR3 dimensions, bool centerOrigin, float angle)
{
    this->dim = dimensions;
    
    //if the object have its origin in the center, the factor is 1, else 0. 
    //this will make the AABB surrounding the object correctly
    this->factor = centerOrigin ? 1 : 0;
    pVB = NULL;        
    pData = NULL;
    //In itiate and create the bounding box
    init(position, angle);

}
AABB::AABB(D3DXVECTOR3 position, D3DXVECTOR3 *posMin, D3DXVECTOR3 *posMax, float scale, float angle)
{
    pVB = NULL;
    pData = NULL;
    factor = 1;

    //Calculate the dimensions from the min and max positions
    //The posmin and posmax are the distance from the origin of the model
    dim.x = (posMax->x - posMin->x)*scale;
    dim.y = (posMax->y - posMin->y)*scale;
    dim.z = (posMax->z - posMin->z)*scale;

    //Angles: 0 = 0 deg, 1 = 90 deg, 2 = 180 deg, 3 = 270 deg.
    if(angle == 1)
    {
        float temp = posMin->z;
        posMin->z = posMin->x;
        posMin->x = temp;

        temp = posMax->z;
        posMax->z = posMax->x;
        posMax->x = temp;        
    }
    else if(angle == 2)
    {
        float temp = posMin->z * -1;
        posMin->z = posMax->z * -1;
        posMax->z = temp;
    }
    else if(angle == 3)
    {
        float temp = posMin->z * -1;
        posMin->z = posMax->x * -1;
        posMax->x = temp;

        temp = posMax->z * -1;
        posMax->z = posMin->x * -1;
        posMin->x = temp;
    }
    // scale the pos min and max
    *posMax *= scale;
    *posMin *= scale;

    // set the min and max positions for the bounding box
    min = D3DXVECTOR3(position.x + posMin->x, position.y + posMin->y, position.z + posMin->z);
    max = D3DXVECTOR3(position.x + posMax->x, position.y + posMax->y, position.z + posMax->z);
}
void AABB::init(D3DXVECTOR3 pos, float angle)
{        
    // set the min and max positions for the bounding box
    min = D3DXVECTOR3(pos.x -dim.x / 2, pos.y -(dim.y / 2) * factor, pos.z -dim.z / 2);
    max = D3DXVECTOR3(pos.x + dim.x / 2, pos.y + (dim.y / 2),pos.z + dim.z / 2);
}

void AABB::updatePos(D3DXVECTOR3 position)
{
    //when an object is moving, we have to update the bouning box, 
    //so that it follows the object
    min.x = position.x - dim.x / 2;
    min.y = position.y - (dim.y / 2) * factor;
    min.z = position.z - dim.z / 2;

    max.x = position.x + dim.x / 2;
    max.y = position.y + dim.y / 2;
    max.z = position.z + dim.z / 2;
}

//This method is used when we want to draw bounding box. This is usable in 
//debuging purposes when we want to check if collisions work correctly and if the bounding
//boxes are aligned correctly
void AABB::DrawHitBox(DxHandler* h)
{
    int index = 0;

    if(!pVB)
    {
        D3D10_BUFFER_DESC bd;
        bd.Usage = D3D10_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof( LineVertex ) * 24;
        bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
        bd.MiscFlags = 0;

    
        if( FAILED( h->getID3D10Device()->CreateBuffer( &bd, 0, &pVB ) ) )
            MessageBox( 0, "Unable to create Vertex Buffer", "VB Error", 0 );
    }
    pData = NULL;
    pVB->Map(D3D10_MAP_WRITE_DISCARD, 0, reinterpret_cast< void** >((void**)&pData));

    pData[index++].pos = D3DXVECTOR3(min.x, min.y, min.z);
    pData[index++].pos = D3DXVECTOR3(max.x, min.y, min.z);
    
    pData[index++].pos = D3DXVECTOR3(max.x, max.y, min.z);
    pData[index++].pos = D3DXVECTOR3(min.x, max.y, min.z);

    pData[index++].pos = D3DXVECTOR3(min.x, min.y, max.z);
    pData[index++].pos = D3DXVECTOR3(max.x, min.y, max.z);

    pData[index++].pos = D3DXVECTOR3(max.x, max.y, max.z);
    pData[index++].pos = D3DXVECTOR3(min.x, max.y, max.z);

    pData[index++].pos = D3DXVECTOR3(min.x, min.y, min.z);
    pData[index++].pos = D3DXVECTOR3(min.x, max.y, min.z);

    pData[index++].pos = D3DXVECTOR3(min.x, min.y, min.z);
    pData[index++].pos = D3DXVECTOR3(min.x, min.y, max.z);
    
    pData[index++].pos = D3DXVECTOR3(min.x, max.y, min.z);
    pData[index++].pos = D3DXVECTOR3(min.x, max.y, max.z);
    
    pData[index++].pos = D3DXVECTOR3(min.x, max.y, max.z);
    pData[index++].pos = D3DXVECTOR3(min.x, min.y, max.z);
    
    pData[index++].pos = D3DXVECTOR3(max.x, min.y, min.z);
    pData[index++].pos = D3DXVECTOR3(max.x, max.y, min.z);
    
    pData[index++].pos = D3DXVECTOR3(max.x, max.y, min.z);
    pData[index++].pos = D3DXVECTOR3(max.x, max.y, max.z);
    
    pData[index++].pos = D3DXVECTOR3(max.x, max.y, max.z);
    pData[index++].pos = D3DXVECTOR3(max.x, min.y, max.z);
    
    pData[index++].pos = D3DXVECTOR3(max.x, min.y, min.z);
    pData[index++].pos = D3DXVECTOR3(max.x, min.y, max.z);

    pVB->Unmap();
    

    D3DXMATRIX mWorld;
    D3DXMatrixIdentity(&mWorld);
    h->sendWorldMatrix(mWorld);
    UINT stride = sizeof(LineVertex);
    UINT offset = 0;

    h->getID3D10Device()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
    h->getID3D10Device()->IASetVertexBuffers( 0, 1, &pVB, &stride, &offset );
    h->getID3D10Device()->IASetInputLayout( h->g_pVertexLayout );
    
    D3D10_TECHNIQUE_DESC techDesc;
    h->g_pTechRenderRedLine->GetDesc( &techDesc );
    for( UINT p = 0; p < techDesc.Passes; p++ )
    {
        h->g_pTechRenderRedLine->GetPassByIndex( p )->Apply(0);
        h->getID3D10Device()->Draw(24, 0);
    }
}
//Check intersection with another AABB
bool AABB::intersects(AABB *box)
{
    for(int c = 0; c < 3; c++)
        if(min[c] > box->getMax()[c] || box->getMin()[c] > max[c])
            return false;
    return true;
}
//This is mainly used for moving objects so that the collision from the sides
//and the collision from above and from the bottom is separated.
bool AABB::intersectsXZ(AABB *box)
{
    if(min.x > box->getMax().x || box->getMin().x > max.x)
        return false;
    if(min.z > box->getMax().z || box->getMin().z > max.z)
        return false;
    return true;
}
bool AABB::intersectsY(AABB *box)
{
    if(min.y > box->getMax().y || box->getMin().y > max.y)
        return false;
    return true;
}
AABB::~AABB()
{
    //Release the vertex buffer containing the lines
    SAFE_RELEASE(pVB);
    SAFE_DELETE(pData);
}
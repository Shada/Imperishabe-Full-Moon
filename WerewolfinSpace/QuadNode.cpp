#include "QuadNode.h"
#include "CollisionObject.h"
#include "Player.h"

QuadNode::QuadNode(int subLevels, D3DXVECTOR2 area, D3DXVECTOR2 pos, Camera* camera)
    :AABB( D3DXVECTOR3( pos.x, 0, pos.y ), &D3DXVECTOR3( 0, -1000, 0 ), &D3DXVECTOR3( area.x, 1000, area.y ), 1, 0 )
{
    
    this->camera = camera;
    terrain = NULL;
    this->area = area;
    this->pos = pos;

    lastFailed = camera->getFrustumPlaneAt(0);
    //create children as long as subleves is greater or equal to 0.
    if(subLevels >= 0)
    {
        --subLevels;
        for(int y = 0; y < 2; y++)
            for(int x = 0; x < 2; x++)
            {
                nodes.push_back(
                    QuadNode(
                    subLevels,
                    area / 2,
                    D3DXVECTOR2( pos.x + area.x * .5f*( x%2 ),
                                pos.y + area.y * .5f*( y%2 ) ),camera));
            }
    }
}
void QuadNode::addTerrain(Terrain* t, ID3D10Device *device)
{
    //devide the terrain so that they fit perfectly in the nodes that are 640x640 units 
    if( (int)(area.x + .5f) == 640)
    {
        subTerrainIndex = t->subTerrain(pos / 10,area / 10, device);
        terrain = t;
        return;
    }
    // if this QuadNode have children, check if the object is in one of those.
    if(nodes.size() > 0)
        for(unsigned int i = 0; i < nodes.size(); i++)
        {
            nodes.at(i).addTerrain(t, device);
        }
}
void QuadNode::addObject(Object3D* object)
{
    // if this QuadNode have children, check if the object is in one of those.
    if(nodes.size() > 0)
        for(unsigned int i = 0; i < nodes.size(); i++)
        {
            if(nodes.at(i).hitTest(object))
            {
                // if it is in one of the children, the object 
                //will be added there
                nodes.at(i).addObject(object);
                return;
            }
        }
    //if there aren't any children or if the object is not
    //in any of the children, the object will be added here instead.
    objects.push_back(object);
}



bool QuadNode::removeObject(Object3D* object)
{
    //go through the nodes and look in their objectlists, if the object is found delete it
    //otherwise these are not the objects you are looking for.
    for(unsigned int i = 0; i < objects.size(); i++)
    {
        if(object == objects.at(i))
        {
            objects.erase(objects.begin() + i);
            return true;
        }
    }
    for(unsigned int i = 0; i < nodes.size(); i++)
    {
        if(nodes.at(i).removeObject(object))
            return true;
    }
    return false;

}
bool QuadNode::hitTest(Object3D* object)
{
    //check if the object is within the xz-area of the quadnode.
    return (object->getMin().x > pos.x &&
            object->getMax().x < pos.x + area.x &&
            object->getMin().z > pos.y &&
            object->getMax().z < pos.y + area.y);
}
bool QuadNode::planeAABBIntersect(D3DXPLANE *plane)
{
    if( D3DXPlaneDotCoord(plane, &max) < 0 && D3DXPlaneDotCoord(plane, &min) < 0)
    {
        D3DXVECTOR3 aCorner = D3DXVECTOR3(min.x, min.y, max.z);
        D3DXVECTOR3 bCorner = D3DXVECTOR3(max.x, max.y, min.z);
        if( D3DXPlaneDotCoord(plane, &aCorner) < 0 && D3DXPlaneDotCoord(plane, &bCorner) < 0 )
        {
            aCorner = D3DXVECTOR3(max.x, min.y, max.z);
            bCorner = D3DXVECTOR3(min.x, max.y, min.z);
            if( D3DXPlaneDotCoord(plane, &aCorner) < 0 && D3DXPlaneDotCoord(plane, &bCorner) < 0 )
            {
                aCorner = D3DXVECTOR3(max.x, min.y, min.z);
                bCorner = D3DXVECTOR3(min.x, max.y, max.z);
                if( D3DXPlaneDotCoord(plane, &aCorner) < 0 && D3DXPlaneDotCoord(plane, &bCorner) < 0 )
                {
                    lastFailed = plane;
                    return false;
                }
            }
        }
    }
    return true;
}
bool QuadNode::frustumAABBintersect()
{
    //check if the aabb intersects every one of the frustum planes 
    for( int k = 0; k < 6; k++ )
    {
        if(!planeAABBIntersect(camera->getFrustumPlaneAt(k)))
            return false;
    }
    return true;
}

void QuadNode::CheckCollision(Player* player)
{
    if(intersects(player->getCollisionBox()))
    {
        //check collision for all collision objects
        for(unsigned int i = 0; i < objects.size(); i++)
        {
            if( CollisionObject* t = dynamic_cast<CollisionObject*>(objects.at(i)))
                t->collision(player);
        }
        //check all the children
        for(unsigned int i = 0; i < nodes.size(); i++)
            nodes.at(i).CheckCollision(player);
    }
}
void QuadNode::DrawTerrain(DxHandler* h)
{
    if(!planeAABBIntersect(lastFailed))
        return;
    if(frustumAABBintersect())
    {
        //if this is the quadlevel with the terrain draw it
        if(terrain)
        {
            terrain->Draw(h,subTerrainIndex);
            return;
        }
        else
        {
            //draw all the children
            for(unsigned int i = 0; i < nodes.size(); i++)
                nodes.at(i).DrawTerrain(h);
        }
    }

}
void QuadNode::Draw(DxHandler* h, D3DXVECTOR3 plPos)
{
    if(!planeAABBIntersect(lastFailed))
        return;
    //if( collide with frustum)
    if(frustumAABBintersect())
    {
        //draw all the objects in the node
        for(unsigned int i = 0; i < objects.size(); i++)
        {
            float fadedist = (float)objects.at(i)->getViewDistance();
            float dist = D3DXVec3Length(&(plPos - objects.at(i)->getPos())); 
            if( dist < fadedist)
            {
                float fadeweight = dist - (fadedist * 0.8f);
                if(fadeweight > 0 )
                {
                    fadeweight = 1- (fadeweight /= fadedist - (fadedist * 0.8f));
                    h->getID3D10Effect()->GetVariableByName( "fadefactor" )->AsScalar()->SetFloat( (float)fadeweight );
                }
                else
                    h->getID3D10Effect()->GetVariableByName( "fadefactor" )->AsScalar()->SetFloat( 1 );
                objects.at(i)->Draw(h);
            }
        }
        //draw all the children
        for(unsigned int i = 0; i < nodes.size(); i++)
            nodes.at(i).Draw(h, plPos); 
    }
    //DrawHitBox(h);
}

QuadNode::~QuadNode()
{
    lastFailed = NULL;
    camera = NULL;
    terrain = NULL;

    for( unsigned int i = 0; i < objects.size(); i++)
        objects.at(i) = NULL;

    for( unsigned int i = 0; i < nodes.size(); i++ )
        nodes.at(i).~QuadNode();
}


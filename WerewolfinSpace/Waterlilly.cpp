#include "Waterlilly.h"

Waterlilly::Waterlilly( D3DXVECTOR3 pos, vector<Model*> model, ID3D10EffectTechnique* tech, 
                        float scale, D3DXVECTOR3 rotation )
    : Platform( pos, model, tech, scale, rotation )
{
    morphState = mDefault;

    timer = 0;
}

void Waterlilly::behaviour( Player* player )
{
    Platform::behaviour( player );
    if( morphState != mWait )
        morphState = mStepped;
}

void Waterlilly::Update( double dt, Object3D* player )
{
    CollisionObject::Update( dt, player );
    switch( morphState )
    {
        case mStepped:
        {
            timer += dt * 9;
            if( timer >= 1 )
            {
                timer = 0;
                morphState = mWait;
            }
            break;
        }
        case mWait:
        {
            CollisionObject* fuling = (CollisionObject*)player;
            if( !fuling->getCollisionBox()->intersects( collisionBox ) )
            {
                morphState = mReturn;
                timer = 0;
            }
            break;
        }
        case mReturn:
        {
            timer += dt;
            if( timer >= 1 )
            {
                timer = 0;
                morphState = mDefault;
            }
            break;
        }
    }
}

void Waterlilly::Draw( DxHandler* h )
{
    h->sendWorldMatrix( mWorld );
    switch( morphState )
    {
        case mDefault:
        {
            model.at(0)->Draw( h, h->g_pTechRenderNoMoveLight );
            break;
        }
        case mStepped:
        {
            h->getID3D10Effect()->GetVariableByName( "time" )->AsScalar()->SetFloat( (float)timer );
            model.at(0)->Draw( h, tech, model.at(1) );
            break;
        }
        case mWait:
        {
            model.at(1)->Draw( h, h->g_pTechRenderNoMoveLight );
            break;
        }
        case mReturn:
        {
            h->getID3D10Effect()->GetVariableByName( "time" )->AsScalar()->SetFloat( (float)timer );
            model.at(1)->Draw( h, tech, model.at(0) );
            break;
        }
    }
}

Waterlilly::~Waterlilly()
{
}
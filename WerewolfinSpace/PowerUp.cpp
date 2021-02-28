#include "PowerUp.h"
#include "Player.h"
#include "Level.h"

PowerUp::PowerUp( D3DXVECTOR3 position, float scale, vector<Model*> model, ID3D10EffectTechnique* tech, Level *lvl, D3DXVECTOR3 rotation )
    : CollisionObject( position, model, tech, scale, rotation )
{
    collisionBox = new AABB( position, D3DXVECTOR3( 10, 12, 10 ) * scale, true, rotation.y );
    viewdistance = POWERUP_VIEWDISTANCE;

    this->lvl = lvl;
    rotationSpeed = 0;
}

bool PowerUp::collision( Player *player )
{
    if( player->getCollisionBox()->intersects( collisionBox ) )
    {
        this->behaviour( player );
        lvl->deleteThisObject( this );
        return true;
    }
    return false;
}

void PowerUp::Update( double dt, Object3D *player )
{
    D3DXMatrixRotationY( &mRotation, (float)( rotationSpeed * dt ) );
    mWorld = mRotation * mWorld;

    collision( (Player*)player );
}

PowerUp::~PowerUp()
{
}

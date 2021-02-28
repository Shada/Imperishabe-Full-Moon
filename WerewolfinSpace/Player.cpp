#include "Player.h"
#include "InputHandler.h"
#include "Camera.h"
#include <math.h>

Player::Player( D3DXVECTOR3 position, InputHandler* inputH, Camera* camera, Terrain* terrain, 
                ID3D10EffectTechnique* tech, vector<Model*> model, float scale, DxHandler *dxHandler )
    :CollisionObject( position, model, tech, scale, D3DXVECTOR3( 0, 0, 0 ) )
{
    this->input = inputH;
    this->camera = camera;
    this->terrain = terrain;
    this->checkPointPos = position;

    //used for the flash power-up.
    this->boostFactor = 1;
    this->boostDuration = 0; 
    //--if we want to extend this game we could use a power up list instead and have these kind of 
    //--variables in the player. And update them from the list.

    soundRun = new SoundHandler();
    soundLand = new SoundHandler();
    soundSpring = new SoundHandler();

    D3DXMatrixScaling( &mScale, scale, scale, scale );

    D3DX10CreateFont( dxHandler->getID3D10Device(), 40, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, 
                    ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Verdana", &font );

    initPlayer();

    soundRun->initialize(*dxHandler->getHWnd(), "footstep1.wav");
    soundLand->initialize(*dxHandler->getHWnd(), "landing.wav");
    soundSpring->initialize(*dxHandler->getHWnd(), "bounce.wav");
    
}

void Player::initPlayer()
{
    collisionBox = new AABB( position, D3DXVECTOR3( 2, 20, 2 ), false, vRotation.y );

    timer = fallSpeed = 0.f;
    modelIndex = nextModelIndex = 0;
    morphFreq = 0.5f;

    vSpeed = D3DXVECTOR2( 0, 0 );

    accelleration = 90.f;
    retardation = 8.0f;
    speedLimit = 45.0f;
    
    camera->setEyePt( position );

    vDirection = camera->getDirection();
    vPrevDirection = vDirection;

    state = jumping;
    morphState = mFalling;
    
    setDefaultControls();

    prevPos = position;
}

void Player::resetPlayer()
{
    fallSpeed = 0;
    vSpeed = D3DXVECTOR2( 0, 0 );

    state = jumping;

    camera->resetCamera();
    camera->setEyePt( position );

    collisionBox->updatePos(position);

    vDirection = camera->getDirection();
    D3DXMatrixRotationY( &mRotation, atan2( vDirection.x, vDirection.z ) );
}

void Player::setDefaultControls()
{
    cUp        = new CControl( DIK_W );
    cDown    = new CControl( DIK_S );
    cLeft    = new CControl( DIK_A );    
    cRight    = new CControl( DIK_D );
    cJump    = new CControl( DIK_SPACE );
}

void Player::Update( double dt )
{
    
    //set previous height
    prevPos = position;

    if( input->getDeltaZ() != 0 )
        camera->Zoom( input->getDeltaZ() );
    if( input->getDeltaX() != 0 )
        camera->Rotate( input->getDeltaX() );

    //suicide button
    if(input->getKeyState()[DIK_P] && !input->getPrevKeyState()[DIK_P])
        death();

    //used for the flash, and again if we want more powerup a list of powerups should be added
    if( boostDuration > 0)
    {
        //if a flash is activated and the boost duration is greater than 0 decrease the boost duration
        //until it is zero again and reset the speed limit to normal.
        boostDuration = boostDuration - (float)dt < 0 ? 0 : boostDuration - (float)dt;
        if(boostDuration == 0)
        {
            speedLimit /= boostFactor;
            boostFactor = 1;
        }
    }

    move(dt);

    /////////////////////////////
    //    used for cheating    //
    /////////////////////////////
    /*if(input->getKeyState()[DIK_H])
        setFallSpeed(-10);
    else if(input->getKeyState()[DIK_G])
        fallSpeed = 0;
    else if(input->getKeyState()[DIK_F])
        speedLimit = 150;
    else if(input->getPrevKeyState()[DIK_F])
        speedLimit = 45;*/

    collisionBox->updatePos( position );

    D3DXMatrixTranslation( &mTranslate, position.x, position.y, position.z );

    D3DXMatrixMultiply( &mWorld, &mScale, &mRotation );
    D3DXMatrixMultiply( &mWorld, &mWorld, &mTranslate );
    
    camera->Update( position, dt );
}

//used for the flash, called from flash power up
void Player::boost(float magnitude, float duration)
{ 
    // reset the speedlimit, in case we already have a boost, we don't want double boost
    speedLimit /= boostFactor; 
    //Set the boost factor
    boostFactor = magnitude;
    // multiply the sppedlimit with the boostfactor
    speedLimit *= boostFactor;
    //add the new boost duration. We want more duration if we take two flashes
    boostDuration += duration; 
}
void Player::death()
{
    position = checkPointPos;
    state = jumping;
    prevPos = position;

    vSpeed = D3DXVECTOR2( 0, 0 );
    fallSpeed = 0;

    camera->setEyePt( position );
}

void Player::bounceOff(AABB *collidingBox)
{
    // Create a temporary colliding box to reveal the axis in which the player collides
    AABB *temp = new AABB(D3DXVECTOR3(position.x, position.y, prevPos.z), D3DXVECTOR3( 2, 20, 2 ), false, vRotation.y);
    if(temp->intersects(collidingBox))
    {
        // Sets a legitimate speed when collidion with an object as well as sets the speed going against the object to 0
        vSpeed.y = vSpeed.y > 0 ? D3DXVec2Length(&vSpeed) - abs(vSpeed.x) : -(D3DXVec2Length(&vSpeed) - abs(vSpeed.x));
        vSpeed.x = 0;
        position.x = prevPos.x; 
    }
    else
    {
        SAFE_DELETE(temp);
        temp = new AABB(D3DXVECTOR3(prevPos.x, position.y, position.z), D3DXVECTOR3( 2, 20, 2 ), false, vRotation.y);
        if(temp->intersects(collidingBox))
        {
            // Same here, set a legitimate speed
            vSpeed.x = vSpeed.x > 0 ? D3DXVec2Length(&vSpeed) - abs(vSpeed.y) : -(D3DXVec2Length(&vSpeed) - abs(vSpeed.y));
            vSpeed.y = 0;
            position.z = prevPos.z;
        }
    }

    SAFE_DELETE(temp);
    fallSpeed *= fallSpeed > 5 ? 0.65f : 1.f;
}

void Player::morph(double dt)
{
    switch( morphState )
    {
        case mStanding:
        {
            timer += dt;
            if( timer >= 1 )
            {
                modelIndex = nextModelIndex;
                nextModelIndex++;
                if( nextModelIndex > 1 )
                {
                    nextModelIndex = 0;
                    modelIndex = 1;
                }
                else
                {
                    nextModelIndex = 1;
                    modelIndex = 0;
                }
                timer = 0.0;
            }
            break;
        }
        case mRunning:
        {
            timer += dt;
            if( timer >= morphFreq )
            {
                modelIndex = nextModelIndex;
                nextModelIndex++;
                if( nextModelIndex > 7 )
                    nextModelIndex = 2;
                if( modelIndex == 2 || modelIndex == 5)
                    soundRun->playSingle();

                timer = 0.0;
            }
            break;
        }
        case mJumping:
        {
            if( timer < 1.0 )
                timer += dt * 10.0;
            else
                timer = 1.0;
            break;
        }
        case mFalling:
        {
            //almost same as above
            if( timer < 1.0 )
                timer += dt * 2.0;
            else
                timer = 1.0;
            break;
        }
        default:
        {
            timer += dt * 5.f;
            if( timer >= 1.f )
            {
                morphState = mStanding;
                timer = 0.f;
                modelIndex = 0;
                nextModelIndex = 1;
            }
            break;
        }
    }
}

void Player::move( double dt )
{
    
    morph(dt);

    vDirection = camera->getDirection();
    D3DXVECTOR2 tempMove = D3DXVECTOR2( 0, 0 );
    
    if( input->getKeyState()[cUp->get()] & 0x80 )
    {
        tempMove.x += vDirection.x;
        tempMove.y += vDirection.z;
    }
    else if( input->getKeyState()[cDown->get()] & 0x80 )
    {
        tempMove.x -= vDirection.x;
        tempMove.y -= vDirection.z;
    }
    if( input->getKeyState()[cRight->get()] & 0x80 )
    {
        tempMove.x += vDirection.z;
        tempMove.y -= vDirection.x;
    }
    else if( input->getKeyState()[cLeft->get()] & 0x80 )
    {
        tempMove.x -= vDirection.z;
        tempMove.y += vDirection.x;
    }
    if( input->getKeyState()[cJump->get()] & 0x80 && state != jumping  && state != gliding )
    {
        fallSpeed = 60;
        state = jumping;
        position.y++;
    }

    if( state == gliding )
        glide();

    else if( D3DXVec2Length( &tempMove ) > 0 )
    {
        //normalize direction movement
        D3DXVec2Normalize( &tempMove, &tempMove );
        
        //accellerate
        vSpeed += tempMove * (float)( accelleration * dt );
        
        //check speed-limit
        if( D3DXVec2Length(&vSpeed) > speedLimit )
        {
            D3DXVec2Normalize( &vSpeed, &vSpeed );
            vSpeed *= speedLimit;
        }
        
        //rotate model
        D3DXMatrixRotationY( &mRotation, atan2( tempMove.x, tempMove.y ) );
        D3DXMatrixMultiply( &mWorld, &mWorld, &mRotation );
    }
    else if(D3DXVec2Length( &vSpeed) > 0 )
    {
        float retardationFactor = ( state != jumping ) ? 1 : .05f;

        //slow down if no movement is defined
        vSpeed -= vSpeed * (float)( retardation * retardationFactor * dt );
        
        //prevent negative movement
        if( D3DXVec2Length( &vSpeed ) < 0.1 )
            vSpeed = D3DXVECTOR2( 0, 0 );
    }
    float impactSpeed = fallSpeed;
    followTerrain( dt );

    //check terrain-angle
    if( angle < 0.77f && state != gliding && state != jumping && position.y > prevPos.y )
    {
        vSpeed /= 1 / triangleNormal.y * 2;
        float temp = abs( min( impactSpeed, 1 ) );
        vSpeed.x += triangleNormal.x * ( 1 / triangleNormal.y ) * ( temp + 1) * .2f;
        vSpeed.y += triangleNormal.z * ( 1 / triangleNormal.y ) * ( temp + 1 ) * .2f;
        state = gliding;
        //glide();
    }
    else if( angle >= 0.77f && state == gliding )
        state = notMoving;

    //move player
    position.x += (float)( vSpeed.x * dt );
    position.z += (float)( vSpeed.y * dt );
    morphFreq = (1.0 - D3DXVec2Length(&vSpeed) * 0.02 * 45.0 / speedLimit) / boostFactor;
    
    if( fallSpeed > 0 && state == jumping )
    {
        if( morphState != mJumping )
            timer = 0;
        morphState = mJumping;
    }
    else if( fallSpeed <= 0 && state == jumping )
    {
        if(morphState != mFalling)
            timer = 0;
        morphState = mFalling;
    }
    
    else if( D3DXVec2Length( &vSpeed ) > 5 && state != gliding )
    {
        
        if( morphState != mRunning )
        {
            soundRun->playSingle();
            morphState = mRunning;
            timer = 0;
            nextModelIndex = 2;
        }
    }
    else 
    {
        if( morphState == mRunning || morphState == mFalling )
        {
            timer = 0;
            soundRun->playSingle();
            morphState = mGoToStand;
            modelIndex = nextModelIndex;
            nextModelIndex = 0;
        }
        else if( morphState != mStanding && morphState != mGoToStand )
        {
            morphState = mStanding;
            timer = 0;
            nextModelIndex = 1;
        }
    }
}

void Player::Draw( DxHandler* h )
{    
    // http:// http.developer.nvidia.com/GPUGems3/gpugems3_ch03.html
    h->sendWorldMatrix( mWorld );
    h->getID3D10Effect()->GetVariableByName( "fadefactor" )->AsScalar()->SetFloat( (float)(1.0) );
    switch( morphState )
    {
        case mStanding:
            h->getID3D10Effect()->GetVariableByName( "time" )->AsScalar()->SetFloat( (float)timer );
            model.at( modelIndex )->Draw( h, tech, model.at( nextModelIndex ) );
            break;
        case mRunning:
            h->getID3D10Effect()->GetVariableByName( "time" )->AsScalar()->SetFloat( (float)timer / morphFreq );
            model.at( modelIndex )->Draw( h, tech, model.at( nextModelIndex ) );
            break;
        case mJumping:
        {
            h->getID3D10Effect()->GetVariableByName( "time" )->AsScalar()->SetFloat( (float)timer );
            model.at(0)->Draw( h, tech, model.at(8) );
            break;
        }
        case mFalling:
        {
            h->getID3D10Effect()->GetVariableByName( "time" )->AsScalar()->SetFloat( (float)timer );
            model.at(8)->Draw( h, tech, model.at(9) );
            break;
        }
        default:
        {
            h->getID3D10Effect()->GetVariableByName( "time" )->AsScalar()->SetFloat( (float)timer );
            model.at(modelIndex)->Draw( h, tech, model.at(0) );
            break;
        }
    }
    //uncomment if you want to get the hitbox for the player visualized
    //collisionBox->DrawHitBox( h );
}

void Player::followTerrain( double dt )
{
    //the cell where the player is
    int cellX = (int)( ( position.x - terrain->getPos().x ) / 10 );
    int cellY = (int)( ( position.z - terrain->getPos().z ) / 10 );

    //number of vertexpoints-width on terrain
    int terrainWidth = (int)terrain->getDimensions().x;
    
    //the index for down-left point in the cell
    index = cellY * terrainWidth + cellX;

    //down-left point in the cell
    D3DXVECTOR3 *closestPoint = terrain->getVertexPointAt( index );

    //distance from point to player
    float dx = position.x - closestPoint->x;
    float dz = position.z - closestPoint->z;

    int tempIndex;

    //check what triangle player is in
    if(dx < dz)
        tempIndex = index + terrainWidth;

    else if(dx >= dz)
        tempIndex = index + 1;

    //calculate normal for the triangle player is in
    D3DXVECTOR3 v0v1 = *closestPoint - *terrain->getVertexPointAt( tempIndex );
    D3DXVECTOR3 v0v2 = *closestPoint - *terrain->getVertexPointAt( index + terrainWidth + 1 );
    
    D3DXVec3Cross( &triangleNormal, &v0v1, &v0v2 );
    D3DXVec3Normalize( &triangleNormal, &triangleNormal );

    //calculate triangle angle
    D3DXVec3Cross( &angleVector, &triangleNormal, &D3DXVECTOR3( 0, 1, 0 ) );
    angle = acos( D3DXVec3Length( &angleVector ) );

    //height of the terrain under the player + offset
    float temp = closestPoint->y + ( triangleNormal.x * dx + triangleNormal.z * dz ) / -triangleNormal.y;
    //check if player hits
    if( position.y <= temp && state == jumping )
    {
        state = moving;
        fallSpeed = 0;
    }

    if( position.y - temp > 5 && state == moving )
    {
        state = jumping;
        fallSpeed = 0;
    }

    switch( state )
    {
        case jumping:
        {
            Physics::ApplyGravity( fallSpeed, dt );
            position.y += (float)( fallSpeed * dt );
            break;
        }
        case notMoving:
        {
            if(D3DXVec2Length( &vSpeed ) > 0)
                state = moving;
            break;
        }
        default:
        {
            if(D3DXVec2Length( &vSpeed ) == 0)
                state = notMoving;
            position.y += ( temp - position.y ) / 5;
            break;
        }
    }
}

void Player::glide()
{
    vSpeed.x += triangleNormal.x * 1 / triangleNormal.y * .2f;
    vSpeed.y += triangleNormal.z * 1 / triangleNormal.y * .2f;
}

Player::~Player()
{
    SAFE_DELETE( cUp );
    SAFE_DELETE( cDown );
    SAFE_DELETE( cLeft );
    SAFE_DELETE( cRight );
    SAFE_DELETE( cJump );
}
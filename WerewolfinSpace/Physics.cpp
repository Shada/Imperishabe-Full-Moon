#include "Physics.h"

void Physics::ApplyGravity( float &velocityY, double dt )
{
    velocityY -= (float)( gravCoeff * dt * 9 );
}
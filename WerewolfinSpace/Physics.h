#pragma once
#define gravCoeff 9.82f

class Physics
{
public:

    static void ApplyGravity(float &velocityY, double dt);
};
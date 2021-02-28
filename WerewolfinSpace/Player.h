#pragma once
#include "CollisionObject.h"
#include "CControl.h"
#include "Physics.h"
#include "Terrain.h"
#include "SoundHandler.h"
#include "DxHandler.h"
#include "InputHandler.h"

class Camera;
class Player :
	public CollisionObject
{
private:

	int modelIndex, nextModelIndex, index;

	InputHandler	*input;
	SoundHandler	*soundRun, *soundLand, *soundSpring;
	ID3DX10Font		*font;
	
	D3DXVECTOR2     vSpeed;
	D3DXVECTOR3		triangleNormal;
	D3DXVECTOR3		vDirection,
					vPrevDirection,
					angleVector,
					checkPointPos,
					prevPos;
	
	float			accelleration, 
					retardation, 
					speedLimit, 
					fallSpeed, 
					yPosReferred, 
					angle,
					morphFreq,
					boostDuration,
					boostFactor;
					
	RECT			r;
	double			timer;
	
	Camera*			camera;
	
	CControl		*cUp,
					*cDown,
					*cLeft,
					*cRight,
					*cJump;

	Terrain* terrain;

	enum PlayerState
	{
		notMoving,
		moving,
		jumping,
		gliding
	};
	PlayerState state;

	enum MorphState
	{
		mStanding,
		mRunning,
		mJumping,
		mFalling,
		mGoToStand
	};
	MorphState morphState;

	void initPlayer();

public:
	
	Player	( D3DXVECTOR3 position, InputHandler* inputH, Camera* camera, Terrain* terrain,
				ID3D10EffectTechnique* tech, vector<Model*> model, float scale, DxHandler *dxHandler );
	~Player	();

	void Update(double dt);
	void Draw(DxHandler* h);

	void death();
	void glide();
	void resetPlayer();
	void move(double dt);
	void morph(double dt);
	void followTerrain(double dt);
	void bounceOff(AABB *collidingBox);
	void boost(float magnitude, float duration);

	bool isJumping() { return state == jumping; }

	void setDefaultControls();
	void setToPrevYPos()				{ position.y--;										}
	void setPos(D3DXVECTOR3 pos)		{ position = pos;									} 
	void setCheckPoint(D3DXVECTOR3 pos) { checkPointPos = pos;								}
	void setNotMovingState(float y)		{ fallSpeed = 0; state = notMoving; position.y = y; }

	// an external object shoots the player up in the sky
	void setFallSpeed( float fallSpeed ){ this->fallSpeed = fallSpeed; position.y += 2; state = jumping; soundSpring->playSingle(); }

	float getPrevYPos() { return prevPos.y; }
};
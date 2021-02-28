#include "Springboard.h"
#include "Player.h"

Springboard::Springboard( D3DXVECTOR3 pos, vector<Model*> model, ID3D10EffectTechnique* tech, 
							float scale, float magnitude, D3DXVECTOR3 rotation )
	: CollisionObject( pos, model, tech, scale, rotation )
{
	this->magnitude = magnitude;
	SAFE_DELETE( collisionBox );

	collisionBox = new AABB( position, &min, &max, scale, rotation.y );
	viewdistance = SPRING_VIEWDISTANCE;
	morphState = mCoiled;
	timer = 0;
}

void Springboard::behaviour( Player *player )
{
	player->setFallSpeed( magnitude );
	morphState = mUncoiled;
}

void Springboard::Draw( DxHandler* h )
{
	h->sendWorldMatrix( mWorld );
	switch( morphState )
	{
		case mCoiled:
		{
			//this one does not animate, so it's not necessary to use Animation-tech
			model.at(0)->Draw( h, h->g_pTechRenderNoMoveLight );
			break;
		}
		case mUncoiled:
		{
			h->getID3D10Effect()->GetVariableByName( "time" )->AsScalar()->SetFloat( (float)timer );
			model.at(0)->Draw( h, tech, model.at(1) );
			break;
		}
		case mReturn:
		{
			h->getID3D10Effect()->GetVariableByName( "time" )->AsScalar()->SetFloat( (float)timer );
			model.at(1)->Draw( h, tech, model.at(0) );
			break;
		}
		case mStayUncoiled:
		{
			//this one does not animate, so it's not necessary to use Animation-tech
			model.at(1)->Draw( h, h->g_pTechRenderNoMoveLight );
			break;
		}
	}
}

void Springboard::Update( double dt, Object3D *player )
{
	
	CollisionObject::Update( dt, player );
	switch( morphState )
	{
		//this state is for when the spring throws up the player
		case mUncoiled:
		{
			
			timer += (float)( dt * magnitude / 6 );
			if( timer >= 1 )
			{
				timer = 0;
				//when the spring is completely uncoiled we want it 
				//to stay there for a little while
				morphState = mStayUncoiled;
			}
			break;
		}
		//This state is for when we want the spring to stay up
		case mStayUncoiled:
		{
			timer += dt;
			if( timer > 4 )
			{
				timer = 0;
				//After 4 seconds, we want it to retur
				morphState = mReturn;
			}
			break;
		}
		//This state is for when we want the spring to return to the coiled state.
		case mReturn:
		{
			timer += dt;
			if( timer >= 1 )
			{
				timer = 0;
				//When the spring have returned to it's original position, 
				//we want it to stay coiled
				morphState = mCoiled;
			}
			break;
		}
	}
}

Springboard::~Springboard()
{
}
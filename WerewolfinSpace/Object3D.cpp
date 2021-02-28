

#include "Object3D.h"
//modelID is not currently used. Might be used to write objects added in edit mode directly to the level file
Object3D::Object3D(int ID, int modelID,int lastModelID, D3DXVECTOR3 position, vector<Model*> model, ID3D10EffectTechnique* tech, float scale,D3DXVECTOR3 rotation)
{
	this->ID = ID;
	this->modelID = modelID;
	this->lastModelID = lastModelID;
	this->model = model;
	this->position = position;
	this->tech = tech;
	this->vRotation = rotation;

	numberOfVertices = model.at(0)->getVertexAmount();
	D3DXMatrixScaling(&mScale, scale, scale, scale);
	
	D3DXMatrixRotationY(&mRotation,vRotation.y);

	D3DXMatrixTranslation( &mTranslate, position.x, position.y, position.z );
	D3DXMatrixMultiply( &mWorld, &mScale, &mRotation );
	D3DXMatrixMultiply( &mWorld, &mWorld, &mTranslate );
	
	min = D3DXVECTOR3(0,0,0);
	max = D3DXVECTOR3(0,0,0);
	
	getHitBoxDim(&min, &max);
	viewdistance = DEFAULT_VIEWDISTANCE;
}
//modelID is not currently used. Might be used to write objects added in edit mode directly to the level file
//used by subclasses that dont use a model read from file, so this might not even need modelIDs
Object3D::Object3D(int ID, int modelID, int lastModelID, D3DXVECTOR3 position, ID3D10EffectTechnique* tech,D3DXVECTOR3 rotation)
{
	this->ID = ID;
	this->modelID = modelID;
	this->lastModelID = lastModelID;
	this->position = position;
	this->tech = tech;
	this->vRotation = rotation;

	model = vector<Model*>();

	numberOfVertices = 0;

	D3DXMatrixScaling(&mScale, 1, 1, 1);
	D3DXMatrixRotationYawPitchRoll( &mRotation, vRotation.x, vRotation.x, vRotation.z );
	D3DXMatrixTranslation( &mTranslate, position.x, position.y, position.z );
	D3DXMatrixMultiply( &mWorld, &mRotation, &mTranslate );
	
	min = D3DXVECTOR3(0, 0, 0);
	max = D3DXVECTOR3(0, 0, 0);
}

void Object3D::getHitBoxDim(D3DXVECTOR3 *min, D3DXVECTOR3 *max)
{
	for(unsigned int k = 0; k < model.front()->getGroups()->size(); k++)
	{
		for(int i = 0; i < model.front()->getGroups()->at(k).getVertexAmount(); i++)
		{
			D3DXVECTOR3 pos = model.front()->getGroups()->at(k).getVertexPosAt(i);
			if(pos.x < min->x)
				min->x = pos.x;
			else if(pos.x > max->x)
				max->x = pos.x;
			if(pos.y < min->y)
				min->y = pos.y;
			else if(pos.y > max->y)
				max->y = pos.y;
			if(pos.z < min->z)
				min->z = pos.z;
			else if(pos.z > max->z)
				max->z = pos.z;
		}
	}
}

void Object3D::Draw(DxHandler* h)
{
	//send this objects worldmatrix to the dxhandler
	h->sendWorldMatrix(mWorld);
	//draw the model of the object, if it has more than one model it will override
	//this method
	model.back()->Draw(h, tech);
}

Object3D::~Object3D()
{
	tech = NULL;
	for( unsigned int i = 0; i < model.size(); i++ )
		model.at(i) = NULL;
}

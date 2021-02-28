#pragma once
#include "stdafx.h"
#include "DxHandler.h"
#include "Model.h"
#include "AABB.h"

class Object3D
{
protected:
	D3DXVECTOR3 position,
				vRotation,
				min, 
				max;

	D3DXMATRIX	mRotation,
				mWorld,
				mScale,
				mTranslate;

	ID3D10EffectTechnique* tech;

	vector<Model*> model;
	int numberOfVertices, ID, modelID, lastModelID, viewdistance;

	void getHitBoxDim(D3DXVECTOR3 *min, D3DXVECTOR3 *max);
public:
	
	Object3D(int ID,int modelID,int lastModelID, D3DXVECTOR3 position, vector<Model*> model, ID3D10EffectTechnique* tech, float scale, D3DXVECTOR3 rotation);
	Object3D(int ID,int modelID,int lastModelID, D3DXVECTOR3 position, ID3D10EffectTechnique* tech,D3DXVECTOR3 rotation);
	virtual ~Object3D();

	virtual void Update(double dt, Object3D* player){;}
	virtual void Draw(DxHandler* h);

	int getID()				{ return ID;			}
	int getModelID()		{ return modelID;		}
	int getLastModelID()	{ return lastModelID;	}
	int getViewDistance()	{ return viewdistance;	}

	Model* getModel()		{ return model.at(0);	}
	D3DXVECTOR3 getPos()	{ return position;		}
	D3DXMATRIX	getWorld()	{ return mWorld;		}

	virtual D3DXVECTOR3 getMax() { return max + position; }
	virtual D3DXVECTOR3 getMin() { return min + position; }

	void setModel	( vector<Model*> model ) { this->model = model; } 
};
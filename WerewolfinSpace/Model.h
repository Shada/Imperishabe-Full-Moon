#pragma once
#include "stdafx.h"
#include "PolygonGroup.h"


class Model
{
	friend class ObjReader;
	friend class MtlReader;
private:

	vector<PolygonGroup> groups;
	string name;
	ID3D10Buffer* g_pVB;
	
	int vertexAmount;

	void createPData();
public:
	
	Model(string name);
	~Model();

	void Draw(DxHandler* h, ID3D10EffectTechnique* tech);
	void Draw(DxHandler* h, ID3D10EffectTechnique* tech,Model* nextKeyframe);

	void feedData();
	void createBuffer(ID3D10Device* g_pd3dDevice);

	string getName()					{  return name;			}
	int getVertexAmount()				{ return vertexAmount;	}
	ID3D10Buffer* getBuffer()			{return g_pVB;			}
	vector<PolygonGroup> *getGroups()	{ return &groups;		}

	void loadImageData(ID3D10Device* device);
	void loadAlphaMapData(ID3D10Device* device);
};
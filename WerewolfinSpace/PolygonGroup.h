#pragma once
#include "stdafx.h"
#include "Material.h"
#include "DxHandler.h"

class PolygonGroup
{
private:

	string name;
	int bufferIndex;
	vector<LineVertex> vertices;
public:

	Material* material;
	
	PolygonGroup(string name);
	~PolygonGroup();

	void Draw(DxHandler* h, ID3D10EffectTechnique* tech);

	void feedData(D3DXVECTOR3* vertex, D3DXVECTOR2* textureCoordinate, D3DXVECTOR3* normal);
	void feedToPData(LineVertex* pData);

	void setStartPos(int x) { bufferIndex = x; } 
	
	string getName()						{ return name;					}
	int getStartPos()						{ return bufferIndex;			}
	int getVertexAmount()					{ return vertices.size();		}
	D3DXVECTOR3 getVertexPosAt(int index)	{ return vertices[index].pos;	}
};
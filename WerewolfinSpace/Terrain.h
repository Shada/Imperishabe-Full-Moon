#pragma once
#include "object3d.h"
#include "HeightMap.h"
class Terrain :
	public Object3D
{
private:
	
	D3DXVECTOR2 dimensions;
	
	int subTerrainIndex;
	
	vector<D3DXVECTOR3*> *normals;
	vector<D3DXVECTOR2*> *texCoords;
	vector<D3DXVECTOR3*> *vertexPoints;
	
	vector<int> vertexIndex;
	vector<int> texCoordIndex;
	
	void createNormals();
	void createTexCoords();
	void createVertexPoints( string sHeightMap, float scale, float offset );
	void mapPositions();

public:
	
	Terrain( D3DXVECTOR3 position, D3DXVECTOR2 dimensions, string sHeightMap, ID3D10EffectTechnique* tech,
				DxHandler *h, string blendMap, string texture1, string texture2, string texture3, float scale, float offset );
	~Terrain();

	void Draw( DxHandler* h, int subTerrainIndex );
	
	int subTerrain( D3DXVECTOR2 pos, D3DXVECTOR2 area, ID3D10Device *device );
	
	D3DXVECTOR2 getDimensions() { return dimensions; }
	
	D3DXVECTOR3 *getVertexPointAt( int i )	{ return vertexPoints->at( i ); }
	D3DXVECTOR3 *getNormalAt( int i)		{ return normals->at( i );		}
	D3DXVECTOR2 *getTexCoordAt( int i )		{ return texCoords->at( i );	}
	
	vector<D3DXVECTOR3*>	*getVertexPoints()	{ return vertexPoints;	}
	vector<D3DXVECTOR3*>	*getNormals()		{ return normals;		}
	vector<D3DXVECTOR2*>	*getTexCoords()		{ return texCoords;		}	
};
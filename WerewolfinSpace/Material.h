#pragma once
#include "stdafx.h"
class Material
{
	friend class MtlReader;
private:

	string name;
	
	int illum;
	float ns, alpha;

	string textureFileName,	alphaMapFileName;
	ID3D10ShaderResourceView* g_pTextureResource, *g_pAlphaResource;
public:

	Material(string name);
	~Material();

	D3DXVECTOR3 ka, kd, ks;

	void loadImageData(ID3D10Device* g_pd3dDevice);
	void loadAlphaMap(ID3D10Device* g_pd3dDevice);
	void useShaderVariables(ID3D10Effect* g_pEffect);

	void setTexture(string textureFileName)		{ this->textureFileName = textureFileName; }
	void setAlphaMap(string alphaMapFileName)	{ this->alphaMapFileName = alphaMapFileName; }

	D3DXVECTOR3 getKa()			{ return ka;				}
	D3DXVECTOR3 getKd()			{ return kd;				}
	D3DXVECTOR3 getKs()			{ return ks;				}
	string getName()			{ return name;				}
	string getTextureFileName() { return textureFileName;	}
};
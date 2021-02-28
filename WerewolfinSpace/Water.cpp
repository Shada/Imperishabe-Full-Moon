#include "Water.h"


Water::Water(D3DXVECTOR3 position, D3DXVECTOR2 dimensions, string image, string alpha, DxHandler *dxHandler, float movement)
	: Object3D(10,-1,-1,position, dxHandler->g_pTechRenderWater,D3DXVECTOR3(0,0,0))
{
	this->image = image;
	this->alpha = alpha;

	this->dim = dimensions;
	this->movement = movement;
	t = 0;

	createWaterModel(dxHandler->getID3D10Device());
}

void Water::createWaterModel(ID3D10Device *device)
{
	model.push_back(new Model("Water"));
	Material *material = new Material("waterontop");
	PolygonGroup group = PolygonGroup("default");
	vector<PolygonGroup> *poly = model.back()->getGroups();

	//first triangle
	D3DXVECTOR3 *normal = new D3DXVECTOR3(0, 1, 0);
	D3DXVECTOR3 *pos = new D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR2 *texC = new D3DXVECTOR2(0, 0);

	group.feedData(pos, texC, normal);
	
	pos->z = dim.y;
	texC->y = 256;
	group.feedData(pos, texC, normal);

	pos->x = dim.x;
	texC->x = 256;
	group.feedData(pos, texC, normal);
	
	//second triangle
	pos->x = pos->z = 0;
	texC->x = texC->y = 0;
	group.feedData(pos, texC, normal);

	pos->x = dim.x;
	pos->z = dim.y;
	texC->x = texC->y = 256;
	group.feedData(pos, texC, normal);

	pos->z = 0;
	texC->y = 0;
	group.feedData(pos, texC, normal);
	
	SAFE_DELETE(normal);
	SAFE_DELETE(pos);
	SAFE_DELETE(texC);

	material->setTexture(image);
	material->setAlphaMap(alpha);

	material->loadImageData(device);
	material->loadAlphaMap(device);

	group.material = material;
	
	poly->push_back(group);
	model.back()->feedData();
	model.back()->createBuffer(device);
}

void Water::Update(double dt, Object3D *player)
{
	t += (float)( movement * dt );
}

void Water::Draw(DxHandler *h)
{
	//uses a simple technique that draws in two passes and adds to layers.   
	h->sendWorldMatrix(mWorld);
	h->getID3D10Effect()->GetVariableByName( "time" )->AsScalar()->SetFloat( t );
	model.at(0)->Draw(h, tech);
}

Water::~Water()
{
	for(unsigned int i = 0; i < model.size(); i++)
		SAFE_DELETE(model.at(i));
}
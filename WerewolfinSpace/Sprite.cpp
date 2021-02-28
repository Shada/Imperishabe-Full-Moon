#include "Sprite.h"


Sprite::Sprite(D3DXVECTOR2 anchor, D3DXVECTOR2 dimensions, ID3D10ShaderResourceView* background, 
				ID3D10ShaderResourceView* alpha, ID3D10Buffer* g_pVB, int bufferOffset, SpriteVertex* pData)
//shared buffer
{
	transparent = true;
	init(anchor, dimensions, background, alpha, g_pVB, bufferOffset, pData);
}
Sprite::Sprite(D3DXVECTOR2 anchor, D3DXVECTOR2 dimensions, ID3D10ShaderResourceView* background, 
				ID3D10Buffer* g_pVB, int bufferOffset, SpriteVertex* pData)
{
	transparent = false;
	init(anchor, dimensions, background, NULL, g_pVB, bufferOffset, pData);
}
Sprite::Sprite(D3DXVECTOR2 anchor, D3DXVECTOR2 dimensions, ID3D10ShaderResourceView* background, DxHandler* h)
{
	transparent = false;
	init2(anchor, dimensions, background, NULL, h);
}
Sprite::Sprite(D3DXVECTOR2 anchor, D3DXVECTOR2 dimensions, ID3D10ShaderResourceView* background, 
				ID3D10ShaderResourceView* alpha, DxHandler* h)
{
	transparent = true;
	init2(anchor, dimensions, background, alpha, h);
}
void Sprite::init(D3DXVECTOR2 anchor, D3DXVECTOR2 dimensions, ID3D10ShaderResourceView* background, 
					ID3D10ShaderResourceView* alpha, ID3D10Buffer* g_pVB, int bufferOffset, SpriteVertex* pData)
{
	this->background = background;
	this->alpha = alpha;
	this->bufferOffset = bufferOffset;
	this->g_pVB = g_pVB;

	topLeft = D3DXVECTOR3(anchor.x, anchor.y, 0.f);
	bottomRight = D3DXVECTOR3(anchor.x + dimensions.x, anchor.y + dimensions.y, 0.f);
	D3DXMatrixIdentity(&mWorld);
	
	pData[bufferOffset].anchor = D3DXVECTOR3( pixToClip(SCREENWIDTH, (int)anchor.x ), 
						-pixToClip(_SCREENHEIGHT, (int)anchor.y), 0.f);// D3DXVECTOR2(-1,1);
	pData[bufferOffset].dimensions = D3DXVECTOR2( pixToClipDist(SCREENWIDTH, (int)dimensions.x),
						pixToClipDist(_SCREENHEIGHT, (int)dimensions.y));//D3DXVECTOR2(2,2);

	hovered = false;
}
//has its own buffer
void Sprite::init2(D3DXVECTOR2 anchor, D3DXVECTOR2 dimensions, ID3D10ShaderResourceView* background, 
					ID3D10ShaderResourceView* alpha, DxHandler* h)
{
	this->background = background;
	this->alpha = alpha;
	topLeft = D3DXVECTOR3(anchor.x, anchor.y, 0);
	bottomRight = D3DXVECTOR3(anchor.x + 1.f, anchor.y + 1, 0);
	

	bufferOffset = 0;
	D3DXMatrixIdentity(&mWorld);

	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof( SpriteVertex ); //*2
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	if( FAILED( h->getID3D10Device()->CreateBuffer( &bd, 0, &g_pVB ) ) )
		MessageBox( 0, "Unable to create Vertex Buffer", "VB Error", 0 );
	
	SpriteVertex* pData;
	g_pVB->Map( D3D10_MAP_WRITE_DISCARD, 0, reinterpret_cast< void** >((void**)&pData) );

	pData[bufferOffset].anchor = D3DXVECTOR3( pixToClip(SCREENWIDTH, (int)anchor.x ), 
					-pixToClip(_SCREENHEIGHT, (int)anchor.y), 0.f );
	pData[bufferOffset].dimensions = D3DXVECTOR2( pixToClipDist(SCREENWIDTH, (int)dimensions.x), 
					pixToClipDist(_SCREENHEIGHT, (int)dimensions.y ));

	g_pVB->Unmap();

	hovered = false;
}
void Sprite::Draw(DxHandler* h)
{
	h->getID3D10Effect()->GetVariableByName("pic")->AsShaderResource()->SetResource(background);
	if(transparent)
		h->getID3D10Effect()->GetVariableByName("alphaMap")->AsShaderResource()->SetResource(alpha);
	h->getID3D10Effect()->GetVariableByName( "mWorld" )->AsMatrix()->SetMatrix( (float*)&mWorld );

	h->getID3D10Effect()->GetVariableByName("hovered")->AsScalar()->SetBool(hovered);
	
	bottomRight = bottomRight;

	UINT stride = sizeof(SpriteVertex);
	UINT offset = 0;
	h->getID3D10Device()->IASetInputLayout( h->g_pSpriteLayout );
	h->getID3D10Device()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	h->getID3D10Device()->IASetVertexBuffers( 0, 1, &g_pVB, &stride, &offset );

	if(transparent)
		renderPasses(h, h->g_pTechRenderAlphaSprite);
	else
		renderPasses(h, h->g_pTechRenderSprite);
}
void Sprite::renderPasses(DxHandler* h, ID3D10EffectTechnique* technique)
{
	D3D10_TECHNIQUE_DESC techDesc;
	technique->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; p++ )
	{
		technique->GetPassByIndex( p )->Apply(0);
		h->getID3D10Device()->Draw(1, bufferOffset);
	}
}
void Sprite::move(float x , float y)
{
	topLeft += D3DXVECTOR3(x, y, 0.f);
	bottomRight += D3DXVECTOR3(x, y, 0.f);
	D3DXMatrixTranslation(&mWorld, 1.f + pixToClip(SCREENWIDTH, (int)topLeft.x), 
					pixToClip(_SCREENHEIGHT, (int)-topLeft.y) + 1.f, 0);
	
}
void Sprite::setPos(float x, float y)
{
	topLeft = D3DXVECTOR3(x, y, 0.f);
	bottomRight = D3DXVECTOR3(x + 1.f, y + 1.f, 0.f);
	D3DXMatrixTranslation(&mWorld, 1.f + pixToClip(SCREENWIDTH, (int)topLeft.x), 
					pixToClip(_SCREENHEIGHT, (int)-topLeft.y) + 1.f, 0);
}

bool Sprite::intersects(Sprite a)
{
	bool xIntersects =	valueInRange((int)topLeft.x, (int)a.getTopLeft().x,	(int)a.getBottomRight().x) ||
						valueInRange((int)a.getTopLeft().x,	(int)topLeft.x,	(int)bottomRight.x);

	bool yIntersects =	valueInRange((int)topLeft.y, (int)a.getTopLeft().y,	(int)a.getBottomRight().y) ||
						valueInRange((int)a.getTopLeft().y,	(int)topLeft.y,	(int)bottomRight.y);

	return xIntersects && yIntersects;
}
Sprite::~Sprite()
{
	background = NULL;
	alpha = NULL;
	SAFE_RELEASE(background);
	SAFE_RELEASE(alpha);

	g_pVB = NULL;
}
#include "Model.h"



Model::Model(string name)
{
	//The models name is saved, so that it can be used for searching
	//it's not used, but it might be in the future
	this->name = name;
	//The vector with groups
	groups = vector<PolygonGroup>();

	g_pVB = NULL;
}

void Model::Draw(DxHandler* h, ID3D10EffectTechnique* tech)
{
	//The stride and offset
	UINT stride = sizeof(LineVertex), offset = 0;

	//Set the layouts and topology
	h->getID3D10Device()->IASetInputLayout( h->g_pVertexLayout );
	h->getID3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	h->getID3D10Device()->IASetVertexBuffers( 0, 1, &g_pVB, &stride, &offset );

	for( unsigned int i = 0; i < groups.size(); i++ )
		groups.at(i).Draw(h, tech);
}

//This draw call is only used for the models which will be morphed
void Model::Draw(DxHandler* h, ID3D10EffectTechnique* tech, Model* nextKeyframe)
{
	// Set Input Assembler params
	UINT stride[2];
	
	stride[0] = sizeof(LineVertex);
	stride[1] = sizeof(LineVertex);
	UINT offset[2] = { 0, 0 };
	
	//Create the array of buffers with this model and the next fram model.
	ID3D10Buffer* buffers[2];
	buffers[0] = this->g_pVB;
	buffers[1] = nextKeyframe->getBuffer();

	//set the layouts and topology
	h->getID3D10Device()->IASetInputLayout( h->g_pAnimationLayout );
	h->getID3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	h->getID3D10Device()->IASetVertexBuffers( 0, 2, buffers, stride, offset );

	// Check if morphing is possible, this draw could be called by misstake
	if( this->getVertexAmount() == nextKeyframe->getVertexAmount()
		&& this->getGroups()->size() == nextKeyframe->getGroups()->size() )
 		for( unsigned int i = 0; i < groups.size(); i++ )
			groups.at(i).Draw( h, tech );
	else
		// Just render it normally if morphing is not possible
		Draw( h, tech );
}
void Model::feedData()
{ 
	// Set the correct startpositions within the buffer and count the total amount 
	//of vertex points
	vertexAmount = 0;
	unsigned int i;
	for( i = 1; i < groups.size(); i++)
	{
		vertexAmount += groups.at(i - 1).getVertexAmount();
		groups.at(i).setStartPos(vertexAmount);
	}
	vertexAmount += groups.back().getVertexAmount();
}

void Model::createBuffer(ID3D10Device* g_pd3dDevice)
{
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof( LineVertex ) * getVertexAmount();
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	if( FAILED( g_pd3dDevice->CreateBuffer( &bd, 0, &g_pVB ) ) )
		MessageBox( 0, "Unable to create Vertex Buffer", "VB Error", 0 );

	createPData();
}

void Model::createPData()
{
	LineVertex* pData = NULL;
	//model has its own VB
	g_pVB->Map( D3D10_MAP_WRITE_DISCARD, 0, reinterpret_cast< void** >((void**)&pData) );
	for(unsigned int i=0;i < groups.size();i++)
		groups.at(i).feedToPData(pData);

	g_pVB->Unmap();
}

void Model::loadImageData(ID3D10Device* device)
{
	for(unsigned int i = 0; i < groups.size(); i++)
		groups.at(i).material->loadImageData(device);
}
void Model::loadAlphaMapData(ID3D10Device* device)
{
	for(unsigned int i = 0; i < groups.size(); i++)
		groups.at(i).material->loadAlphaMap(device);
}

Model::~Model()
{
	SAFE_RELEASE(g_pVB);
}

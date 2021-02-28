#include "Skybox.h"
//the -2 values are for modelIDs and such, to be able to write new object resently added in the edit mode directly to
//the level file, this is not implemented/used yet. 
Skybox::Skybox(ID3D10EffectTechnique* tech, ID3D10Device* g_pd3dDevice, D3DXVECTOR3* cameraPos, 
            vector<Model*> model, vector<string> *images)
    :Object3D(-2,-2,-2,*cameraPos, tech,D3DXVECTOR3(0,0,0))
{
    this->tech = tech;
    this->g_pd3dDevice = g_pd3dDevice;
    this->cameraPos = cameraPos;
    this->model = model;
    
    model.back()->createBuffer(g_pd3dDevice);

    numberOfVertices = model.back()->getVertexAmount();

    D3DXMatrixScaling(&mScale, 100, 100, 100);

    textures = new vector<ID3D10ShaderResourceView*>(6);
    
    D3DX10CreateShaderResourceViewFromFile(g_pd3dDevice, images->at(0).c_str(), NULL, NULL, &textures->at(1), NULL); //front
    D3DX10CreateShaderResourceViewFromFile(g_pd3dDevice, images->at(1).c_str(), NULL, NULL, &textures->at(3), NULL); //back
    D3DX10CreateShaderResourceViewFromFile(g_pd3dDevice, images->at(2).c_str(), NULL, NULL, &textures->at(2), NULL); //right
    D3DX10CreateShaderResourceViewFromFile(g_pd3dDevice, images->at(3).c_str(), NULL, NULL, &textures->at(4), NULL); //left
    D3DX10CreateShaderResourceViewFromFile(g_pd3dDevice, images->at(4).c_str(), NULL, NULL, &textures->at(5), NULL); //top
    D3DX10CreateShaderResourceViewFromFile(g_pd3dDevice, images->at(5).c_str(), NULL, NULL, &textures->at(0), NULL); //bottom
}

void Skybox::Draw(DxHandler* h)
{
    UINT stride = sizeof(LineVertex), offset = 0;

    ID3D10Buffer *g_pVB = model.back()->getBuffer();

    h->getID3D10Device()->IASetInputLayout( h->g_pVertexLayout );
    h->getID3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    h->getID3D10Device()->IASetVertexBuffers( 0, 1, &g_pVB, &stride, &offset );

    h->sendWorldMatrix(mWorld);
    ID3D10Effect* effect = h->getID3D10Effect();
    for(int i = 0; i < 6; i++)
    {
        effect->GetVariableByName("skybox")->AsShaderResource()->SetResource(textures->at(i));
        h->Render( tech, i * 6, 6 );
    }
}
void Skybox::Update(double dt, Object3D *player)
{
    D3DXMatrixTranslation(&mTranslate, cameraPos->x, cameraPos->y + 2.f, cameraPos->z);

    D3DXMatrixMultiply(&mWorld, &mScale, &mTranslate);
}
Skybox::~Skybox()
{
    for(unsigned int i = 0; i < textures->size(); i++)
        SAFE_RELEASE(textures->at(i));
    textures->clear();

    for(unsigned int i = 0; i < model.size(); i++)
        SAFE_DELETE(model.at(i));
    SAFE_DELETE(textures);
}
#pragma once
#include "stdafx.h"
#include "Camera.h"

class DxHandler
{
private:
	
	D3D10_DRIVER_TYPE        g_driverType;

	ID3D10Device			*g_pd3dDevice;
	IDXGISwapChain			*g_pSwapChain;
	ID3D10RenderTargetView	*g_pRenderTargetView;
	ID3D10Texture2D			*g_pDepthStencil;
	ID3D10DepthStencilView	*g_pDepthStencilView;
	HWND					hWnd;


	Camera					*camera;
	
	D3DXMATRIX				mView,
							mProj,
							mWVP,
							mWV;

	float t;

	ID3D10Effect			*g_pEffect;

	HRESULT InitDevice();
public:
	
	// All techniques for rendering, all liked to different pixel and vertex shaders
	ID3D10EffectTechnique	*g_pTechRenderNoMoveLight;
	ID3D10EffectTechnique	*g_pTechRenderLine;
	ID3D10EffectTechnique	*g_pTechRenderSkyBox;
	ID3D10EffectTechnique	*g_pTechRenderBillboard;
	ID3D10EffectTechnique	*g_pTechRenderSprite;
	ID3D10EffectTechnique	*g_pTechRenderAlphaSprite;
	ID3D10EffectTechnique	*g_pTechRenderWater;
	ID3D10EffectTechnique	*g_pTechRenderAnimated;
	ID3D10EffectTechnique	*g_pTechRenderRedLine;
	ID3D10EffectTechnique	*g_pTechRenderTree;

	ID3D10InputLayout		*g_pSpriteLayout;
	ID3D10InputLayout		*g_pVertexLayout;
	ID3D10InputLayout		*g_pAnimationLayout;
	
	
	DxHandler( HWND g_hWnd, Camera* camera );
	~DxHandler();

	HRESULT Render( ID3D10EffectTechnique* tech, int bufferIndex, int numberOfVertices );

	void Update();
	void sendWorldMatrix( D3DXMATRIX mWorld );

	HRESULT setResolution();

	ID3D10Device* getID3D10Device() { return g_pd3dDevice; }
	ID3D10Effect* getID3D10Effect() { return g_pEffect;	}

	void clearRenderTarget();
	void swapChain();

	HWND *getHWnd() { return &hWnd; }
};
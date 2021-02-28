#include "DxHandler.h"


DxHandler::DxHandler(HWND g_hWnd, Camera* camera)
{
	g_driverType			= D3D10_DRIVER_TYPE_NULL;
	g_pd3dDevice			= NULL;
	g_pSwapChain			= NULL;
	g_pRenderTargetView		= NULL;
	g_pDepthStencil			= NULL;
	g_pDepthStencilView		= NULL;
	
	this->camera = camera;

	hWnd = g_hWnd;

	t = 0;
	InitDevice();
}

static float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };


//Initiate the device
HRESULT DxHandler::InitDevice()
{
	HRESULT hr = S_OK;;

	RECT rc;
	GetClientRect( hWnd, &rc );
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

	D3D10_DRIVER_TYPE driverTypes[] = 
	{
		D3D10_DRIVER_TYPE_HARDWARE,
		D3D10_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof(sd) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D10CreateDeviceAndSwapChain( NULL, g_driverType, NULL, createDeviceFlags, 
			D3D10_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice );
		if( SUCCEEDED( hr ) )
			break;
	}
	if( FAILED(hr) )
		return hr;

	// Create a render target view
	ID3D10Texture2D* pBackBuffer;
	hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), (LPVOID*)&pBackBuffer );
	if( FAILED(hr) )
		return hr;

	hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRenderTargetView );
	pBackBuffer->Release();
	if( FAILED(hr) )
		return hr;

	// Create depth stencil texture
	D3D10_TEXTURE2D_DESC descDepth;
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D10_USAGE_DEFAULT;
	descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = g_pd3dDevice->CreateTexture2D( &descDepth, NULL, &g_pDepthStencil );
	if( FAILED(hr) )
		return hr;

	// Create the depth stencil view
	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
	if( FAILED(hr) )
		return hr;


	g_pd3dDevice->OMSetRenderTargets( 1, &g_pRenderTargetView, g_pDepthStencilView );
	// Setup the viewport
	D3D10_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pd3dDevice->RSSetViewports( 1, &vp );
	//Init shader effect
	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;
	if(FAILED(D3DX10CreateEffectFromFile(	"shader.fx",
											NULL,
											NULL,
											"fx_4_0",
											dwShaderFlags,
											0,
											g_pd3dDevice,
											NULL,
											NULL,
											&g_pEffect,
											NULL,
											NULL)))
	{
		MessageBox(0, "Error compiling shader!", "Shader error!", 0);
		return E_FAIL;
	}

	//get the techniques from the shader
	g_pTechRenderLine = g_pEffect->GetTechniqueByName("DrawObject");
	g_pTechRenderNoMoveLight = g_pEffect->GetTechniqueByName("DrawNoMoveLight");
	g_pTechRenderSkyBox = g_pEffect->GetTechniqueByName("DrawSkyBox");
	g_pTechRenderSprite = g_pEffect->GetTechniqueByName("DrawSprite");
	g_pTechRenderAlphaSprite = g_pEffect->GetTechniqueByName("DrawTransparentSprite");
	g_pTechRenderWater = g_pEffect->GetTechniqueByName("DrawWater");
	g_pTechRenderAnimated = g_pEffect->GetTechniqueByName("DrawTweening");
	g_pTechRenderRedLine = g_pEffect->GetTechniqueByName("DrawLine");
	g_pTechRenderTree = g_pEffect->GetTechniqueByName("DrawTree");

	// Define our vertex data layout
	const D3D10_INPUT_ELEMENT_DESC lineVertexLayout[] =
	{
		{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORM", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 3, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 6, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};
	const D3D10_INPUT_ELEMENT_DESC spriteVertexLayout[] =
	{
		{ "ANCHOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "DIMENSIONS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 3, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};
	const D3D10_INPUT_ELEMENT_DESC animationLayout[] =
	{
		{ "FPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "FNORM", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 3, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "FTEXC", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 6, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "LPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "LNORM", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, sizeof(float) * 3, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};
	D3D10_PASS_DESC PassDesc;
	g_pTechRenderLine->GetPassByIndex(0)->GetDesc(&PassDesc);

	//Create Input Layout (== Vertex Declaration)
	g_pd3dDevice->CreateInputLayout(lineVertexLayout,
		sizeof(lineVertexLayout) / sizeof(D3D10_INPUT_ELEMENT_DESC),
		PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize,
		&g_pVertexLayout );

	g_pTechRenderSprite->GetPassByIndex(0)->GetDesc(&PassDesc);
	if(FAILED(g_pd3dDevice->CreateInputLayout(spriteVertexLayout,
		sizeof(spriteVertexLayout) / sizeof(D3D10_INPUT_ELEMENT_DESC),
		PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize,
		&g_pSpriteLayout )))
		MessageBox(0,"failed to create spriteVertexLayout", "fail", 0);

	g_pTechRenderAnimated->GetPassByIndex(0)->GetDesc(&PassDesc);
	if(FAILED(g_pd3dDevice->CreateInputLayout(animationLayout,
		sizeof(animationLayout) / sizeof(D3D10_INPUT_ELEMENT_DESC), 
		PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize,
		&g_pAnimationLayout )))
		MessageBox(0,"failed to create animLayout", "fail", 0);

	

	//Set the light variables and send to shader
	D3DXVECTOR3 Ld(.8f,.8f,.8f);
	D3DXVECTOR3 Ls(.5f,.5f,.5f);
	D3DXVECTOR4 lightPos(3000, 100, 2500, 1.0f);
	g_pEffect->GetVariableByName( "Ld" )->AsVector()->SetFloatVector( (float*)&Ld );
	g_pEffect->GetVariableByName( "Ls" )->AsVector()->SetFloatVector( (float*)&Ls );
	g_pEffect->GetVariableByName( "LightPos" )->AsVector()->SetFloatVector( (float*)&lightPos );


	return S_OK;
}

//We tried to make a function for changing the resolution. We didn't get it to work though...
// Code is deleted..
HRESULT DxHandler::setResolution()
{
	InitDevice();
	return S_OK;
}

void DxHandler::Update()
{
	//Get the view and projection matrices from the camera
	mView = camera->getViewMatrix();
	mProj = camera->getProjMatrix();

	// Send in variables to the shader
	g_pEffect->GetVariableByName( "CameraPos" )->AsVector()->SetFloatVector((float*)&D3DXVECTOR4(*camera->getPos(), 1) );
	g_pEffect->GetVariableByName( "mView" )->AsMatrix()->SetMatrix( (float*)&mView );
}

void DxHandler::sendWorldMatrix(D3DXMATRIX mWorld)
{
	//Calculate the worldview and the worldviewprojection matrices
	mWV = mWorld * mView;
	mWVP = mWV * mProj;

	//send int the matrices to the shader
	g_pEffect->GetVariableByName( "mWorldView" )->AsMatrix()->SetMatrix( (float*)&mWV );
	g_pEffect->GetVariableByName( "mWorldViewProjection" )->AsMatrix()->SetMatrix( (float*)&mWVP );
}
HRESULT DxHandler::Render( ID3D10EffectTechnique* tech, int bufferIndex, int numberOfVertices )
{
	// Render objects using specified technique
	D3D10_TECHNIQUE_DESC techDesc;
	tech->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; p++ )
	{
		tech->GetPassByIndex( p )->Apply(0);
		g_pd3dDevice->Draw(numberOfVertices, bufferIndex);
	}

	return S_OK;
}

void DxHandler::clearRenderTarget()
{
	//clear render target
	g_pd3dDevice->ClearRenderTargetView( g_pRenderTargetView, ClearColor );

	//clear depth info
	g_pd3dDevice->ClearDepthStencilView( g_pDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0 );
}
void DxHandler::swapChain()
{
	//swap the chain
	g_pSwapChain->Present( 0, 0 );
}
DxHandler::~DxHandler()
{
	//Release the layouts
	SAFE_RELEASE( g_pVertexLayout );
	SAFE_RELEASE( g_pAnimationLayout );
	SAFE_RELEASE( g_pSpriteLayout );

	//Release the effect variables
	SAFE_RELEASE( g_pEffect );
	SAFE_RELEASE( g_pd3dDevice );
	SAFE_RELEASE( g_pSwapChain );
	SAFE_RELEASE( g_pRenderTargetView );
	SAFE_RELEASE( g_pDepthStencil );
	SAFE_RELEASE( g_pDepthStencilView );

	// set the techniques to null
	g_pTechRenderNoMoveLight	= NULL;
	g_pTechRenderLine			= NULL;
	g_pTechRenderSkyBox			= NULL;
	g_pTechRenderBillboard		= NULL;
	g_pTechRenderSprite			= NULL;
	g_pTechRenderAlphaSprite	= NULL;
	g_pTechRenderWater			= NULL;
	g_pTechRenderAnimated		= NULL;
	g_pTechRenderRedLine		= NULL;
	g_pTechRenderTree			= NULL;
}

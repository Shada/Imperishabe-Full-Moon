Texture2D skybox;

Texture2D blendTex;
Texture2D terrain1;
Texture2D terrain2;
Texture2D terrain3;

Texture2D pic;
Texture2D alphaMap;

// **** **** ****  **** **** **** //
// **** Vertex Shader Inputs **** //
// **** **** ****  **** **** **** //
struct VSSceneIn
{
	float3 Pos		: POS;
	float3 Norm		: NORM;
	float2 TexCoord	: TEXCOORD;
};

struct VSAnimIn
{
	float3 Pos0			: FPOS;
	float3 Norm0		: FNORM;
	float2 TexCoord0	: FTEXC;
	float3 Pos1			: LPOS;
	float3 Norm1		: LNORM;
};


// **** **** **** **** **** **** //
// **** Pixel Shader Inputs **** //
// **** **** **** **** **** **** //
struct PSSceneIn
{
	float4 Pos		: SV_Position;
	float4 Norm		: NORM;
	float2 TexCoord : TEXCOORD;
	float2 blendTexC	: BLENDTEXC;
	float3 LightInt : LIGHT;
};

struct PSRecColorIn
{
	float4 Pos		: SV_POSITION;
};

struct PSNoBlendIn
{
	float4 Pos		: SV_Position;
	float4 Norm		: NORM;
	float2 TexCoord : TEXCOORD;
	float3 LightInt : LIGHT;
};

struct PSSkyBoxIn
{
	float4 Pos : SV_POSITION;
	float4 Norm		: NORM;
	float2 TexCoord : TEXCOORD;
};

struct PS_INPUT2
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

struct SpriteInput
{
	float3 topLeft : ANCHOR;
	float2 dimensions : DIMENSIONS;
};

// **** **** **** **** **** //
// **** Sampler States **** //
// **** **** **** **** **** //
SamplerState linearSampler {
	Filter = ANISOTROPIC;
	AddressU = Wrap;
	AddressV = Wrap;
};
SamplerState bilinearSampler
{
    Filter = min_mag_mip_linear;
    AddressU = MIRROR;
    AddressV = MIRROR;	
};
SamplerState skyBoxSampler {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

// **** **** **** **** **** //
// ***** Blend States ***** //
// **** **** **** **** **** //

BlendState alphaBlend
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
	BlendOp = ADD;
	SrcBlendAlpha = ZERO;
	DestBlendAlpha = ZERO;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0F;
};
// **** **** **** **** //
// **** C-Buffers **** //
// **** **** **** **** //

cbuffer cbOnce
{
	float3 Ld;
	float3 Ls;
	float4 LightPos;
}
cbuffer cbEveryFrame
{
	float3 Ka;
	float3 Kd;
	float3 Ks;
	float4 CameraPos;
	float time;
	float fadefactor;

	matrix mWorld;
	matrix mWorldViewProjection;	
	matrix mWorldView;
	matrix mView;
};
cbuffer cbEverySprite
{
	bool hovered;
};
// **** **** ****  **** **** **** //
// **** Depth Stencil States **** //
// **** **** ****  **** **** **** //

DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

DepthStencilState EnableDepthTestOnly
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
};

DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
};

// **** **** ****  **** **** //
// *** Resterizer States *** //
// **** **** ****  **** **** //
RasterizerState NoCulling
{
	CullMode = NONE;
};
RasterizerState backFaceCulling
{
	cullmode = back;
};
RasterizerState frontFaceCulling
{
	cullmode = front;
};


// **** **** **** **** **** //
// **** Vertex Shaders **** //
// **** **** **** **** **** //
PSSceneIn VSScene(VSSceneIn input)
{
	PSSceneIn output = (PSSceneIn)0;
	output.Pos = mul( float4(input.Pos, 1.0), mWorldViewProjection );
	float3 tnorm = normalize(mul(input.Norm, mWorldView));
	float4 eyeCoords = mul(float4(input.Pos, 1.0), mWorldView);
	float3 lightPos = mul(LightPos, mView);
	float3 cameraPos = mul(CameraPos, mView);
	float3 s = normalize(lightPos - eyeCoords);

	float3 lightIntens = Ld * Kd * max(dot(s, tnorm), 0.0) + 
						Ka + 
						Ls * Ks * pow(max(dot(normalize(-s + 2 * dot(s, tnorm) * tnorm), 
										normalize(cameraPos - eyeCoords)), 0.0), 20);

	output.LightInt = lightIntens;
	
	float2 tile;
	tile.x = (input.Pos.x + 4 * 10) / (4 * 10);
	tile.y = (input.Pos.z - 4 * 10 ) / (-4 * 10);
	output.TexCoord = tile;//input.TexCoord;

	float2 blendTexC;
	blendTexC.x = (input.Pos.x + 256 * 10) / (256 * 10);
	blendTexC.y = (input.Pos.z - 256 * 10 ) / (-256 * 10);
	output.blendTexC = blendTexC; //256*5 should be send in as a variable

	return output;
}

PSNoBlendIn VSNoMoveLight(VSSceneIn input)
{
	PSNoBlendIn output = (PSNoBlendIn)0;
	output.Pos = mul( float4(input.Pos, 1.0), mWorldViewProjection );
	float3 lightPos = mul(LightPos, mView);
	float3 tnorm = normalize(mul(input.Norm, mWorldView));
	float4 eyeCoords = mul(float4(input.Pos, 1.0), mWorldView);
	float3 cameraPos = mul(CameraPos, mView);
	float3 s = normalize(lightPos - eyeCoords);

	float3 lightIntens = Ld * Kd * max(dot(s, tnorm), 0.0) + 
						Ka + 
						Ls * Ks * pow(max(dot(normalize(-s + 2 * dot(s, tnorm) * tnorm), 
										normalize(cameraPos - eyeCoords)), 0.0), 20);

	
	output.LightInt = lightIntens; 
	
	
	output.TexCoord = input.TexCoord;

	return output;
}

PSNoBlendIn VSTweening(VSAnimIn input)
{
	PSNoBlendIn output = (PSNoBlendIn)0;

	float3 tweenedPos = lerp(input.Pos0,input.Pos1,time);
	float3 tweenedNorm = lerp(input.Norm0,input.Norm1,time);

	output.Pos = mul( float4(tweenedPos, 1.0), mWorldViewProjection );
	float3 lightPos = mul(LightPos, mView);
	float3 tnorm = normalize(mul(tweenedNorm, mWorldView));
	float4 eyeCoords = mul(float4(tweenedPos, 1.0), mWorldView);
	float3 cameraPos = mul(CameraPos, mView);
	float3 s = normalize(lightPos - eyeCoords);

	float3 lightIntens = Ld * Kd * max(dot(s, tnorm), 0.0) + 
						Ka + 
						Ls * Ks * pow(max(dot(normalize(-s + 2 * dot(s, tnorm) * tnorm), 
										normalize(cameraPos - eyeCoords)), 0.0), 20);

	
	output.LightInt = lightIntens; 
	
	
	output.TexCoord = input.TexCoord0;

	return output;
}

PSSkyBoxIn VSSkyBox(VSSceneIn input)
{
	PSSkyBoxIn output = (PSSkyBoxIn)0;
	output.TexCoord = input.TexCoord;

	output.Pos = mul(float4(input.Pos,1.0), mWorldViewProjection );

	return output;
}

SpriteInput VSSprite( SpriteInput input ) //: SV_POSITION
{
	SpriteInput output;
	output.topLeft = (float3)(mul(float4(input.topLeft,1.0), mWorld));
	output.dimensions = input.dimensions;
	return output;
}

PSNoBlendIn VSWater(VSSceneIn input)
{
	PSNoBlendIn output = (PSNoBlendIn)0;
	output.Pos = mul( float4(input.Pos, 1.0), mWorldViewProjection );
	float3 tnorm = normalize(mul(input.Norm, mWorldView));

	float4 eyeCoords = mul(float4(input.Pos, 1.0), mWorldView);
	float3 lightPos = mul(LightPos, mView);
	float3 cameraPos = mul(CameraPos, mView);
	float3 s = normalize(lightPos - eyeCoords);

	float3 lightIntens = Ld * Kd * max(dot(s, tnorm), 0.0) + Ka - float3(0.1, 0.1, 0.1);

	output.LightInt = lightIntens;

	output.TexCoord = input.TexCoord * 0.8 + .04 * sin(time);
	return output;
}

PSNoBlendIn VSWaterPass1(VSSceneIn input)
{
	PSNoBlendIn output = (PSNoBlendIn)0;
	output.Pos = mul( float4(input.Pos, 1.0), mWorldViewProjection );
	output.Pos.y = output.Pos.y + 0.15;
	float3 tnorm = normalize(mul(input.Norm, mWorldView));

	float4 eyeCoords = mul(float4(input.Pos, 1.0), mWorldView);
	float3 lightPos = mul(LightPos, mView);
	float3 cameraPos = mul(CameraPos, mView);
	float3 s = normalize(lightPos - eyeCoords);

	float3 lightIntens = Ld * Kd * max(dot(s, tnorm), 0.0) + Ka;

	output.LightInt = lightIntens;

	output.TexCoord = input.TexCoord + .04 * cos(time);
	return output;
}

PSRecColorIn VSRedColor(VSSceneIn input)
{
	PSRecColorIn output = (PSRecColorIn)0;
	output.Pos = mul( float4(input.Pos, 1.0), mWorldViewProjection );
	return output;
}

PS_INPUT2 VSTree(VSSceneIn input)
{
	PS_INPUT2 output = (PS_INPUT2)0;

	output.pos = mul(float4(input.Pos, 1.0f), mWorldViewProjection);
	output.uv = input.TexCoord;
	return output;
}

// **** **** **** **** **** //
// ***** PixelShaders ***** //
// **** **** **** **** **** //

float4 PSSkyBox(PSSkyBoxIn input) : SV_Target
{
	return skybox.Sample(skyBoxSampler, input.TexCoord);
}

float4 PSScene(PSSceneIn input) : SV_Target
{	
	float3 blendTexColor = blendTex.Sample(linearSampler,input.blendTexC);
	
	float inverseTotal = 1.0f / (blendTexColor.x + blendTexColor.y + blendTexColor.z);

	float4 terrainCol1 = terrain1.Sample(linearSampler, input.TexCoord);
	float4 terrainCol2 = terrain2.Sample(linearSampler, input.TexCoord);
	float4 terrainCol3 = terrain3.Sample(linearSampler, input.TexCoord);

	terrainCol1 *= blendTexColor.x * inverseTotal;
	terrainCol2 *= blendTexColor.y * inverseTotal;
	terrainCol3 *= blendTexColor.z * inverseTotal;

	float4 finalCol = (terrainCol1 + terrainCol2 + terrainCol3) * float4(input.LightInt,1.0);

	return finalCol;
}

float4 PSNoBlend(PSNoBlendIn input) : SV_Target
{	
	float3 texColor = pic.Sample(linearSampler, input.TexCoord);
	return  float4(texColor * input.LightInt, fadefactor);
}



float4 PSSprite( PS_INPUT2 input ) : SV_TARGET
{		
	float3 col = pic.Sample(bilinearSampler, input.uv);
	if(hovered)
		col *= 1.5;
	return float4(col,1.0f);
}

float4 PSAlphaSprite( PS_INPUT2 input ) : SV_TARGET
{		
	float3 col = pic.Sample(bilinearSampler, input.uv);
	float alpha = alphaMap.Sample(linearSampler, input.uv);
	if(hovered)
		col *= 1.5;
	return float4(col,alpha);
}



float4 PSWater( PSNoBlendIn input ) : SV_TARGET
{
	float3 col = pic.Sample(bilinearSampler, input.TexCoord);
	float alpha = alphaMap.Sample(linearSampler, input.TexCoord);
	float3 fin = col * input.LightInt;
	return float4(fin, alpha);
}

float4 PSWaterPass1( PSNoBlendIn input ) : SV_TARGET
{
	float3 col = pic.Sample(bilinearSampler, input.TexCoord);
	float alpha = alphaMap.Sample(linearSampler, input.TexCoord);
	float3 fin = col * input.LightInt;
	return float4(fin, alpha * 0.7);
}


float4 PSRedColor(PSRecColorIn input) : SV_Target
{	
	return  float4(1,0,0,1);
}

float4 PSTree( PS_INPUT2 input ) : SV_Target
{
	float3 col = pic.Sample(linearSampler, input.uv);
	float alpha = alphaMap.Sample(linearSampler, input.uv);
	if(alpha == 0)
	{
		discard;
	}
	return float4( col * 0.6, alpha * fadefactor);
}
float4 PSTreeAlpha ( PS_INPUT2 input) : SV_Target
{
	float3 col = pic.Sample(linearSampler, input.uv);
	float alpha = alphaMap.Sample(linearSampler, input.uv);
	if(alpha > .99)
	{
		discard;
	}
	return float4( col * 0.6, alpha * fadefactor);
}

// **** **** **** **** **** **** //
// ***** Geometry  Shaders ***** //
// **** **** **** **** **** **** //
[maxvertexcount(4)]void GS( point SpriteInput s[1] , inout TriangleStream<PS_INPUT2> triStream )
{

	PS_INPUT2 p;

	//bottom left
	p.pos = float4(s[0].topLeft.x,s[0].topLeft.y-s[0].dimensions.y,s[0].topLeft.z,1);
	p.uv = float2(0,1);
	triStream.Append(p);
	 
	//top left
	p.pos = float4(s[0].topLeft.x,s[0].topLeft.y,s[0].topLeft.z,1);
	p.uv = float2(0,0);
	triStream.Append(p);
 
	//bottom right
	p.pos = float4(s[0].topLeft.x+s[0].dimensions.x,s[0].topLeft.y-s[0].dimensions.y,s[0].topLeft.z,1);
	p.uv = float2(1,1);
	triStream.Append(p);
	 
	//top right
	p.pos = float4(s[0].topLeft.x+s[0].dimensions.x,s[0].topLeft.y,s[0].topLeft.z,1);
	p.uv = float2(1,0);
	triStream.Append(p);
}


// **** ****  **** **** //
// **** Techniques **** //
// **** ****  **** **** //
technique10 DrawObject
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
	    
	    SetRasterizerState( backFaceCulling ); //NO should be back

	    SetDepthStencilState( EnableDepth, 0 );
	    //SetDepthStencilState( DisableDepth, 0 );
	    //SetDepthStencilState( EnableDepthTestOnly, 0 );
    }  
}
technique10 DrawNoMoveLight
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSNoMoveLight() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSNoBlend() ) );
	    
	    SetRasterizerState( backFaceCulling ); //correct

	    SetDepthStencilState( EnableDepth, 0 );
		SetBlendState( alphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );	
	    //SetDepthStencilState( DisableDepth, 0 );
	    //SetDepthStencilState( EnableDepthTestOnly, 0 );
    }  
}
technique10 DrawLine
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSRedColor() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSRedColor() ) );
	    
	    SetRasterizerState( backFaceCulling ); //correct

	    SetDepthStencilState( EnableDepth, 0 );
	    //SetDepthStencilState( DisableDepth, 0 );
	    //SetDepthStencilState( EnableDepthTestOnly, 0 );
    }  
}
technique10 DrawTweening
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSTweening() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSNoBlend() ) );
	    
	    SetRasterizerState( backFaceCulling ); //correct

	    SetDepthStencilState( EnableDepth, 0 );
		SetBlendState( alphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );	
	    //SetDepthStencilState( DisableDepth, 0 );
	    //SetDepthStencilState( EnableDepthTestOnly, 0 );
    }  
}
technique10 DrawSkyBox
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSSkyBox() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSSkyBox() ) );
	    
	    SetRasterizerState( backFaceCulling ); //NO! should be front

	    //SetDepthStencilState( EnableDepth, 0 );
	    SetDepthStencilState( DisableDepth, 0 );
	    //SetDepthStencilState( EnableDepthTestOnly, 0 );
    }  
}

technique10 DrawSprite
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSSprite() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );
        SetPixelShader( CompileShader( ps_4_0, PSSprite() ) );	
		SetRasterizerState( backFaceCulling );	
    }
}

technique10 DrawTransparentSprite
{
	pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSSprite() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );
        SetPixelShader( CompileShader( ps_4_0, PSAlphaSprite() ) );	
		SetRasterizerState( backFaceCulling );
		SetBlendState( alphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );		
    }
} 

technique10 DrawWater
{
	pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSWater() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSWater() ) );	
		SetRasterizerState( backFaceCulling );
		SetBlendState( alphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( EnableDepth, 0 );	
    }
	pass P1
    {
        SetVertexShader( CompileShader( vs_4_0, VSWaterPass1() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSWaterPass1() ) );	
		SetRasterizerState( backFaceCulling );
		SetBlendState( alphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( EnableDepth, 0 );		
    }
}

technique10 DrawTree
{
	//pass P0
	//{
		//SetVertexShader( CompileShader( vs_4_0, VSTree() ) );
        //SetGeometryShader( NULL );
        //SetPixelShader( CompileShader( ps_4_0, PSTreeAlpha() ) );	
		//SetRasterizerState( NoCulling );
		//SetBlendState( alphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		//SetDepthStencilState( DisableDepth, 0 );
	//}
	pass P1
    {
        SetVertexShader( CompileShader( vs_4_0, VSTree() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSTree() ) );	
		SetRasterizerState( NoCulling );
		SetBlendState( alphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( EnableDepth, 0 );
    }
}
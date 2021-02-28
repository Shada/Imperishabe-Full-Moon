#include "Camera.h"

Camera::Camera()
{
	vEyePt		= D3DXVECTOR3	( 0,  10, -20 );
	vLookAtPt	= D3DXVECTOR3	( 0, 0, 0 );
	vUp			= D3DXVECTOR3	( 0, 1, 0 );
	vDistance	= D3DXVECTOR3	( 0, -7, 15 );
	vDesiredPt	= D3DXVECTOR3	( 0,  10, -20 );

	mView		= new D3DXMATRIX();
	mProj		= new D3DXMATRIX();
	D3DXMatrixLookAtLH( mView, &vEyePt, &vLookAtPt, &vUp );
	D3DXMatrixPerspectiveLH(mProj, (float)D3DX_PI * .9f, (float)((float)SCREENWIDTH / (float)_SCREENHEIGHT), 1.0f, 3000.0f);
	buildViewFrustum();
}

void Camera::buildViewFrustum()
{
	D3DXMATRIX viewProj = *mView * *mProj;

	// Left plane
	frustum[0].a = viewProj._14 + viewProj._11;
    frustum[0].b = viewProj._24 + viewProj._21;
    frustum[0].c = viewProj._34 + viewProj._31;
    frustum[0].d = viewProj._44 + viewProj._41;
 
    // Right plane
    frustum[1].a = viewProj._14 - viewProj._11;
    frustum[1].b = viewProj._24 - viewProj._21;
    frustum[1].c = viewProj._34 - viewProj._31;
    frustum[1].d = viewProj._44 - viewProj._41;
 
    // Top plane
    frustum[2].a = viewProj._14 - viewProj._12;
    frustum[2].b = viewProj._24 - viewProj._22;
    frustum[2].c = viewProj._34 - viewProj._32;
    frustum[2].d = viewProj._44 - viewProj._42;
 
    // Bottom plane
    frustum[3].a = viewProj._14 + viewProj._12;
    frustum[3].b = viewProj._24 + viewProj._22;
    frustum[3].c = viewProj._34 + viewProj._32;
    frustum[3].d = viewProj._44 + viewProj._42;
 
    // Near plane
    frustum[4].a = viewProj._13;
    frustum[4].b = viewProj._23;
    frustum[4].c = viewProj._33;
    frustum[4].d = viewProj._43;
 
    // Far plane
    frustum[5].a = viewProj._14 - viewProj._13;
    frustum[5].b = viewProj._24 - viewProj._23;
    frustum[5].c = viewProj._34 - viewProj._33;
    frustum[5].d = viewProj._44 - viewProj._43;

	for(int i = 0; i < 6; i++)
		D3DXPlaneNormalize( &frustum[i], &frustum[i] );
}

void Camera::resetCamera()
{
	vLookAtPt	= D3DXVECTOR3	( 0.f, 0.f, 0.f );
	vUp			= D3DXVECTOR3	( 0.f, 1.f, 0.f );
	vDistance	= D3DXVECTOR3	( 0.f, -7.f, 15.f );
	vDesiredPt		= D3DXVECTOR3	( 0.f,  10.f, -20.f );
}

void Camera::Update( D3DXVECTOR3 pos, double dt ) //player pos
{
	pos.y += 5;
	vDesiredPt += pos - vLookAtPt;
	vEyePt.x += (float)((vDesiredPt.x - vEyePt.x) / .035f * dt);
	vEyePt.y += (float)((vDesiredPt.y - vEyePt.y) / .100f * dt);
	vEyePt.z += (float)((vDesiredPt.z - vEyePt.z) / .035f * dt);
	if(D3DXVec3Length(&(vEyePt - vDesiredPt)) < .01f)
		vEyePt = vDesiredPt;

	//vEyePt += (D3DXVec3Length( &(pos - vEyePt) ) < 20) ? temp : temp * .9999f ;
	vLookAtPt = pos;
	//create view matrix
	D3DXMatrixLookAtLH( mView, &vEyePt, &vLookAtPt, &vUp );
	//create frustum planes;
	buildViewFrustum();
}


void Camera::Rotate(long deltaX)
{
	fRotX = deltaX * 0.005f;

	D3DXMATRIX	mRotX;

	//create rotation matrices
	D3DXMatrixRotationAxis( &mRotX, &vUp, fRotX ); 
	
	D3DXVec3TransformCoord( &vDesiredPt, &vDesiredPt, &mRotX );
	D3DXVec3TransformCoord( &vLookAtPt, &vLookAtPt, &mRotX );
}

void Camera::Zoom(long deltaZ)
{
	vDistance			= vDesiredPt - vLookAtPt;
	D3DXVECTOR3 delta	= vDistance * (float)(deltaZ * 0.0005f);
	float lenght		= D3DXVec3Length( &(vDistance - delta) );

	if ( lenght < 50.f && lenght > 10.f)
		vDesiredPt -= delta;
}

void Camera::setEyePt(D3DXVECTOR3 pos)
{
	 pos.y += 5.f;
	 vDistance = D3DXVECTOR3( 0.f, -7.f, 15.f );
	 vEyePt = pos - vDistance;
	 vLookAtPt = pos;
	 vDesiredPt = vEyePt;
}

Camera::~Camera()
{
	SAFE_DELETE(mView);
	SAFE_DELETE(mProj);
}
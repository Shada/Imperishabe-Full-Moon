#pragma once
#include "stdafx.h"

class Camera
{
private:
	
    float           fSpeed;     //movement speed
	float			fRotX;		//rotation around y-axis

    D3DXMATRIX		*mView,		//view-matrix
					*mProj;				

	D3DXVECTOR3     vEyePt,     //eye point
					vDesiredPt,	//wanted point
                    vLookAtPt,  //look-at target
                    vUp,		//up-vector
					vDistance;	//distance to player

	D3DXPLANE		frustum[6];  //frustum planes
	
	void			buildViewFrustum();
		
public:

	Camera();
	~Camera();

	D3DXVECTOR3		*getPos()		{ return &vEyePt;				}
	D3DXVECTOR3		getLook()		{ return vLookAtPt;				}
	D3DXVECTOR3		getUp()			{ return vUp;					}
	D3DXVECTOR3		getDirection()	{ return vLookAtPt - vDesiredPt;}

	D3DXMATRIX		getViewMatrix()	{ return *mView; }
	D3DXMATRIX		getProjMatrix()	{ return *mProj; }
	D3DXPLANE		*getFrustumPlaneAt( int k ) { return &frustum[k];}

	void setEyePt	( D3DXVECTOR3 pos );
	void setPerspective( float ratio ) { D3DXMatrixPerspectiveLH(mProj, (float)D3DX_PI * ratio,
						(float)((float)SCREENWIDTH / (float)_SCREENHEIGHT), 1.0f, 3000.0f); }

    void Update		( D3DXVECTOR3 pos, double dt );	//update camera
    void Rotate		( long deltaX );		//rotate view
	void Zoom		( long deltaZ );		//zoom in player

	void resetCamera();
};
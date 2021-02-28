#pragma once
#include <windows.h>
#include <D3D10.h>
#include <D3DX10.h>

#include <string>
#include <vector>

#include <iostream>
#include <sstream>
using namespace std;

struct LineVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 texCoord;
	LineVertex(D3DXVECTOR3 pos, D3DXVECTOR3 normal, D3DXVECTOR2 texCoord)
	{
		this->normal = normal;
		this->pos = pos;
		this->texCoord = texCoord;
	}
	LineVertex()
	{
	}
};
struct SpriteVertex
{

	D3DXVECTOR3 anchor;
	D3DXVECTOR2 dimensions;
	SpriteVertex(D3DXVECTOR3 anchor,D3DXVECTOR2 dimensions)
	{
		this->anchor = anchor;
		this->dimensions = dimensions;
	}
};

#define TREE_VIEWDISTANCE (800)
#define SPRING_VIEWDISTANCE (700)
#define PLATFORM_VIEWDISTANCE (700)
#define DEFAULT_VIEWDISTANCE (500)
#define BUSHPLANT_VIEWDISTANCE (400)
#define POWERUP_VIEWDISTANCE (300)

#define SAFE_RELEASE(x) if( x ) { (x)->Release(); (x) = NULL; }
#define SAFE_DELETE(x) if( x ) { delete(x); (x) = NULL; }

#define PI (3.14159265358979323846f)
#define _SCREENHEIGHT (720)
#define SCREENWIDTH (1280)
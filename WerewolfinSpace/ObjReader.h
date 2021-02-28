#pragma once
#include <string>
#include <iostream>
#include <fstream>

#include "stdafx.h"
#include <math.h>
#include "Model.h"
#include "MtlReader.h"

class ObjReader
{
private:
	int groupIndex;

	vector<D3DXVECTOR3*> *vertices;
	vector<D3DXVECTOR3*> *normals;
	vector<D3DXVECTOR2*> *texels;

	vector<PolygonGroup> *groups;
	MtlReader			 *mtlreader;

	ifstream			 *datafile;

	void reset();

	void readVertex();
	void readFace();
	void readMtl();
	void readUse();
	void readGroup();

public:
	ObjReader();
	~ObjReader();
	
	void readData( string filename, Model* model );
};
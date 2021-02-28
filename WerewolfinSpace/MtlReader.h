#pragma once
#include "stdafx.h"
#include "Material.h"
#include <string>
#include <iostream>
#include <fstream>

class MtlReader
{
private:
	vector<Material*> materials;

public:
	MtlReader();
	~MtlReader();

	void readFile(string filename);

	Material *getMaterial(string name);

	void reset();
};
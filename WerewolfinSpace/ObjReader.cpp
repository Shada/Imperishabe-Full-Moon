#include "ObjReader.h"


ObjReader::ObjReader()
{
	//The current group that will be modified
	groupIndex = 0;

	//The vertices values
	vertices =	new vector<D3DXVECTOR3*>();
	normals	=	new vector<D3DXVECTOR3*>();
	texels =	new vector<D3DXVECTOR2*>();

	// mtlReader, so that we can read the mtl-file connected to a group
	mtlreader = new MtlReader();
}

void ObjReader::readVertex()
{
	// The controlsymbol is used for checking the first char in the file.
	char controlsymbol;
	// These are collected from the file.
	float x, y, z;
	// Peek at the first char.
	controlsymbol = datafile->peek();

	switch ( controlsymbol )
	{
		case 't': // It's a texcoord
		{
			*datafile >> controlsymbol >> x >> y;

			// Add the texcoord to the vector.
			texels->push_back( new D3DXVECTOR2( x, y ) );
			break;
		}

		case 'n': // It's a normal
		{
			*datafile >> controlsymbol >> x >> y >> z;

			// Add the normal to the vector
			normals->push_back( new D3DXVECTOR3( x, y, z ) );
			break;
		}

		case 'p': // This is not used, I don't do anything with it.
		{
			break;
		}

		default: // It's a vertexposition
		{
			*datafile >> x >> y >> z;

			// Add the position to the vector.
			vertices->push_back( new D3DXVECTOR3( x, y, z ) );
			break;
		}
	}
}

void ObjReader::readFace()
{
	// Indices.
	int vertIndex, texIndex, normIndex;
	// this is used for the '/'
	char separator;

	// Reads 3 vertices.
	for( int i = 0; i < 3; i++ )
	{

		// Vertex info.
		*datafile >> vertIndex >> separator >> 
				texIndex >> separator >> normIndex;

		// Feed the info to the current group.
		groups->at( groupIndex ).feedData( vertices->at( vertIndex - 1 ), 
							texels->at( texIndex - 1 ), normals->at( normIndex - 1 ) );
	}
}

void ObjReader::readMtl()
{
	string t;

	// Read the next word in the file.
	*datafile >> t;

	char buffer[64];

	// The complete word is "mtllib" 
	if( t == "tllib" )
	{
		*datafile >> t;
		// Read the mtl file
		mtlreader->readFile( t );
	}
	else
		datafile->getline( buffer, 64 );

}

void ObjReader::readGroup()
{
	string t;
	// Get the group name.
	*datafile >> t;
	groupIndex = groups->size();

	// We have to find out if the group exists 
	// or if we should create a new.
	int temp = groupIndex;

	for( unsigned int i = 0; i < groups->size(); i++ )
	{
		// Check if there exist a group with the name.
		if( t.find( groups->at( i ).getName() ) != string::npos )
			groupIndex = i;
	}
	if( groupIndex == temp )
	{
		// Add new group if there aren't any group with the name.
		groups->push_back( PolygonGroup( t ) );
	}
}

void ObjReader::readUse()
{
	string t;

	*datafile >> t;
	char buffer[32];

	// if the complete word is "usemtl"
	if( t == "semtl" )
	{
		// Get the material name to use.
		*datafile >> t;
		// Get the material with the name from the mtl reader
		// and add it to the current group.
		groups->at( groupIndex ).material = mtlreader->getMaterial( t );
	}
	else
		datafile->getline( buffer, 32 );
}

void ObjReader::readData( string filename, Model *model )
{
	reset();
	
	groups = model->getGroups();

	datafile = new ifstream( filename );

	if( datafile->is_open() )
	{
		char controlsymbol;
		char buffer[512];
		while( !datafile->eof() )
		{
			
			*datafile >> controlsymbol;
			
			switch ( controlsymbol )
			{
				case 'v': //vertex data
					readVertex();
					break;

				case 'f': //face data
					readFace();
					break;
				case 'm': //mtl?
					readMtl();
					break;
				case 'g':
					readGroup();
					break;
				case 'u':
					readUse();
					break;
				default:
					datafile->getline( buffer, 512 );
					break;
			}
		}
	}
	else
		cout << "Unable to open " << filename << endl;

	model->feedData();
	SAFE_DELETE( datafile );
}

void ObjReader::reset()
{
	//reset the mtl-reader so that we can use it for the new obj-file
	mtlreader->reset();

	// delete and clear the vertex-data
	unsigned int i;
	for(i = 0; i < vertices->size(); i++)
		SAFE_DELETE(vertices->at(i));
	for(i = 0; i < normals->size(); i++)
		SAFE_DELETE(normals->at(i));
	for(i = 0; i < texels->size(); i++)
		SAFE_DELETE(texels->at(i));
	vertices->clear();
	normals->clear();
	texels->clear();

	// set the groups to point to NULL
	groups = NULL;
}

ObjReader::~ObjReader()
{	
	//delete the mtlreader
	SAFE_DELETE(mtlreader);

	//delete all the vertex-data
	unsigned int i;
	for(i = 0; i < vertices->size(); i++)
		SAFE_DELETE(vertices->at(i));
	for(i = 0; i < normals->size(); i++)
		SAFE_DELETE(normals->at(i));
	for(i = 0; i < texels->size(); i++)
		SAFE_DELETE(texels->at(i));
	
	// delete the vectors
	SAFE_DELETE(vertices);
	SAFE_DELETE(normals);
	SAFE_DELETE(texels);
}

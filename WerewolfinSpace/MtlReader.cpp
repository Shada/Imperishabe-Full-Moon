#include "MtlReader.h"


MtlReader::MtlReader()
{
	// creating the vector that will be filled with materials.
	materials = vector<Material*>();
}

void MtlReader::readFile( string filename )
{
	// Opening the material-file 
	ifstream* file = new ifstream( filename.c_str() );
	if( file->is_open() )
	{
		char controlsymbol;
		char buffer[128];
		string s;
		while( !file->eof() )
		{
			*file >> controlsymbol;
			switch( controlsymbol )
			{
				case 'K': // Material's light coeff.
				{
					*file >> controlsymbol;
					switch( controlsymbol )
					{
						case 'a': // Ambient coeff.
							*file >> materials.back()->ka.x >> materials.back()->ka.y >> materials.back()->ka.z;
							break;
						case 'd': // Diffuse coeff.
							*file >> materials.back()->kd.x >> materials.back()->kd.y >> materials.back()->kd.z; 
							break;
						case 's': // Specular coeff.
							*file >> materials.back()->ks.x >> materials.back()->ks.y >> materials.back()->ks.z;
							break;
						default:
							//if it's not a valid coeff. the line will be thrown away
							file->getline( buffer, 128 );
					}
					break;
				}
				case 'T':
				{
					*file >> controlsymbol;
					switch(controlsymbol)
					{
						case 'f':
							file->getline( buffer, 128 ); //can be added later.
							break;
						case 'r': // Transparancy
							*file >> materials.back()->alpha;
							break;
						default:
							file->getline( buffer, 128 );
					}
					break;
				}
				case 'N':
				{
					*file >> controlsymbol;
					switch(controlsymbol)
					{
						case 'i':
							file->getline( buffer, 128 );
							break;
						case 's':
							*file >> materials.back()->ns;
							break;
						default:
							file->getline( buffer, 128 );
					}
					break;
				}
				case 'm': // Maps.
				{
					*file >> s;
					if( s == "ap_Kd" ) // Diffuse map.
						*file >> materials.back()->textureFileName;
					else if( s == "ap_d" ) // Alpha map
						*file >> materials.back()->alphaMapFileName;
					else
						file->getline( buffer, 128 );
					break;
				}
				case 'i':
				{
					*file >> s;
					if( s == "llum" )
						*file >> materials.back()->illum;
					else
						file->getline(buffer, 128);
					break;
				}
				case 'n':
				{
					s.clear();
					*file >> s;
					if( s == "ewmtl" ) // Add new material to the vector.
					{
						s.clear();
						*file >> s;
						materials.push_back( new Material( s.c_str() ) );
					}
					break;
				}
				default:
				{
					// throw away line
					file->getline( buffer, 128 );
					break;
				}
			}
		}
	}

	//deleting the filereader
	SAFE_DELETE( file );
}

void MtlReader::reset()
{
	// clear the list, so it can be reused.
	for(unsigned int i = 0; i < materials.size(); i++)
		materials.at(i) = NULL;
	materials.clear();
}

Material *MtlReader::getMaterial( string name )
{
	// Search the list for the material-name.
	for(unsigned int i = 0; i < materials.size(); i++)
		if(materials.at(i)->name == name)
			return materials.at(i);

	// If the material can't be found, return null.
	return NULL;
}
MtlReader::~MtlReader()
{
}

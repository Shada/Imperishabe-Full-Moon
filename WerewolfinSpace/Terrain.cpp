#include "Terrain.h"


Terrain::Terrain( D3DXVECTOR3 position, D3DXVECTOR2 dimensions, string sHeightMap, ID3D10EffectTechnique* tech,
        DxHandler *h, string blendMap, string texture1, string texture2, string texture3, float scale, float offset )
    : Object3D( -2,-2,-2,position, tech, D3DXVECTOR3( 0, 0, 0 ) )
{
    this->dimensions = dimensions;
    
    // This index is used when the terrain is divided into smaller bits for the quad-tree.
    subTerrainIndex = -1;
    
    // The max corner of the terrain. Every tile is 10 units big.
    max.x = dimensions.x * 10;
    max.z = dimensions.y * 10;

    vertexPoints = new vector<D3DXVECTOR3*>();
    normals = new vector<D3DXVECTOR3*>();
    texCoords = new vector<D3DXVECTOR2*>();
    
    // Create vertices and map the positions. Calculates the normals per vertex.
    createTexCoords();
    createVertexPoints( sHeightMap, scale, offset );
    mapPositions();
    createNormals();

    // These are the textures that will be blended for the terrain.
    ID3D10Effect* effect = h->getID3D10Effect();
    ID3D10ShaderResourceView* g_pShaderResourceView  = NULL;
    D3DX10CreateShaderResourceViewFromFile( h->getID3D10Device(), blendMap.c_str(), NULL, NULL, &g_pShaderResourceView, NULL );
    effect->GetVariableByName( "blendTex" )->AsShaderResource()->SetResource( g_pShaderResourceView );

    g_pShaderResourceView  = NULL;
    D3DX10CreateShaderResourceViewFromFile( h->getID3D10Device(), texture1.c_str(), NULL, NULL, &g_pShaderResourceView, NULL );
    effect->GetVariableByName( "terrain1" )->AsShaderResource()->SetResource( g_pShaderResourceView );
    
    g_pShaderResourceView  = NULL;
    D3DX10CreateShaderResourceViewFromFile( h->getID3D10Device(), texture2.c_str(), NULL, NULL, &g_pShaderResourceView, NULL );
    effect->GetVariableByName( "terrain2" )->AsShaderResource()->SetResource( g_pShaderResourceView );

    g_pShaderResourceView  = NULL;
    D3DX10CreateShaderResourceViewFromFile( h->getID3D10Device(), texture3.c_str(), NULL, NULL, &g_pShaderResourceView, NULL );
    effect->GetVariableByName( "terrain3" )->AsShaderResource()->SetResource( g_pShaderResourceView );
}

void Terrain::createVertexPoints( string sHeightMap, float scale, float offset )
{
    // Load the heightmap using the dimensions, scale and offset.
    HeightMap* heightMap = new HeightMap();
    heightMap->loadRaw( (int)dimensions.x, (int)dimensions.y, sHeightMap, scale, offset );

    vector<float> heights = heightMap->getHeights();

    int k;
    for( int y = 0; y < (int)dimensions.y; y++ )
    {
        for( int x = 0; x < (int)dimensions.x; x++ )
        {
            k = y * (int)dimensions.x + x;
            // Create a new vertexposition 10 units from the previous.
            vertexPoints->push_back( new D3DXVECTOR3( (float)x * 10.0f, 0, (float)y * 10.0f ) );
            // Change the height according to the heightmap.
            vertexPoints->back()->y = heights[k] + position.y;
            // Create a normal with no direction, this is needed for the normal calculation later.
            normals->push_back( new D3DXVECTOR3( 0.f, 0.f, 0.f ) );
        }
    }
    // Delete the heightmap.
    SAFE_DELETE( heightMap );
}

void Terrain::createTexCoords()
{
    // Create the 4 texcoords.
    texCoords->push_back( new D3DXVECTOR2( 0, 0 ) );
    texCoords->push_back( new D3DXVECTOR2( 0, 1 ) );
    texCoords->push_back( new D3DXVECTOR2( 1, 0 ) );
    texCoords->push_back( new D3DXVECTOR2( 1, 1 ) );
}

// Create indices for the vertexpoints and texcoords.
void Terrain::mapPositions()
{    
    int index1 = 0;
    int index2 = 0;

    // Tne less row of tiles than actual points
    for( int y = 0; y < dimensions.y - 1; y++ ) 
    {
        for( int x = 0; x < dimensions.x - 1; x++ )
        {
            // Index for the point in the bottom left corner in a tile.
            index1 = (int)(      y            * dimensions.x + x );
            // Index for the point in the top left corner in a tile.
            index2 = (int)( ( y + 1 )    * dimensions.x + x );

            
            // Three points per triangle and six for a square.
            vertexIndex.push_back( index1      );
            vertexIndex.push_back( index2      );
            vertexIndex.push_back( index2 + 1 );
            
            vertexIndex.push_back( index1      );
            vertexIndex.push_back( index2 + 1 );
            vertexIndex.push_back( index1 + 1 );
            

            // The texcoord indices. These are the same for all the tiles.
            texCoordIndex.push_back(0);
            texCoordIndex.push_back(2);
            texCoordIndex.push_back(1);

            texCoordIndex.push_back(1);
            texCoordIndex.push_back(2);
            texCoordIndex.push_back(3);

            numberOfVertices += 6;
        }
    }
}

void Terrain::createNormals()
{
    // Vectors used for calculating the normal.
    D3DXVECTOR3 side1;
    D3DXVECTOR3 side2;

    // The normal.
    D3DXVECTOR3 normal;

    // The three indices for the points in a triangle.
    int index1;
    int index2; 
    int index3;

    for(int i = 0; i < numberOfVertices / 3; i++)
    {    
        // Get three indices that corresponds to a triangle.
        index1 = vertexIndex.at(i * 3);
        index2 = vertexIndex.at(i * 3 + 1);
        index3 = vertexIndex.at(i * 3 + 2);
        
        // Calculate two sides of the triangle.
        side1 = *vertexPoints->at(index1) - *vertexPoints->at(index2);
        side2 = *vertexPoints->at(index1) - *vertexPoints->at(index3);
        
        // Cross product between the two sides results in the normal for the triangle.
        D3DXVec3Cross(&normal, &side1, &side2);

        // Adding the normal will give a vector pointing out from the vertexpoint.
        *normals->at(index1) += normal;
        *normals->at(index2) += normal;
        *normals->at(index3) += normal;
    }
    // Lastly, normalizing the normals.
    for( unsigned int i = 0; i < normals->size(); i++ )
        D3DXVec3Normalize( normals->at( i ), normals->at( i ) ); 
}

// Devides the terrain in several models that fits into the quad-tree.
int Terrain::subTerrain( D3DXVECTOR2 pos, D3DXVECTOR2 area, ID3D10Device *device )
{
    // Create a new model.
    model.push_back(new Model("Terrain"));
    // Get a pointer to the model's groups.
    vector<PolygonGroup> *groups = model.back()->getGroups();
    // Create a new group.
    groups->push_back(PolygonGroup("default"));
    
    int index1;
    int index2;
    // Go through all vertexPoints within the area of the quad-tree.
    for( int y = (int)pos.y; y < (int)( pos.y + area.y ); y++ )
    {
        for( int x = (int)pos.x; x < (int)( pos.x + area.x ); x++ )
        {
            // Find the indices of the bottom left and the top left corners.
            index1 = (int)( y         * dimensions.x + x );
            index2 = (int)( ( y + 1 ) * dimensions.x + x );

            // Add 6 positions, texcoords and normals to the groupat the back (terrain-models only need one group)
            // Do this as long as the x i s below 255.
            if( x < 255 && y < 255 )
            {
                groups->back().feedData( vertexPoints->at( index1 ),     texCoords->at(0), normals->at( index1 ) );
                groups->back().feedData( vertexPoints->at( index2 ),     texCoords->at(2), normals->at( index2 ) );
                groups->back().feedData( vertexPoints->at( index2 + 1 ), texCoords->at(1), normals->at( index2 + 1 ) );

                groups->back().feedData( vertexPoints->at( index1 ),     texCoords->at(1), normals->at( index1 ) );
                groups->back().feedData( vertexPoints->at( index2 + 1 ), texCoords->at(2), normals->at( index2 + 1 ) );
                groups->back().feedData( vertexPoints->at( index1 + 1 ), texCoords->at(3), normals->at( index1 + 1 ) );
            }
        }
    }
    // Create buffer and pData.
    model.back()->feedData();
    model.back()->createBuffer( device );
    
    // Set the groups to NULL.
    groups = NULL;

    // Return the supterrain index ti the quad-tree.
    return ++subTerrainIndex;
}

void Terrain::Draw( DxHandler* h, int subTerrainIndex )
{
    // Send in the world matrix to the shader.
    h->sendWorldMatrix( mWorld );
    // Draw the model at the index specified.
    model.at( subTerrainIndex )->Draw( h, tech );
}
Terrain::~Terrain()
{
    for(unsigned int i = 0; i < vertexPoints->size(); i++ )
        SAFE_DELETE( vertexPoints->at( i ) );
    for(unsigned int i = 0; i < texCoords->size(); i++ )
        SAFE_DELETE( texCoords->at( i ) );
    for(unsigned int i = 0; i < normals->size(); i++ )
        SAFE_DELETE( normals->at( i ) );
    for(unsigned int i = 0; i < model.size(); i++)
        SAFE_DELETE(model.at(i));
    SAFE_DELETE( texCoords );
    SAFE_DELETE( vertexPoints );
    SAFE_DELETE( normals );
}
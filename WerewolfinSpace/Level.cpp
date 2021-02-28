#include "Level.h"

Level::Level(DxHandler* dxHandler, int levelID, InputHandler *ih, Camera *camera)
{
    this->ih = ih;
    this->camera = camera;
    this->dxHandler = dxHandler;

    
    box = new vector<AABB*>();
    models = new vector<Model*>();
    objects = new vector<Object3D*>();
    powerUps = new vector<PowerUp*>();

    timePassed = 0;
    levelOver = false;

    quad = new QuadNode(3, D3DXVECTOR2(2560, 2560), D3DXVECTOR2(0, 0),camera);
    readLevelFile(levelID);

    D3DX10CreateFont(dxHandler->getID3D10Device(), 40, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
                OUT_TT_ONLY_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Verdana", &font);

    //if(!lvlFile.is_open())
    //        lvlFile.open("clockpos.txt",ios::out | ios::ate | ios::end);
}

void Level::readLevelFile(int levelID)
{
    vector<D3DXVECTOR4> trees;
    vector<D3DXVECTOR4> bushes;
    vector<D3DXVECTOR4> springs;
    vector<D3DXVECTOR4> plattforms;

    ID3D10Device *device = dxHandler->getID3D10Device();

    sprintf_s(buffer, "%d", levelID);
    string filename = "Levels/level" + (string)buffer + ".txt";
    vector<string> modelNames = vector<string>();

    vector<Model*> objectModels;

    char cSymbol;
    int iSymbol = -1;
    int modelIndex = -1, lastModelIndex = -1;
    string SSymbol;

    char crap;
    float x, y, z, scale, magnitude, xrot, yrot, zrot;
    ObjReader or = ObjReader();
    reader = new ifstream(filename);
    while (!reader->eof())
    {
        if(reader->peek() != '#')
            *reader >> iSymbol;
        else 
            reader->getline(buffer, 512);


        switch(iSymbol)
        {
            
            case 0: //read model
            {
                *reader >> SSymbol;
            
                models->push_back( new Model(SSymbol) );
                or.readData( "Models/" + SSymbol, models->back() );
                
                models->back()->createBuffer( device );
                models->back()->loadImageData( device );
                models->back()->loadAlphaMapData( device );

                break;
            }
            case 1: // name of heightmap

                *reader >> heightmap;
                *reader >> SSymbol;
                blendmap = "Images/" + SSymbol;

                *reader >> heightmapSizeX;
                *reader >> heightmapSizeZ;
            
                *reader >> SSymbol;
                tex1 = "Images/" + SSymbol;

                *reader >> SSymbol;
                tex2 = "Images/" + SSymbol;

                *reader >> SSymbol;
                tex3 = "Images/" + SSymbol;

                *reader >> scale >> magnitude;

                objects->push_back( new Terrain( D3DXVECTOR3( 0, 0, 0 ), D3DXVECTOR2( (float)heightmapSizeX, (float)heightmapSizeZ ), 
                    "Heightmaps/" + heightmap, dxHandler->g_pTechRenderLine, dxHandler, blendmap, tex1, tex2, tex3, scale, magnitude ) );

                quad->addTerrain((Terrain*)objects->back(), dxHandler->getID3D10Device());
                break;
            case 2: // name of skybox images (6 files)
            {
                vector<string> skybox;
                for(int c = 0; c < 6; c++)
                {
                    *reader >> SSymbol;
                    skybox.push_back("Images/" + SSymbol);
                }

                //add a model in the form of a box
                Block *block = new Block(D3DXVECTOR3(1, 1, 1), D3DXVECTOR3(0, 0, 0), dxHandler->g_pTechRenderSkyBox, device);
                //push it back into the models to be sent into the skybox
                objectModels.push_back( block->getModel());
                objects->push_back(new Skybox(dxHandler->g_pTechRenderSkyBox, device, camera->getPos(), objectModels, &skybox));
                SAFE_DELETE(block);
                for( unsigned int i = 0; i < objectModels.size(); i++)
                    objectModels.at(i) = NULL;

                //clear the modellist so that the next object can use it.
                objectModels.clear();
                break;    
            }
            case 3: //player
            {

                *reader >> x >> crap >> y >> crap >> z >> scale >> modelIndex >> lastModelIndex;

                playerStartPos = D3DXVECTOR3(x, y, z);

                //add all the models that the player will use in its morphing to the list that will be
                //sent into the player
                for(int i = modelIndex; i <= lastModelIndex; i++)
                    objectModels.push_back(models->at(i));

                player = new Player(playerStartPos, ih, camera, (Terrain*)objects->at(1), 
                        dxHandler->g_pTechRenderAnimated, objectModels, scale, dxHandler);
                
                for( unsigned int i = 0; i < objectModels.size(); i++)
                    objectModels.at(i) = NULL;

                //clear the modellist so that the next object can use it.
                objectModels.clear();
                break;
            }
            case 4: // platform
            {
                *reader >> x >> crap >> y >> crap >> z >> scale >> modelIndex >> yrot;

                
                objectModels.push_back(models->at(modelIndex));

                objects->push_back(new Platform(D3DXVECTOR3(x, y, z),
                                    objectModels, dxHandler->g_pTechRenderNoMoveLight, scale, D3DXVECTOR3(0, yrot, 0)));
                quad->addObject(objects->back());

                for( unsigned int i = 0; i < objectModels.size(); i++)
                    objectModels.at(i) = NULL;

                //clear the modellist so that the next object can use it.
                objectModels.clear();
                break;
            }
            
            case 5: // spring
            {
                
                *reader >> x >> crap >> y >> crap >> z >> scale >> modelIndex>> lastModelIndex >> magnitude
                    >> xrot >> crap >> yrot >> crap >> zrot;

                //this object uses more than one model
                for(int i = modelIndex; i <= lastModelIndex; i++)
                    objectModels.push_back(models->at(i));

                objects->push_back(new Springboard(D3DXVECTOR3(x, y, z),
                    objectModels, dxHandler->g_pTechRenderAnimated, scale, magnitude,D3DXVECTOR3(xrot,yrot,zrot)));
                quad->addObject(objects->back());

                for( unsigned int i = 0; i < objectModels.size(); i++)
                    objectModels.at(i) = NULL;

                //clear the modellist so that the next object can use it.
                objectModels.clear();
                break;
            }
            case 6: // tree normal
            {
                *reader >> x >> crap >> y >> crap >> z >> scale >> modelIndex >> xrot >> crap >> yrot >> crap >> zrot;

                objectModels.push_back(models->at(modelIndex));
                objects->push_back(new Tree(D3DXVECTOR3(x,y,z), objectModels, dxHandler->g_pTechRenderTree, 
                    scale, D3DXVECTOR3(xrot,yrot,zrot)));

                trees.push_back(D3DXVECTOR4(x, y, z, scale));
                quad->addObject(objects->back());

                for( unsigned int i = 0; i < objectModels.size(); i++)
                    objectModels.at(i) = NULL;

                //clear the modellist so that the next object can use it.
                objectModels.clear();
                break;
            }
            case 7: // bush
            {
                *reader >> x >> crap >> y >> crap >> z >> scale >> modelIndex
                    >> xrot >> crap >> yrot >> crap >> zrot;

                objectModels.push_back(models->at(modelIndex));
                objects->push_back(new Object3D(-2,-2,-2,D3DXVECTOR3(x, y, z), objectModels, dxHandler->g_pTechRenderNoMoveLight, scale,D3DXVECTOR3(xrot,yrot,zrot)));
                quad->addObject(objects->back());

                for( unsigned int i = 0; i < objectModels.size(); i++)
                    objectModels.at(i) = NULL;

                //clear the modellist so that the next object can use it.
                objectModels.clear();
                break;
            }
            case 8: //soundname
            {
                *reader >> soundName;
                break;
            }
            case 9: // water

                *reader >> x >> crap >> y >> crap >> z;
                *reader >> tex1 >> tex2 >> scale;

                //add all the power ups into the quad and the objectlist, just before the water is added, since the
                //water should be last in the list
                powerUpStartIndex = objects->size();
                for(unsigned int c = 0; c < powerUps->size(); c++)
                {
                    objects->push_back(powerUps->at(c));
                    quad->addObject(objects->back());
                }
                //add the water
                objects->push_back(new Water(D3DXVECTOR3(x, y, z), D3DXVECTOR2((float)heightmapSizeX, (float)heightmapSizeZ) * 40.f,
                            "Images/" + tex1, "Images/" + tex2, dxHandler, scale));

                //the water is not added to the quad since it is just a plane it would be unnecessary to devide it
                break;
            
            case 10: //clock
            {
                *reader >> x >> crap >> y >> crap >> z >> scale >> modelIndex >> magnitude >>
                    xrot >> crap >> yrot >> crap >> zrot;

                objectModels.push_back(models->at(modelIndex));

                powerUps->push_back(new Clock(D3DXVECTOR3(x, y, z), scale, objectModels,
                                        dxHandler->g_pTechRenderNoMoveLight, magnitude, this, D3DXVECTOR3(xrot,yrot,zrot)));

                for( unsigned int i = 0; i < objectModels.size(); i++)
                    objectModels.at(i) = NULL;

                //the power ups are not added to the object list or the quad yet, 
                //they will be added all together right before the water is addded

                //clear the modellist so that the next object can use it.
                objectModels.clear();
                break;
            }
            case 11://waterlilly
            {
                
                *reader >> x >> crap >> y >> crap >> z >> scale >> modelIndex >> lastModelIndex
                    >> xrot >> crap >> yrot >> crap >> zrot;

                //the waterlilly also uses seeral models
                for(int i = modelIndex; i <= lastModelIndex; i++)
                    objectModels.push_back(models->at(i));

                objects->push_back(new Waterlilly(D3DXVECTOR3(x, y, z),
                    objectModels, dxHandler->g_pTechRenderAnimated, scale,D3DXVECTOR3(xrot,yrot,zrot)));
                quad->addObject(objects->back());

                for( unsigned int i = 0; i < objectModels.size(); i++)
                    objectModels.at(i) = NULL;

                //clear the modellist so that the next object can use it.
                objectModels.clear();
                break;
            }
            case 12: //checkpoints
            {
                *reader >> x >> crap >> y >> crap >> z >> scale >> modelIndex >>
                    xrot >> crap >> yrot >> crap >> zrot;

                objectModels.push_back(models->at(modelIndex));

                powerUps->push_back(new CheckPoint(D3DXVECTOR3(x, y, z), scale, objectModels,
                                        dxHandler->g_pTechRenderNoMoveLight, this ,D3DXVECTOR3(xrot,yrot,zrot)));

                for( unsigned int i = 0; i < objectModels.size(); i++)
                    objectModels.at(i) = NULL;

                //clear the modellist so that the next object can use it.
                objectModels.clear();
                break;
            }
            case 13: //moving platform
            {
                //all subdestinations will be saved in this vector
                vector<D3DXVECTOR3> destinations;
                float speed;

                //there will always be one destination 
                destinations.push_back(D3DXVECTOR3(0,0,0));
                *reader >> x >> crap >> y >> crap >> z >> scale >> modelIndex >> yrot >> speed
                    >> destinations.back().x >> crap >> destinations.back().y >> crap >> destinations.back().z;

                //if this char is found it means there is another destination
                while(reader->peek() == ':')
                {
                    destinations.push_back(D3DXVECTOR3(0,0,0));
                    *reader >> crap >> destinations.back().x >> crap >> destinations.back().y >> crap >> destinations.back().z;
                }

                objectModels.push_back(models->at(modelIndex));
                objects->push_back(new MovingPlatform(D3DXVECTOR3(x, y, z),
                                    objectModels, dxHandler->g_pTechRenderNoMoveLight, scale,
                                    D3DXVECTOR3(0, yrot, 0),destinations,speed));
                quad->addObject(objects->back());

                for( unsigned int i = 0; i < objectModels.size(); i++)
                    objectModels.at(i) = NULL;

                //clear the modellist so that the next object can use it.
                objectModels.clear();
                break;
            }
            case 14: //goal
            {
                *reader >> x >> crap >> y >> crap >> z >> scale >> modelIndex >>
                    xrot >> crap >> yrot >> crap >> zrot;

                objectModels.push_back(models->at(modelIndex));

                powerUps->push_back(new Goal(D3DXVECTOR3(x, y, z), scale, objectModels,
                                        dxHandler->g_pTechRenderNoMoveLight, this, D3DXVECTOR3(xrot,yrot,zrot)));

                for( unsigned int i = 0; i < objectModels.size(); i++)
                    objectModels.at(i) = NULL;

                //clear the modellist so that the next object can use it.
                objectModels.clear();
                break;
            }
            case 15: //flash
            {
                float duration;
                *reader >> x >> crap >> y >> crap >> z >> scale >> modelIndex >> magnitude >> duration >> 
                    xrot >> crap >> yrot >> crap >> zrot;

                objectModels.push_back(models->at(modelIndex));

                powerUps->push_back(new Flash( D3DXVECTOR3(x,y,z), scale, objectModels, dxHandler->g_pTechRenderNoMoveLight,
                    this, D3DXVECTOR3(xrot, yrot, zrot), magnitude, duration));

                for( unsigned int i = 0; i < objectModels.size(); i++)
                    objectModels.at(i) = NULL;

                //clear the modellist so that the next object can use it.
                objectModels.clear();
                break;
            }
            case 16:
            { // spikes
                *reader >> x >> crap >> y >> crap >> z >> scale >> modelIndex >> xrot >> crap >> yrot >> crap >> zrot;

                objectModels.push_back(models->at(modelIndex));
                objects->push_back(new Spike(D3DXVECTOR3(x,y,z), objectModels, dxHandler->g_pTechRenderNoMoveLight, 
                    scale, D3DXVECTOR3(0,0,0)));
                quad->addObject(objects->back());

                for( unsigned int i = 0; i < objectModels.size(); i++)
                    objectModels.at(i) = NULL;

                //clear the modellist so that the next object can use it.
                objectModels.clear();
                break;
            }

            default:

                cSymbol = reader->peek();
                if(cSymbol > 47 && cSymbol < 58)
                    iSymbol = cSymbol - 48;
                else
                    reader->getline(buffer, 512);
                break;
        }
    }
    reader->close();
    SAFE_DELETE(reader);
}

void Level::restartLevel()
{
    Object3D *temp = objects->back();
    for(int c = powerUpStartIndex; (unsigned)powerUpStartIndex < objects->size(); c++)
    {
        quad->removeObject( objects->at( powerUpStartIndex ) );
        objects->erase(objects->begin() + powerUpStartIndex);
    }
    for(unsigned int c = 0; c < powerUps->size(); c++)
    {
        objects->push_back( (PowerUp*)powerUps->at( c ) );
        quad->addObject( (PowerUp*)powerUps->at( c ) );
    }

    objects->push_back((Water*)temp);
    player->setPos(playerStartPos);

    timePassed = 0;
    player->resetPlayer();

    levelOver = false;
    temp = NULL;

    Sleep(1000);
}

void Level::Draw(double dt)
{
    //the skybox
    objects->front()->Draw(dxHandler);
    
    //the player
    player->Draw(dxHandler);

    quad->DrawTerrain(dxHandler);


    //the water
    objects->back()->Draw(dxHandler);

    //everything in he quad
    quad->Draw(dxHandler, player->getPos());

    

    //the timer
    DrawTime(dt);
}
void Level::DrawTime(double dt)
{
    r.top = 30;
    r.bottom =  80;
    r.left =  (long)(SCREENWIDTH * .5f) - 100;
    r.right = (long)(SCREENWIDTH * .5f) + 100;

    double temp;
    temp = (double)((timePassed) / 10.f);// * dt * 60) / 100.f;

    int minutes = (int)(temp / 3600);
    int seconds = (int)(temp / 60 - minutes * 60);
    int milliseconds = (int)(temp * (5.f / 3.f)) % 100;

    std::ostringstream min, sec, millisec;
    min << minutes;
    sec << seconds;
    millisec << milliseconds;

    string text;

    if(minutes < 10)
        text = "0";
    text += min.str() + ":";
    if(seconds < 10)
        text += "0";
    text += sec.str() + ":";
    if(milliseconds < 10)
        text += "0";
    text += millisec.str();

    //string text = "Time passed: " + time.str(); 
    font->DrawTextA(NULL, text.c_str(), -1, &r, DT_LEFT | DT_CENTER, D3DXCOLOR(1, 1, 1, 1.0f));
}
void Level::Update(double dt)
{
    quad->CheckCollision(player);
    objects->front()->Update(dt, player);

    player->Update(dt);
    for(unsigned int i = 1; i < objects->size(); i++)
        objects->at(i)->Update(dt, player);

    if(dt < 1)
        timePassed += (long)(10 * dt * 60);


    if(player->getPos().y < objects->back()->getPos().y - 9)
        player->death();


    //if(ih->getKeyState()[DIK_L] && !ih->getPrevKeyState()[DIK_L])
    //{
    //    lvlFile << "10  "  << player->getPos().x << "/" << player->getPos().y << "/" << player->getPos().z<< " .5 10 1 0/0/0"<< endl;


    //}
    //if(ih->getKeyState()[DIK_B] && !ih->getPrevKeyState()[DIK_B])
    //{
    //    float rot = (rand() % (628)) * .01f;
    //    lvlFile << "7  "  << player->getPos().x << "/" << player->getPos().y << "/" << player->getPos().z<< " 1 15 0/" <<rot<<"/0"<< endl;


    //    vector<Model*> objectModels;
    //    objectModels.push_back(models->at(15));
    //    Object3D* temp = objects->back();
    //    objects->pop_back();
    //    objects->push_back (new Object3D(-2,-2,-2,player->getPos(), objectModels, dxHandler->g_pTechRenderNoMoveLight, 1 ,D3DXVECTOR3(0,rot,0)));
    //    quad->addObject(objects->back());
    //    objects->push_back(temp);
    //    

    //}
    //    if(ih->getKeyState()[DIK_1] && !ih->getPrevKeyState()[DIK_1])
    //{
    //    lvlFile << "4 "  << (int)(player->getPos().x + 0.5) << "/" << (int)(player->getPos().y + 0.5) -4<< "/" 
    //                << (int)(player->getPos().z + 0.5)<< "    0.6    11    0"<< endl;

    //    vector<Model*> objectModels;
    //    objectModels.push_back(models->at(11));
    //    Object3D* temp = objects->back();
    //    objects->pop_back();
    //    objects->push_back (new Platform(D3DXVECTOR3(player->getPos().x,player->getPos().y-4,player->getPos().z), 
    //                objectModels, dxHandler->g_pTechRenderNoMoveLight, 0.6f ,D3DXVECTOR3(0,0,0)));
    //    quad->addObject(objects->back());
    //    objects->push_back(temp);
    //    
    //}
    //if(ih->getKeyState()[DIK_2] && !ih->getPrevKeyState()[DIK_2])
    //{
    //    lvlFile << "5 "  << (int)(player->getPos().x + 0.5) << "/" << (int)(player->getPos().y + 0.5) -4<< "/" 
    //                << (int)(player->getPos().z + 0.5)<< "    0.6    12    13    100    0/0/0"<< endl;

    //    vector<Model*> objectModels;
    //    objectModels.push_back(models->at(12));
    //    objectModels.push_back(models->at(13));
    //    Object3D* temp = objects->back();
    //    objects->pop_back();

    //    objects->push_back (new Springboard(D3DXVECTOR3(player->getPos().x,player->getPos().y - 5, player->getPos().z), 
    //                objectModels, dxHandler->g_pTechRenderAnimated, 0.6f ,100, D3DXVECTOR3(0,0,0)));
    //    
    //    quad->addObject(objects->back());
    //    objects->push_back(temp);
    //    
    //}
    //if(ih->getKeyState()[DIK_3] && !ih->getPrevKeyState()[DIK_3])
    //{
    //    float rot = (rand() % (628)) * .01f;
    //    lvlFile << "6 "  << (int)(player->getPos().x + 0.5) << "/" << (int)(player->getPos().y + 0.5) - 5<< "/" 
    //                << (int)(player->getPos().z + 0.5) << "    0.37    23    0/"<<rot<<"/0"<< endl;

    //    vector<Model*> treeModels;
    //    treeModels.push_back(models->at(23));
    //    Object3D* temp = objects->back();
    //    objects->pop_back();

    //    objects->push_back (new Tree(D3DXVECTOR3(player->getPos().x,player->getPos().y - 5, player->getPos().z), 
    //                treeModels, dxHandler->g_pTechRenderTree, 0.37f, D3DXVECTOR3(0,rot,0)));
    //    
    //    quad->addObject(objects->back());
    //    objects->push_back(temp);
    //    
    //}
    //if(ih->getKeyState()[DIK_4] && !ih->getPrevKeyState()[DIK_4])
    //{
    //    float rot = (rand() % (628)) * .01f;
    //    lvlFile << "6 "  <<( int)(player->getPos().x + 0.5) << "/" << (int)(player->getPos().y + 0.5) - 5<< "/" 
    //                << (int)(player->getPos().z + 0.5) << "    0.35    24    0/"<<rot<<"/0"<< endl;

    //    vector<Model*> treeModels;
    //    treeModels.push_back(models->at(24));
    //    Object3D* temp = objects->back();
    //    objects->pop_back();

    //    objects->push_back (new Tree(D3DXVECTOR3(player->getPos().x,player->getPos().y - 5, player->getPos().z), 
    //                treeModels, dxHandler->g_pTechRenderTree, 0.35f, D3DXVECTOR3(0,rot,0)));
    //    
    //    quad->addObject(objects->back());
    //    objects->push_back(temp);
    //    
    //}
    //if(ih->getKeyState()[DIK_5] && !ih->getPrevKeyState()[DIK_5])
    //{
    //    float rot = (rand() % (628)) * .01f;
    //    lvlFile << "6 "  << (int)(player->getPos().x + 0.5) << "/" << (int)(player->getPos().y + 0.5) - 5<< "/" 
    //                << (int)(player->getPos().z + 0.5) << "    0.35    20    0/"<<rot<<"/0"<< endl;

    //    vector<Model*> treeModels;
    //    treeModels.push_back(models->at(20));
    //    Object3D* temp = objects->back();
    //    objects->pop_back();

    //    objects->push_back (new Tree(D3DXVECTOR3(player->getPos().x,player->getPos().y - 5, player->getPos().z), 
    //                treeModels, dxHandler->g_pTechRenderTree, 0.35f, D3DXVECTOR3(0,rot,0)));
    //    
    //    quad->addObject(objects->back());
    //    objects->push_back(temp);
    //    
    //}

    //if(ih->getKeyState()[DIK_I] && !ih->getPrevKeyState()[DIK_I])
    //{

    //    //beginning to saving lvlfiles
    //    ofstream lvlFile2;

    //    if(!lvlFile2.is_open())
    //        lvlFile2.open("Levels/testlvl.txt",ios::out | ios::ate | ios::end);

    //    for(unsigned int i = 0; i < models->size(); i++)
    //        lvlFile2 << "0 " + models->at(i)->getName() << endl; 


    //    lvlFile2.close();
    //    //for(unsigned int i = 0; i < objects->size(); i++) 
    //    //    lvlFile2 <<
    //}
}

bool Level::Peek(int index)
{
    char mapID[2];
    sprintf_s(mapID, "%d", index);
    string filename = "Levels/level" + (string)mapID + ".txt";

    ifstream *a = new ifstream(filename);
    if(a->is_open())
    {
        a->close();

        SAFE_DELETE(a);
        return true;
    }

    SAFE_DELETE(a);
    return false;
}

void Level::deleteThisObject(Object3D *obj)
{
    quad->removeObject(obj);
    for(unsigned int c = 0; c < objects->size(); c++)
        if(objects->at(c) == obj)
        {
            objects->erase(objects->begin() + c);
            break;
        }
}

Level::~Level()
{
    for(unsigned int i = 0; i < objects->size(); i++)
        SAFE_DELETE(objects->at(i));
    for(unsigned int i = 0; i < models->size(); i++)
        SAFE_DELETE(models->at(i));

    dxHandler = NULL;
    ih = NULL;
    camera = NULL;
    
    SAFE_RELEASE(font);

    SAFE_DELETE(powerUps);
    SAFE_DELETE(models);
    SAFE_DELETE(quad);
    SAFE_DELETE(objects);
    SAFE_DELETE(player);
}

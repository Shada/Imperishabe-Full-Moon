#pragma once
#include "Block.h"
#include "ObjReader.h"
#include "Player.h"
#include "Platform.h"
#include "MovingPlatform.h"
#include "PowerUp.h"
#include "QuadNode.h"
#include "Skybox.h"
#include "SoundHandler.h"
#include "Springboard.h"
#include "Water.h"
#include "Goal.h"
#include "Waterlilly.h"
#include "Clock.h"
#include "CheckPoint.h"
#include "Flash.h"
#include "Spike.h"
#include "Tree.h"

class Level
{
private:

    vector<Object3D*>    *objects;
    vector<Model*>        *models;
    vector<PowerUp*>    *powerUps;

    DxHandler            *dxHandler;
    InputHandler        *ih;
    SoundHandler        *sh;

    Camera                *camera;
    Player                *player;
    QuadNode            *quad;
    ID3DX10Font            *font;

    string                soundName;

    // If a level is restarted we don't want to read the file again, so the start position is stored
    D3DXVECTOR3            playerStartPos;

    long                timePassed;
    ifstream            *reader;
    
    RECT r;
    string heightmap, blendmap, tex1, tex2, tex3, tex4;
    int heightmapSizeX, heightmapSizeZ, powerUpStartIndex;

    vector<AABB*> *box;
    char buffer[512];

    void readLevelFile(int levelID);

public:
    // The goal must be able to somehow return that the player has completed the map, and the game class needs to 
    // notice when this has happened. Since it is both set and get, the bool is public
    bool levelOver;

    Level(DxHandler* dxHandler, int levelID, InputHandler *ih, Camera *camera);
    ~Level();

    vector<Object3D*>* getObjects(){ return objects; } //should be const since only level should be able to temper in it

    void Draw(double dt);
    void DrawTime(double dt);
    void Update(double dt);
    // Control whether or not there is a level with the sent in index, thus the program doesn't need to know how many levels there are
    bool Peek(int index);
    
    void restartLevel();
    void decreaseTimePassed(float time) { timePassed -= ((long)timePassed - time * 600 > 0 ) ? (long)(time * 600) : (long)(timePassed - 1); }
    // Power ups should be removed upon activation
    void deleteThisObject(Object3D *obj);

    string getSoundName(){ return soundName; }
    int getScore() { return (int)(100000.0f / (timePassed * .08f) * 10000); }
};
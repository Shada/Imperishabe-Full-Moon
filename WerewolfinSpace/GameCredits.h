#pragma once
#include "stdafx.h"
#include "DxHandler.h"
class GameCredits
{
private:
    ID3DX10Font* font;
    ID3DX10Sprite* sprite;

    RECT r;

    string names[4];
public:

    GameCredits(ID3DX10Font* font,ID3DX10Sprite* sprite);
    ~GameCredits();

    void Draw(DxHandler* h);
};
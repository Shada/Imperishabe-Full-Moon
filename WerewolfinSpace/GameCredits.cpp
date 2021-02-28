#include "GameCredits.h"


GameCredits::GameCredits(ID3DX10Font* font,ID3DX10Sprite* sprite)
{
    this->font = font;
    this->sprite = sprite;
    names[0] = "Emil Bertilsson";
    names[1] = "Stefan Grahn";
    names[2] = "Jimmy Gustafsson";
    names[3] = "Tobias Andersson";

    // Don't need to set this evert time sice it will not change
    r.left = (long)(SCREENWIDTH * .5f) - 150;
    r.right = (long)(SCREENWIDTH * .5f) + 150;

}
void GameCredits::Draw(DxHandler* h)
{
    sprite->Begin(0);
    // Draw the names with a fixed distance
    for(int i = 0; i < 4; i++)
    {
        r.top = 200 + i* 60;
        r.bottom =  260 + i * 60;
        font->DrawTextA(sprite, names[i].c_str(), -1, &r, DT_LEFT | DT_CENTER , D3DXCOLOR(0.324f, 0.027f, 0.027f, 1.0f));
    }
    sprite->End();

    h->getID3D10Device()->OMSetBlendState(NULL, 0, 0xffffffff);
}

GameCredits::~GameCredits()
{
    font = NULL;
    sprite = NULL;
}
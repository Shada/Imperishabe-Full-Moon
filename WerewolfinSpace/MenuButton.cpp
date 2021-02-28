#include "MenuButton.h"

MenuButton::MenuButton(string text,D3DXVECTOR2 anchor,D3DXVECTOR2 dimensions,ID3D10ShaderResourceView* background,
    ID3D10Buffer* g_pVB, int bufferOffset, SpriteVertex* pData,DxHandler* h)
{
    this->anchor = anchor;
    this->dimensions = dimensions;
    this->text = text;
    this->bufferOffset = bufferOffset;
    this->g_pVB = g_pVB;
    this->pData = pData;

    // The image the button will have on it
    this->background = new Sprite(anchor, dimensions, background, g_pVB, bufferOffset, pData);
    
    init( h );
}

MenuButton::MenuButton(    string text, D3DXVECTOR2 anchor, D3DXVECTOR2 dimensions, ID3D10ShaderResourceView* background,
                ID3D10ShaderResourceView* alpha, ID3D10Buffer* g_pVB, int bufferOffset, SpriteVertex* pData, DxHandler* h)
{
    this->anchor = anchor;
    this->dimensions = dimensions;
    this->text = text;
    this->bufferOffset = bufferOffset;
    this->g_pVB = g_pVB;
    this->pData = pData;
    
    // The image the button will have on it
    this->background = new Sprite(anchor, dimensions, background, alpha, g_pVB, bufferOffset, pData);
    
    init( h );
}

void MenuButton::init( DxHandler *h )
{
    D3DX10CreateFont( h->getID3D10Device(), 16, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
                        OUT_TT_ONLY_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Verdana", &font );
    D3DX10CreateSprite(h->getID3D10Device(), 256, &sprite);

    r.top = (long)(anchor.y + dimensions.y * .5f - 8);
    r.bottom = (long)(anchor.y + dimensions.y);
    r.left = (long)anchor.x;
    r.right = (long)(anchor.x + dimensions.x);
}

void MenuButton::Draw(DxHandler* h)
{
    background->Draw(h);
    sprite->Begin(0);

    font->DrawTextA(sprite, text.c_str(), -1, &r, DT_LEFT | DT_CENTER , D3DXCOLOR(0.324f, 0.027f, 0.027f, 1.0f));
    
    //sprite->Flush();
    sprite->End();
    
    h->getID3D10Device()->OMSetBlendState( NULL, 0, 0xffffffff );
    
}

MenuButton::~MenuButton()
{
    g_pVB = NULL;
    pData = NULL;
    SAFE_DELETE(pData);
    SAFE_RELEASE(font);
    SAFE_RELEASE(sprite);

    SAFE_DELETE(background);
}
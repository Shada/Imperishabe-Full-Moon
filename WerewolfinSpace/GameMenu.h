#pragma once
#include "BaseMenu.h"

class GameMenu :
    public BaseMenu
{
private:
    GameCredits* credits;

    // For the next and previous buttons
    bool noNext, noPrev;
    int indexOfHighScore, currentResolution, selectMapIndex;

    vector<string> resolutions;
    vector<D3DXVECTOR2> resolutionSizes;

    string screenResolution;
public:
    
    GameMenu(DxHandler* dxHandler, InputHandler* inputHandler, Game* game);
    ~GameMenu();
    
    void Update(HighScore *&hs);
    void Draw(HighScore *hs);

    // When the player return to the menu from the game, we want to return to the main state
    void setToMainState();

    enum MenuState
    {
        Main,
        Highscore,
        Options,
        Credits,
        SelectChapter,
        EndScreen
    };
    MenuState mState;
};
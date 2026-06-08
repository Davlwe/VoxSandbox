#pragma once

#include "World.h"
#include "Player.h"
#include "Constants.h"

enum class GameState {
    MainMenu,
    Playing
};

class Game {
public:
    Game(int screenWidth, int screenHeight);
    void Run();

private:
    int m_ScreenWidth;
    int m_ScreenHeight;
    GameState m_State;

    // World & player (only active during Playing)
    World  m_World;
    Player m_Player;

    // Hotbar
    BlockType m_Hotbar[9];
    int       m_SelectedSlot;

    // Per-frame helpers
    void UpdatePlaying(float dt);
    void DrawPlaying();
    void DrawPlayingUI();
};

#include "Game.h"
#include "Block.h"
#include "raylib.h"

int main() {
    const int screenWidth  = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "VoxSandbox");
    SetTargetFPS(60);
    EnableCursor();

    InitBlocks();   // load/generate textures

    Game game(screenWidth, screenHeight);
    game.Run();

    UnloadBlocks();
    CloseWindow();
    return 0;
}

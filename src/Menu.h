#pragma once

#include "raylib.h"

enum class MenuAction {
    None,
    Play,
    Quit
};

// Draw the main menu and return the button the user clicked.
// Call once per frame while in the MainMenu state.
MenuAction UpdateMenu(int screenWidth, int screenHeight);

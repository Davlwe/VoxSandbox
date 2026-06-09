#pragma once

#include "raylib.h"

// Draw a simple crosshair at the center of the screen
void DrawCrosshair(int screenWidth, int screenHeight);

// Draw a 9-slot hotbar at the bottom of the screen.
// `selected` is 0–8 for the currently highlighted slot.
void DrawHotbar(int screenWidth, int screenHeight, int selected);

// Draw the name of the currently held block top-right
void DrawHeldBlockName(int screenWidth, int screenHeight, const char* name);

#pragma once

#include "raylib.h"

// Draw a simple crosshair at the center of the screen
void DrawCrosshair(int screenWidth, int screenHeight);

// Draw a 4-slot hotbar at the bottom of the screen.
// `selected` is 0–3 for the currently highlighted slot.
void DrawHotbar(int screenWidth, int screenHeight, int selected);

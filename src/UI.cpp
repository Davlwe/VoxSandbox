#include "UI.h"
#include "Block.h"
#include "Constants.h"
#include <cstring>

// ---------------------------------------------------------------------------
// Crosshair — two thin lines at screen center
// ---------------------------------------------------------------------------
void DrawCrosshair(int screenWidth, int screenHeight) {
    int cx = screenWidth  / 2;
    int cy = screenHeight / 2;
    int gap  = 8;   // half-length of each line in pixels
    int thick = 2;

    // Horizontal
    DrawRectangle(cx - gap, cy - thick / 2, gap * 2, thick, WHITE);
    // Vertical
    DrawRectangle(cx - thick / 2, cy - gap, thick, gap * 2, WHITE);
}

// ---------------------------------------------------------------------------
// Hotbar — 4 slots at the bottom, 1=Grass 2=Dirt 3=Stone 4=Wood
// ---------------------------------------------------------------------------
void DrawHotbar(int screenWidth, int screenHeight, int selected) {
    constexpr int SLOT_SIZE = 48;
    constexpr int GAP       = 4;
    constexpr int NUM_SLOTS = 9;

    // The 9 block types in fixed order (slots 6–9 are empty for now)
    const BlockType kSlots[NUM_SLOTS] = {
        BlockType::Grass,
        BlockType::Dirt,
        BlockType::Stone,
        BlockType::Wood,
        BlockType::Leaves,
        BlockType::Air,
        BlockType::Air,
        BlockType::Air,
        BlockType::Air
    };

    int totalW = NUM_SLOTS * SLOT_SIZE + (NUM_SLOTS - 1) * GAP;
    int startX = (screenWidth  - totalW) / 2;
    int startY = screenHeight - SLOT_SIZE - 12;

    for (int i = 0; i < NUM_SLOTS; ++i) {
        int x = startX + i * (SLOT_SIZE + GAP);
        int y = startY;

        // Slot background (block color, darkened slightly for unselected)
        Color fill = GetBlockColor(kSlots[i]);
        DrawRectangle(x, y, SLOT_SIZE, SLOT_SIZE, fill);

        // Selection highlight
        if (i == selected) {
            DrawRectangleLines(x,     y,     SLOT_SIZE, SLOT_SIZE, WHITE);
            DrawRectangleLines(x + 1, y + 1, SLOT_SIZE - 2, SLOT_SIZE - 2, WHITE);
        } else {
            DrawRectangleLines(x, y, SLOT_SIZE, SLOT_SIZE, DARKGRAY);
        }

        // Key number
        const char* label = nullptr;
        switch (i) {
            case 0: label = "1"; break;
            case 1: label = "2"; break;
            case 2: label = "3"; break;
            case 3: label = "4"; break;
            case 4: label = "5"; break;
            case 5: label = "6"; break;
            case 6: label = "7"; break;
            case 7: label = "8"; break;
            case 8: label = "9"; break;
        }
        int textW = MeasureText(label, 20);
        DrawText(label, x + SLOT_SIZE - textW - 4, y + SLOT_SIZE - 22, 20, WHITE);
    }
}

// ---------------------------------------------------------------------------
// Held block name — top-right corner
// ---------------------------------------------------------------------------
void DrawHeldBlockName(int screenWidth, int /*screenHeight*/, const char* name) {
    constexpr int FONT_SIZE = 22;
    constexpr int PADDING_X = 16;
    constexpr int PADDING_Y = 10;

    int textW = MeasureText(name, FONT_SIZE);
    int bgW   = textW + PADDING_X * 2;
    int bgH   = FONT_SIZE + PADDING_Y * 2;
    int bgX   = screenWidth - bgW - 12;
    int bgY   = 10;

    // Semi-transparent dark backdrop
    DrawRectangle(bgX, bgY, bgW, bgH, Color{ 0, 0, 0, 160 });
    DrawRectangleLines(bgX, bgY, bgW, bgH, Color{ 255, 255, 255, 60 });

    // Block name text
    DrawText(name, bgX + PADDING_X, bgY + PADDING_Y, FONT_SIZE, WHITE);
}

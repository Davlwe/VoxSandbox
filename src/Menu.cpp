#include "Menu.h"

// Helper: draw a centered text button. Returns true if clicked this frame.
static bool DrawButton(const char* label, int y, int screenWidth,
                       int btnW, int btnH, int fontSize)
{
    int x = (screenWidth - btnW) / 2;

    Rectangle rect = { (float)x, (float)y, (float)btnW, (float)btnH };
    Vector2 mouse  = GetMousePosition();
    bool hovered    = CheckCollisionPointRec(mouse, rect);

    // Button fill
    Color fill = hovered ? Color{ 60, 60, 60, 255 } : Color{ 40, 40, 40, 255 };
    DrawRectangleRec(rect, fill);

    // Border
    Color border = hovered ? LIGHTGRAY : DARKGRAY;
    DrawRectangleLinesEx(rect, 2.0f, border);

    // Label
    int textW = MeasureText(label, fontSize);
    DrawText(label, x + (btnW - textW) / 2, y + (btnH - fontSize) / 2,
             fontSize, WHITE);

    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

// ---------------------------------------------------------------------------
MenuAction UpdateMenu(int screenWidth, int screenHeight) {
    // Background
    ClearBackground(Color{ 30, 30, 40, 255 });

    // Title
    const char* title = "VOX SANDBOX";
    int titleW = MeasureText(title, 60);
    DrawText(title, (screenWidth - titleW) / 2, 150, 60, RAYWHITE);

    // Subtitle
    const char* sub = "A Minecraft-style voxel world";
    int subW = MeasureText(sub, 20);
    DrawText(sub, (screenWidth - subW) / 2, 230, 20, LIGHTGRAY);

    // Buttons
    const int btnW = 220;
    const int btnH = 50;

    if (DrawButton("Play", 340, screenWidth, btnW, btnH, 28)) {
        return MenuAction::Play;
    }

    if (DrawButton("Quit", 410, screenWidth, btnW, btnH, 28)) {
        return MenuAction::Quit;
    }

    return MenuAction::None;
}

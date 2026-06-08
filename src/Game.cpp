#include "Game.h"
#include "Menu.h"
#include "VoxelRaycast.h"
#include "UI.h"
#include "raymath.h"

#ifdef PLATFORM_WEB
  #include <emscripten.h>
  #include <emscripten/html5.h>

  static EM_BOOL OnCanvasClick(int eventType, const EmscriptenMouseEvent* e, void*) {
      emscripten_request_pointerlock("#canvas", EM_TRUE);
      return EM_TRUE;
  }
#endif

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
Game::Game(int screenWidth, int screenHeight)
    : m_ScreenWidth(screenWidth)
    , m_ScreenHeight(screenHeight)
    , m_State(GameState::MainMenu)
    , m_World()
    , m_Player()
    , m_Hotbar{ BlockType::Grass, BlockType::Dirt,
                BlockType::Stone, BlockType::Wood }
    , m_SelectedSlot(0)
{
    m_World.Generate();
}

// ---------------------------------------------------------------------------
// Main loop
// ---------------------------------------------------------------------------
void Game::Run() {
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        switch (m_State) {

        case GameState::MainMenu: {
            BeginDrawing();
            MenuAction action = UpdateMenu(m_ScreenWidth, m_ScreenHeight);
            EndDrawing();

            if (action == MenuAction::Play) {
                m_State = GameState::Playing;
                DisableCursor();
                #ifdef PLATFORM_WEB
                  emscripten_set_mousedown_callback("#canvas", nullptr, EM_FALSE, OnCanvasClick);
                #endif
            } else if (action == MenuAction::Quit) {
                return;
            }
            break;
        }

        case GameState::Playing: {
            // ESC returns to menu
            if (IsKeyPressed(KEY_ESCAPE)) {
                m_State = GameState::MainMenu;
                EnableCursor();
                #ifdef PLATFORM_WEB
                  emscripten_set_mousedown_callback("#canvas", nullptr, EM_FALSE, nullptr);
                  emscripten_exit_pointerlock();
                #endif
                break;
            }

            UpdatePlaying(dt);
            DrawPlaying();
            break;
        }
        }
    }
}

// ---------------------------------------------------------------------------
// Playing-state update
// ---------------------------------------------------------------------------
void Game::UpdatePlaying(float dt) {
    m_Player.Update(m_World, dt);

    Camera3D camera = m_Player.GetCamera();

    // Hotbar selection
    if (IsKeyPressed(KEY_ONE))   m_SelectedSlot = 0;
    if (IsKeyPressed(KEY_TWO))   m_SelectedSlot = 1;
    if (IsKeyPressed(KEY_THREE)) m_SelectedSlot = 2;
    if (IsKeyPressed(KEY_FOUR))  m_SelectedSlot = 3;

    // Raycast for interaction
    Vector3 rayOrigin = camera.position;
    Vector3 rayDir    = Vector3Normalize(
                          Vector3Subtract(camera.target, camera.position));

    // Left click — destroy
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        VoxelRayResult hit = VoxelRaycast(m_World, rayOrigin, rayDir, 5.0f);
        if (hit.hit) {
            m_World.SetBlock(hit.blockX, hit.blockY, hit.blockZ, BlockType::Air);
        }
    }

    // Right click — place
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        VoxelRayResult hit = VoxelRaycast(m_World, rayOrigin, rayDir, 5.0f);
        if (hit.hit) {
            if (!m_Player.OverlapsBlock(hit.placeX, hit.placeY, hit.placeZ)) {
                m_World.SetBlock(hit.placeX, hit.placeY, hit.placeZ,
                                 m_Hotbar[m_SelectedSlot]);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Playing-state draw
// ---------------------------------------------------------------------------
void Game::DrawPlaying() {
    Camera3D camera = m_Player.GetCamera();

    BeginDrawing();
    ClearBackground(Color{ 135, 206, 235, 255 });

    BeginMode3D(camera);
        m_World.Draw();
    EndMode3D();

    DrawPlayingUI();
    EndDrawing();
}

void Game::DrawPlayingUI() {
    DrawCrosshair(m_ScreenWidth, m_ScreenHeight);
    DrawHotbar(m_ScreenWidth, m_ScreenHeight, m_SelectedSlot);
    DrawFPS(10, 10);
}

#pragma once

#include <cstdint>

// World dimensions (in blocks)
constexpr int WORLD_WIDTH  = 64;
constexpr int WORLD_HEIGHT = 16;
constexpr int WORLD_DEPTH  = 64;

// Block size in world units
constexpr float BLOCK_SIZE = 1.0f;

// Ground level — the topmost solid layer
// Blocks from y=0 up to y=GROUND_LEVEL are solid; above is air
constexpr int GROUND_LEVEL = 12;   // y = 0..11 is solid, y = 12+ is air

// Which layers are which material (counting from bottom)
constexpr int STONE_TOP    = 8;    // y = 0..7  → stone
constexpr int DIRT_TOP     = 11;   // y = 8..10 → dirt
                                   // y = 11    → grass (top layer)

// Block types — enum so you can add more later without breaking things
enum class BlockType : uint8_t {
    Air   = 0,
    Grass = 1,
    Dirt  = 2,
    Stone = 3,
    Wood  = 4
};

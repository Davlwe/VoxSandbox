#pragma once

#include "Constants.h"
#include "raylib.h"

// ---------------------------------------------------------------------------
// Block definition — add new blocks here and in the BLOCK_DEFS array (Block.cpp)
// ---------------------------------------------------------------------------
struct BlockDef {
    BlockType  type;
    const char* name;
    Color       fallbackColor;
    const char* textureFile;   // e.g. "assets/textures/grass.png" (or nullptr)
    bool        transparent = false;  // true → render in alpha-blended second pass
};

// The master list — defined in Block.cpp
extern const BlockDef BLOCK_DEFS[];
extern const int      BLOCK_DEF_COUNT;

// Call once at startup — generates procedural textures + tries loading files
void InitBlocks();

// Free texture resources
void UnloadBlocks();

// Look up a block's fallback color (always available)
Color GetBlockColor(BlockType type);

// Look up a block's textured cube model (meshes[0] is valid if textures loaded)
Model GetBlockModel(BlockType type);

// True if this block type has a loaded texture model
bool BlockHasModel(BlockType type);

// True if this block type should render in the transparent (alpha-blended) pass
bool IsBlockTransparent(BlockType type);

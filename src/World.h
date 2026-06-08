#pragma once

#include "Constants.h"
#include "Block.h"
#include "raylib.h"

class World {
public:
    World();

    // Fill the world with the default superflat terrain
    void Generate();

    // Draw only exposed (visible) blocks
    void Draw() const;

    // Get the block at (x, y, z). Returns Air if out of bounds.
    BlockType GetBlock(int x, int y, int z) const;

    // Set a block at (x, y, z). Silently ignores out-of-bounds.
    void SetBlock(int x, int y, int z, BlockType type);

private:
    // Flat 3D array
    BlockType m_Blocks[WORLD_WIDTH * WORLD_HEIGHT * WORLD_DEPTH];

    // Precomputed list of exposed-block indices (rebuilt on world changes)
    static constexpr int MAX_EXPOSED = 25000;
    int m_ExposedIndices[MAX_EXPOSED];
    int m_ExposedCount;

    // Re-scan the world and fill m_ExposedIndices with visible blocks
    void RebuildExposedList();

    // Convert 3D coords to flat index
    int Index(int x, int y, int z) const;

    // True if (x, y, z) is inside the world bounds
    bool InBounds(int x, int y, int z) const;

    // True if at least one neighbor is air → this face would be visible
    bool IsExposed(int x, int y, int z) const;
};

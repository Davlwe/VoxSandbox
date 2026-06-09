#include "World.h"
#include "rlgl.h"

World::World()
    : m_ExposedCount(0)
{
    // Block default constructor already fills with Air/state=0
}

void World::Generate() {
    for (int x = 0; x < WORLD_WIDTH; ++x) {
        for (int z = 0; z < WORLD_DEPTH; ++z) {
            for (int y = 0; y <= GROUND_LEVEL; ++y) {
                BlockType bt;
                if (y == GROUND_LEVEL) {
                    bt = BlockType::Grass;          // top layer
                } else if (y > STONE_TOP) {
                    bt = BlockType::Dirt;           // middle layers
                } else {
                    bt = BlockType::Stone;          // bottom layers
                }
                int idx = Index(x, y, z);
                m_Blocks[idx].type  = bt;
                m_Blocks[idx].state = 0;
            }
        }
    }
    RebuildExposedList();   // build the draw cache once
}

void World::Draw() const {
    // --- Pass 1: solid (opaque) blocks -----------------------------------
    for (int i = 0; i < m_ExposedCount; ++i) {
        int idx = m_ExposedIndices[i];

        Block block = m_Blocks[idx];

        // Skip transparent blocks — drawn in pass 2
        if (IsBlockTransparent(block.type)) continue;

        // Flat index → 3D coords
        int x = idx % WORLD_WIDTH;
        int y = (idx / WORLD_WIDTH) % WORLD_HEIGHT;
        int z = idx / (WORLD_WIDTH * WORLD_HEIGHT);

        Vector3 position = {
            (float)x - WORLD_WIDTH  * 0.5f + 0.5f,
            (float)y + 0.5f,
            (float)z - WORLD_DEPTH * 0.5f + 0.5f
        };

        if (BlockHasModel(block.type)) {
            Model model = GetBlockModel(block.type);
            DrawModel(model, position, 1.0f, WHITE);
        } else {
            Color color = GetBlockColor(block.type);
            DrawCube(position, BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, color);
        }
        DrawCubeWires(position, 1.001f, 1.001f, 1.001f, DARKGRAY);
    }

    // --- Pass 2: transparent blocks (alpha blended, no depth writes) -----
    BeginBlendMode(BLEND_ALPHA);
    rlDisableDepthMask();

    for (int i = 0; i < m_ExposedCount; ++i) {
        int idx = m_ExposedIndices[i];

        Block block = m_Blocks[idx];

        if (!IsBlockTransparent(block.type)) continue;

        int x = idx % WORLD_WIDTH;
        int y = (idx / WORLD_WIDTH) % WORLD_HEIGHT;
        int z = idx / (WORLD_WIDTH * WORLD_HEIGHT);

        Vector3 position = {
            (float)x - WORLD_WIDTH  * 0.5f + 0.5f,
            (float)y + 0.5f,
            (float)z - WORLD_DEPTH * 0.5f + 0.5f
        };

        if (BlockHasModel(block.type)) {
            Model model = GetBlockModel(block.type);
            DrawModel(model, position, 1.0f, WHITE);
        } else {
            Color color = GetBlockColor(block.type);
            DrawCube(position, BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, color);
        }
        // Wireframe for transparent blocks (lighter, blended)
        DrawCubeWires(position, 1.001f, 1.001f, 1.001f, Color{ 80, 80, 80, 180 });
    }

    rlEnableDepthMask();
    EndBlendMode();
}

BlockType World::GetBlock(int x, int y, int z) const {
    if (!InBounds(x, y, z)) return BlockType::Air;
    return m_Blocks[Index(x, y, z)].type;
}

Block World::GetRawBlock(int x, int y, int z) const {
    if (!InBounds(x, y, z)) return Block{};
    return m_Blocks[Index(x, y, z)];
}

void World::SetBlock(int x, int y, int z, BlockType type) {
    if (!InBounds(x, y, z)) return;
    int idx = Index(x, y, z);
    if (m_Blocks[idx].type == type) return;   // no change
    m_Blocks[idx].type  = type;
    m_Blocks[idx].state = 0;   // reset state for new block
    RebuildExposedList();
}

int World::Index(int x, int y, int z) const {
    return x + y * WORLD_WIDTH + z * WORLD_WIDTH * WORLD_HEIGHT;
}

bool World::InBounds(int x, int y, int z) const {
    return x >= 0 && x < WORLD_WIDTH &&
           y >= 0 && y < WORLD_HEIGHT &&
           z >= 0 && z < WORLD_DEPTH;
}

bool World::IsExposed(int x, int y, int z) const {
    return GetBlock(x + 1, y, z)     == BlockType::Air ||
           GetBlock(x - 1, y, z)     == BlockType::Air ||
           GetBlock(x, y + 1, z)     == BlockType::Air ||
           GetBlock(x, y - 1, z)     == BlockType::Air ||
           GetBlock(x, y, z + 1)     == BlockType::Air ||
           GetBlock(x, y, z - 1)     == BlockType::Air;
}

void World::RebuildExposedList() {
    m_ExposedCount = 0;

    for (int x = 0; x < WORLD_WIDTH; ++x) {
        for (int z = 0; z < WORLD_DEPTH; ++z) {
            for (int y = 0; y < WORLD_HEIGHT; ++y) {
                int idx = Index(x, y, z);
                if (m_Blocks[idx].type == BlockType::Air) continue;
                if (!IsExposed(x, y, z)) continue;

                m_ExposedIndices[m_ExposedCount++] = idx;
            }
        }
    }
}

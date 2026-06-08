#include "World.h"

World::World()
    : m_ExposedCount(0)
{
    for (int i = 0; i < WORLD_WIDTH * WORLD_HEIGHT * WORLD_DEPTH; ++i) {
        m_Blocks[i] = BlockType::Air;
    }
}

void World::Generate() {
    for (int x = 0; x < WORLD_WIDTH; ++x) {
        for (int z = 0; z < WORLD_DEPTH; ++z) {
            for (int y = 0; y <= GROUND_LEVEL; ++y) {
                BlockType block;
                if (y == GROUND_LEVEL) {
                    block = BlockType::Grass;          // top layer
                } else if (y > STONE_TOP) {
                    block = BlockType::Dirt;           // middle layers
                } else {
                    block = BlockType::Stone;          // bottom layers
                }
                m_Blocks[Index(x, y, z)] = block;      // direct write
            }
        }
    }
    RebuildExposedList();   // build the draw cache once
}

void World::Draw() const {
    for (int i = 0; i < m_ExposedCount; ++i) {
        int idx = m_ExposedIndices[i];

        // Flat index → 3D coords
        int x = idx % WORLD_WIDTH;
        int y = (idx / WORLD_WIDTH) % WORLD_HEIGHT;
        int z = idx / (WORLD_WIDTH * WORLD_HEIGHT);

        BlockType block = m_Blocks[idx];

        Vector3 position = {
            (float)x - WORLD_WIDTH  * 0.5f + 0.5f,
            (float)y + 0.5f,
            (float)z - WORLD_DEPTH * 0.5f + 0.5f
        };

        if (BlockHasModel(block)) {
            Model model = GetBlockModel(block);
            DrawModel(model, position, 1.0f, WHITE);
        } else {
            Color color = GetBlockColor(block);
            DrawCube(position, BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, color);
        }
        DrawCubeWires(position, 1.001f, 1.001f, 1.001f, DARKGRAY);
    }
}

BlockType World::GetBlock(int x, int y, int z) const {
    if (!InBounds(x, y, z)) return BlockType::Air;
    return m_Blocks[Index(x, y, z)];
}

void World::SetBlock(int x, int y, int z, BlockType type) {
    if (!InBounds(x, y, z)) return;
    int idx = Index(x, y, z);
    if (m_Blocks[idx] == type) return;   // no change
    m_Blocks[idx] = type;
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
                if (m_Blocks[idx] == BlockType::Air) continue;
                if (!IsExposed(x, y, z)) continue;

                m_ExposedIndices[m_ExposedCount++] = idx;
            }
        }
    }
}

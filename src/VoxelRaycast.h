#pragma once

#include "raylib.h"

class World;

// Result of a voxel raycast
struct VoxelRayResult {
    bool hit = false;

    // Which solid block was struck (block indices, 0..WORLD_SIZE-1)
    int blockX = 0, blockY = 0, blockZ = 0;

    // Where a new block would be placed (hit block + face normal)
    int placeX = 0, placeY = 0, placeZ = 0;

    // Which face was hit: (-1/0/1, -1/0/1, -1/0/1)
    int normalX = 0, normalY = 0, normalZ = 0;
};

// Cast a ray through the voxel world using a DDA traversal.
// `origin` and `direction` are in world-space.
// `maxDistance` is in blocks.
VoxelRayResult VoxelRaycast(const World& world,
                            Vector3 origin,
                            Vector3 direction,
                            float maxDistance);

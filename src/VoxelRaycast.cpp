#include "VoxelRaycast.h"
#include "World.h"
#include "Constants.h"
#include <cmath>
#include <algorithm>

VoxelRayResult VoxelRaycast(const World& world,
                            Vector3 origin,
                            Vector3 direction,
                            float maxDistance)
{
    VoxelRayResult result;

    // World-space → block-index offsets (same as World::Draw centering)
    const int offX = WORLD_WIDTH  / 2;
    const int offZ = WORLD_DEPTH  / 2;

    // Starting voxel (block indices)
    int curX = (int)floorf(origin.x) + offX;
    int curY = (int)floorf(origin.y);
    int curZ = (int)floorf(origin.z) + offZ;

    // Step direction for each axis (+1, -1, or 0)
    int stepX = (direction.x > 0.0f) ? 1 : (direction.x < 0.0f) ? -1 : 0;
    int stepY = (direction.y > 0.0f) ? 1 : (direction.y < 0.0f) ? -1 : 0;
    int stepZ = (direction.z > 0.0f) ? 1 : (direction.z < 0.0f) ? -1 : 0;

    // tMax  = distance along ray to next voxel boundary on each axis
    // tDelta = distance along ray to cross one full voxel on each axis
    float tMaxX, tMaxY, tMaxZ;
    float tDeltaX, tDeltaY, tDeltaZ;

    // --- X axis ---
    if (stepX != 0) {
        float boundaryX = (stepX > 0)
            ? (float)(curX - offX + 1)   // right edge of current voxel
            : (float)(curX - offX);       // left edge of current voxel
        tMaxX   = (boundaryX - origin.x) / direction.x;
        tDeltaX = fabsf(1.0f / direction.x);
    } else {
        tMaxX   = INFINITY;
        tDeltaX = 0.0f;
    }

    // --- Y axis ---
    if (stepY != 0) {
        float boundaryY = (stepY > 0)
            ? (float)(curY + 1)           // top edge of current voxel
            : (float)(curY);              // bottom edge of current voxel
        tMaxY   = (boundaryY - origin.y) / direction.y;
        tDeltaY = fabsf(1.0f / direction.y);
    } else {
        tMaxY   = INFINITY;
        tDeltaY = 0.0f;
    }

    // --- Z axis ---
    if (stepZ != 0) {
        float boundaryZ = (stepZ > 0)
            ? (float)(curZ - offZ + 1)    // far edge of current voxel
            : (float)(curZ - offZ);        // near edge of current voxel
        tMaxZ   = (boundaryZ - origin.z) / direction.z;
        tDeltaZ = fabsf(1.0f / direction.z);
    } else {
        tMaxZ   = INFINITY;
        tDeltaZ = 0.0f;
    }

    // Prevent zero-step errors when ray starts exactly on a block face
    if (tMaxX < 0.0001f) tMaxX = 0.0001f;
    if (tMaxY < 0.0001f) tMaxY = 0.0001f;
    if (tMaxZ < 0.0001f) tMaxZ = 0.0001f;

    // Face normal of the last crossed boundary
    int normX = 0, normY = 0, normZ = 0;

    float travelled = 0.0f;

    while (travelled < maxDistance) {

        // Advance the axis with the smallest tMax
        if (tMaxX < tMaxY && tMaxX < tMaxZ) {
            if (tMaxX > maxDistance) break;
            curX     += stepX;
            travelled = tMaxX;
            tMaxX    += tDeltaX;
            normX     = -stepX;   // normal points opposite to step
            normY     = 0;
            normZ     = 0;
        } else if (tMaxY < tMaxZ) {
            if (tMaxY > maxDistance) break;
            curY     += stepY;
            travelled = tMaxY;
            tMaxY    += tDeltaY;
            normX     = 0;
            normY     = -stepY;
            normZ     = 0;
        } else {
            if (tMaxZ > maxDistance) break;
            curZ     += stepZ;
            travelled = tMaxZ;
            tMaxZ    += tDeltaZ;
            normX     = 0;
            normY     = 0;
            normZ     = -stepZ;
        }

        // Stop if we left the world
        if (curX < 0 || curX >= WORLD_WIDTH  ||
            curY < 0 || curY >= WORLD_HEIGHT ||
            curZ < 0 || curZ >= WORLD_DEPTH) {
            break;
        }

        // Hit a solid block?
        if (world.GetBlock(curX, curY, curZ) != BlockType::Air) {
            result.hit     = true;
            result.blockX  = curX;
            result.blockY  = curY;
            result.blockZ  = curZ;
            result.normalX = normX;
            result.normalY = normY;
            result.normalZ = normZ;
            result.placeX  = curX + normX;
            result.placeY  = curY + normY;
            result.placeZ  = curZ + normZ;
            break;
        }
    }

    return result;
}

#include "Player.h"
#include <cmath>

Player::Player()
    : m_Position{0.5f, (float)GROUND_LEVEL + 1.0f, 0.5f}
    , m_Velocity{0.0f, 0.0f, 0.0f}
    , m_Yaw(0.0f)
    , m_Pitch(0.0f)
    , m_OnGround(false)
{
}

// ---------------------------------------------------------------------------
// Public
// ---------------------------------------------------------------------------

void Player::Update(const World& world, float deltaTime) {
    if (deltaTime > 0.1f) deltaTime = 0.1f;   // cap dt to avoid tunneling

    // 1 — Mouse look
    Vector2 mouseDelta = GetMouseDelta();
    m_Yaw   -= mouseDelta.x * MOUSE_SENS;
    m_Pitch -= mouseDelta.y * MOUSE_SENS;

    // Clamp pitch so you can't flip upside-down
    constexpr float MAX_PITCH = 1.5f;   // ~85°
    if (m_Pitch >  MAX_PITCH) m_Pitch =  MAX_PITCH;
    if (m_Pitch < -MAX_PITCH) m_Pitch = -MAX_PITCH;

    // 2 — Horizontal input (WASD)
    Vector3 wishDir = {0, 0, 0};
    if (IsKeyDown(KEY_W)) wishDir = Vector3Add(wishDir, Forward());
    if (IsKeyDown(KEY_S)) wishDir = Vector3Subtract(wishDir, Forward());
    if (IsKeyDown(KEY_D)) wishDir = Vector3Add(wishDir, Right());
    if (IsKeyDown(KEY_A)) wishDir = Vector3Subtract(wishDir, Right());

    if (Vector3LengthSqr(wishDir) > 0.0001f) {
        wishDir = Vector3Normalize(wishDir);
        m_Velocity.x = wishDir.x * MOVE_SPEED;
        m_Velocity.z = wishDir.z * MOVE_SPEED;
    } else {
        m_Velocity.x = 0;
        m_Velocity.z = 0;
    }

    // 3 — Gravity
    m_Velocity.y -= GRAVITY * deltaTime;
    if (m_Velocity.y < -MAX_FALL) m_Velocity.y = -MAX_FALL;

    // 4 — Jump
    if (IsKeyPressed(KEY_SPACE) && m_OnGround) {
        m_Velocity.y = JUMP_SPEED;
        m_OnGround = false;
    }

    // 5 — Resolve movement per axis (Y first for ground/ceiling)
    TryMove(world, 1, deltaTime);   // Y axis
    TryMove(world, 0, deltaTime);   // X axis
    TryMove(world, 2, deltaTime);   // Z axis

    // 6 — Resolve any remaining overlap (e.g. fell into a hole off-center)
    ResolvePenetration(world);

    // 7 — Ground check after all movement resolves
    m_OnGround = IsOnGround(world);
}

Camera3D Player::GetCamera() const {
    Camera3D cam = {0};

    // Camera sits at the player's eye level
    cam.position = Vector3{
        m_Position.x,
        m_Position.y + EYE_HEIGHT,
        m_Position.z
    };

    // Forward vector from yaw + pitch
    Vector3 forward = {
        cosf(m_Pitch) * sinf(m_Yaw),
        sinf(m_Pitch),
        cosf(m_Pitch) * cosf(m_Yaw)
    };

    cam.target   = Vector3Add(cam.position, forward);
    cam.up       = Vector3{0, 1, 0};
    cam.fovy     = 70.0f;
    cam.projection = CAMERA_PERSPECTIVE;

    return cam;
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

Vector3 Player::Forward() const {
    // Horizontal-only forward (for wasd movement — ignore pitch)
    return Vector3{ sinf(m_Yaw), 0.0f, cosf(m_Yaw) };
}

Vector3 Player::Right() const {
    return Vector3{ -cosf(m_Yaw), 0.0f, sinf(m_Yaw) };
}

bool Player::IsColliding(const World& world, const Vector3& pos) const {
    float halfW = RADIUS;

    // Convert world-space → block-index by adding the centering offset
    int offX = WORLD_WIDTH  / 2;
    int offZ = WORLD_DEPTH  / 2;

    int bx0 = (int)floorf(pos.x - halfW) + offX;
    int bx1 = (int)floorf(pos.x + halfW - 0.001f) + offX;
    int by0 = (int)floorf(pos.y);
    int by1 = (int)floorf(pos.y + HEIGHT - 0.001f);
    int bz0 = (int)floorf(pos.z - halfW) + offZ;
    int bz1 = (int)floorf(pos.z + halfW - 0.001f) + offZ;

    for (int bx = bx0; bx <= bx1; ++bx) {
        for (int by = by0; by <= by1; ++by) {
            for (int bz = bz0; bz <= bz1; ++bz) {
                if (world.GetBlock(bx, by, bz) != BlockType::Air) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Player::IsOnGround(const World& world) const {
    // Check the block layer directly beneath the player's feet
    int footY = (int)floorf(m_Position.y - 0.001f);
    int offX  = WORLD_WIDTH  / 2;
    int offZ  = WORLD_DEPTH  / 2;

    int x0 = (int)floorf(m_Position.x - RADIUS) + offX;
    int x1 = (int)floorf(m_Position.x + RADIUS - 0.001f) + offX;
    int z0 = (int)floorf(m_Position.z - RADIUS) + offZ;
    int z1 = (int)floorf(m_Position.z + RADIUS - 0.001f) + offZ;

    for (int x = x0; x <= x1; ++x) {
        for (int z = z0; z <= z1; ++z) {
            if (world.GetBlock(x, footY, z) != BlockType::Air) {
                return true;
            }
        }
    }
    return false;
}

bool Player::OverlapsBlock(int bx, int by, int bz) const {
    // Convert block index to world-space AABB
    int offX = WORLD_WIDTH  / 2;
    int offZ = WORLD_DEPTH  / 2;

    float blockMinX = (float)(bx - offX);
    float blockMaxX = blockMinX + 1.0f;
    float blockMinY = (float)by;
    float blockMaxY = blockMinY + 1.0f;
    float blockMinZ = (float)(bz - offZ);
    float blockMaxZ = blockMinZ + 1.0f;

    // Player AABB — only care about upper body (waist up).
    // Blocks at foot level are fine; the player can step onto them.
    float playerMinX = m_Position.x - RADIUS;
    float playerMaxX = m_Position.x + RADIUS;
    float playerMinY = m_Position.y + HEIGHT * 0.5f;   // waist height
    float playerMaxY = m_Position.y + HEIGHT;
    float playerMinZ = m_Position.z - RADIUS;
    float playerMaxZ = m_Position.z + RADIUS;

    return (playerMinX < blockMaxX && playerMaxX > blockMinX) &&
           (playerMinY < blockMaxY && playerMaxY > blockMinY) &&
           (playerMinZ < blockMaxZ && playerMaxZ > blockMinZ);
}

// Tiny helper — let us read/write a Vector3 component by index (0=x, 1=y, 2=z)
static float& VecAxis(Vector3& v, int axis) {
    switch (axis) {
        case 0:  return v.x;
        case 1:  return v.y;
        default: return v.z;
    }
}

void Player::TryMove(const World& world, int axis, float deltaTime) {
    float amount = VecAxis(m_Velocity, axis) * deltaTime;
    if (fabsf(amount) < 0.0001f) return;

    float sign      = (amount > 0) ? 1.0f : -1.0f;
    float remainder = fabsf(amount);

    // Step forward 0.1 blocks at a time, stop if we hit a solid block
    while (remainder > 0.0001f) {
        float step = (remainder < 0.1f) ? remainder : 0.1f;

        Vector3 testPos = m_Position;
        VecAxis(testPos, axis) += step * sign;

        if (IsColliding(world, testPos)) {
            VecAxis(m_Velocity, axis) = 0.0f;   // stop dead on this axis

            // Snap to the exact block surface so we don't hover
            if (axis == 1) {  // Y axis
                if (sign < 0) {
                    // Landing — snap feet to top of the block below
                    m_Position.y = ceilf(testPos.y);
                } else {
                    // Ceiling — snap head to bottom of the block above
                    m_Position.y = floorf(testPos.y + HEIGHT) - HEIGHT;
                }
            }
            return;
        }

        VecAxis(m_Position, axis) += step * sign;
        remainder -= step;
    }
}

void Player::ResolvePenetration(const World& world) {
    // If we're not colliding, nothing to do
    if (!IsColliding(world, m_Position)) return;

    // We're inside a block — search for the nearest free position
    // by nudging along X and Z axes
    const float step   = 0.05f;
    const float maxNudge = 0.5f;

    Vector3 bestPos = m_Position;
    float   bestDist = INFINITY;
    bool    found = false;

    // Try all combinations of ±X, ±Z nudges (4 directions + center)
    for (int sx = -1; sx <= 1; ++sx) {
        for (int sz = -1; sz <= 1; ++sz) {
            if (sx == 0 && sz == 0) continue;  // skip no-nudge

            for (float d = step; d <= maxNudge; d += step) {
                Vector3 test = m_Position;
                test.x += (float)sx * d;
                test.z += (float)sz * d;

                if (!IsColliding(world, test)) {
                    float dist = (float)(sx * sx + sz * sz) * d * d;
                    if (dist < bestDist) {
                        bestDist = dist;
                        bestPos  = test;
                        found    = true;
                    }
                    break;  // found a clear spot in this direction
                }
            }
        }
    }

    if (found) {
        m_Position = bestPos;
    }
}

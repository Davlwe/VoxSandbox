#pragma once

#include "raylib.h"
#include "raymath.h"
#include "World.h"

class Player {
public:
    Player();

    // Call every frame — reads input, applies physics, resolves collision
    void Update(const World& world, float deltaTime);

    // Build a Camera3D positioned at the player's eyes
    Camera3D GetCamera() const;

    // True if a block at the given index would overlap the player hitbox
    bool OverlapsBlock(int bx, int by, int bz) const;

    // Player hitbox dimensions (in blocks)
    static constexpr float WIDTH     = 0.6f;   // total width (X/Z extent)
    static constexpr float HEIGHT    = 1.8f;   // total height
    static constexpr float EYE_HEIGHT = 1.6f;  // eye offset from feet
    static constexpr float RADIUS    = WIDTH * 0.5f;

private:
    Vector3 m_Position;   // feet position (center of base)
    Vector3 m_Velocity;   // blocks per second
    float   m_Yaw;        // horizontal rotation (radians)
    float   m_Pitch;      // vertical rotation (radians)
    bool    m_OnGround;

    // Movement constants
    static constexpr float MOVE_SPEED = 8.0f;
    static constexpr float MOUSE_SENS = 0.003f;
    static constexpr float GRAVITY    = 25.0f;
    static constexpr float JUMP_SPEED = 9.0f;
    static constexpr float MAX_FALL   = 40.0f;

    // Return the forward vector (horizontal only — no pitch tilt for movement)
    Vector3 Forward() const;
    Vector3 Right() const;

    // True if a solid block overlaps the player hitbox placed at `pos`
    bool IsColliding(const World& world, const Vector3& pos) const;

    // True if any solid block sits directly beneath the player
    bool IsOnGround(const World& world) const;

    // Move along one axis, resolve collisions by sliding
    void TryMove(const World& world, int axis, float delta);

    // Push player out of any blocks they're currently overlapping
    void ResolvePenetration(const World& world);
};

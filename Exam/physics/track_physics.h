#pragma once

#include <algorithm>
#include <cmath>

#include <glm/glm.hpp>

#include "utils/state.h"

// Track height function in world Y for a given z position; defines an analytic profile that shapes the marble track for physics and any track mesh in the OpenGL rasterization pipeline
inline float trackHeight(float z)
{
    float s = -z; // Distance along the track, 0 at start line and increasing forward toward negative z

    if (s < 0.0f) {
        return 0.0f; // Flat region behind the start line where racing is not intended
    }

    // Segment 0..5: flat section to allow initial acceleration before ramps
    if (s < 5.0f) {
        return 0.0f;
    }
    // Segment 5..10: upward ramp from 0.0 to about 0.5 units in height
    else if (s < 10.0f) {
        float t = (s - 5.0f) / 5.0f;
        return 0.5f * t;
    }
    // Segment 10..15: high plateau where marbles roll on a constant elevated height
    else if (s < 15.0f) {
        return 0.5f;
    }
    // Segment 15..20: gentle descent from 0.5 down to 0.2 to vary potential energy smoothly
    else if (s < 20.0f) {
        float t = (s - 15.0f) / 5.0f;
        return 0.5f - 0.3f * t;     // 0.5 -> 0.2
    }
    // Segment 20..25: steeper descent from 0.2 down to -0.4 for higher speed section
    else if (s < 25.0f) {
        float t = (s - 20.0f) / 5.0f;
        return 0.2f - 0.6f * t;     // 0.2 -> -0.4
    }
    // Segment 25..30: climb back from -0.4 to 0.0 to slow marbles toward the finish area
    else if (s < 30.0f) {
        float t = (s - 25.0f) / 5.0f;
        return -0.4f + 0.4f * t;    // -0.4 -> 0.0
    }
    // Beyond designed track section: flat continuation at zero height
    else {
        return 0.0f;
    }
}

// Axis-aligned overlap test between marble center and rectangular obstacle in XZ plane, expanded by marble radius to check whether the marble intersects the obstacle footprint
inline bool isInsideObstacleXZ(const glm::vec3& p, const Obstacle& ob, float radius)
{
    float halfX = ob.size.x * 0.5f + radius; // Half-width in X plus marble radius for margin
    float halfZ = ob.size.y * 0.5f + radius; // Half-depth in Z plus marble radius for margin

    float dx = p.x - ob.center.x;           // Horizontal offset in X from obstacle center
    float dz = p.z - ob.center.z;           // Horizontal offset in Z from obstacle center

    return (std::abs(dx) <= halfX && std::abs(dz) <= halfZ);
}

// Collision response between a marble and an axis-aligned obstacle in XZ; computes minimal push direction and moves the marble out along X or Z while canceling velocity in that axis for a simple inelastic contact
inline void resolveObstacleCollisionXZ(Marble& m, const Obstacle& ob)
{
    float halfX = ob.size.x * 0.5f + m.radius; // Collision extent in X including marble radius
    float halfZ = ob.size.y * 0.5f + m.radius; // Collision extent in Z including marble radius

    float dx = m.position.x - ob.center.x;     // Signed offset in X from obstacle center
    float dz = m.position.z - ob.center.z;     // Signed offset in Z from obstacle center

    float penX = halfX - std::abs(dx);         // Penetration depth along X axis
    float penZ = halfZ - std::abs(dz);         // Penetration depth along Z axis

    if (penX < penZ) {
        // Push out along X axis using smallest correction and zero horizontal velocity in that axis to avoid sliding through the obstacle face
        float sx = (dx >= 0.0f) ? 1.0f : -1.0f;
        m.position.x = ob.center.x + sx * halfX;
        m.velocity.x = 0.0f;
    } else {
        // Push out along Z axis using smallest correction and zero horizontal velocity in that axis to stop penetration along depth
        float sz = (dz >= 0.0f) ? 1.0f : -1.0f;
        m.position.z = ob.center.z + sz * halfZ;
        m.velocity.z = 0.0f;
    }
}

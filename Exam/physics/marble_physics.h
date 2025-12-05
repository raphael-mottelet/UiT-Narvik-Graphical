#pragma once

#include <cmath>

#include <glm/glm.hpp>

#include "utils/state.h"

// Handles pairwise marble collision in 3D by resolving overlap and updating velocities with a 1D elastic collision model along the contact normal for a simple but convincing physics response
inline void resolveMarbleCollision(Marble& a, Marble& b)
{
    glm::vec3 delta   = b.position - a.position;               // Vector from marble a to marble b used for distance and collision normal
    float     dist2   = glm::dot(delta, delta);                // Squared distance between marble centers to avoid unnecessary square root
    float     minDist = a.radius + b.radius;                   // Minimum allowed distance between centers when marbles just touch

    if (dist2 <= 0.0001f) {
        return;                                                // Skip collision when marbles are almost at the exact same position to avoid unstable division
    }

    if (dist2 < minDist * minDist) {
        float     dist = std::sqrt(dist2);                     // Actual distance between marble centers
        glm::vec3 n    = delta / dist;                         // Normalized collision normal pointing from a to b

        float penetration = minDist - dist;                    // Overlap amount between marbles along the collision normal

        // Move marbles apart along the collision normal so centers are exactly minDist apart and interpenetration is removed before velocity correction
        a.position -= 0.5f * penetration * n;
        b.position += 0.5f * penetration * n;

        // Project current velocities onto the collision normal to get scalar normal components used in the 1D elastic collision formula
        float vaN = glm::dot(a.velocity, n);
        float vbN = glm::dot(b.velocity, n);

        float m1 = a.mass;                                     // Mass of marble a for collision response
        float m2 = b.mass;                                     // Mass of marble b for collision response

        // Compute new normal velocity components using conservation of momentum and kinetic energy for a perfectly elastic one-dimensional collision along the normal
        float newVaN = (vaN * (m1 - m2) + 2.0f * m2 * vbN) / (m1 + m2);
        float newVbN = (vbN * (m2 - m1) + 2.0f * m1 * vaN) / (m1 + m2);

        // Apply change in normal velocity to full velocity vectors so tangential motion is preserved and only motion along the collision normal is affected
        a.velocity += (newVaN - vaN) * n;
        b.velocity += (newVbN - vbN) * n;
    }
}

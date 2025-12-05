#pragma once

#include <cstddef>
#include <algorithm>
#include <cmath>

#include "utils/state.h"
#include "physics/track_physics.h"
#include "physics/marble_physics.h"

// Global physics parameters for gravity, horizontal friction and vertical bounce response
static constexpr float kGravity      = -9.81f;   // Constant acceleration along world Y axis
static constexpr float kFrictionXZ   =  1.5f;    // Base damping factor for horizontal motion in XZ
static constexpr float kRestitutionY =  0.2f;    // Coefficient for vertical bounce after ground impact

// Per-frame physics step for all marbles: integration, constraints, collisions and finish detection
inline void updatePhysics(AppState& state, float dt)
{
    if (dt <= 0.0f) {
        return;
    }

    // Winner index acts as a latch that stops the simulation once a marble finishes
    if (state.winnerIndex >= 0) {
        return;
    }

    Track& track = state.track;

    // Integrate motion of each marble with gravity, friction, ground collision and obstacles
    for (Marble& m : state.marbles) {
        // Gravity contribution on vertical velocity, modeled as constant acceleration in world Y
        m.velocity.y += kGravity * dt;

        // Horizontal friction in XZ plane modeled as exponential damping; larger marbles receive stronger damping to feel heavier and slower
        const float baseRadius   = 0.5f; // Reference radius used to scale friction strength
        float       radiusFactor = (baseRadius > 0.0f) ? (m.radius / baseRadius) : 1.0f;
        float       friction     = kFrictionXZ * radiusFactor;

        float frictionFactor = std::exp(-friction * dt);
        m.velocity.x *= frictionFactor;
        m.velocity.z *= frictionFactor;

        // Position integration using updated velocity for a simple semi-implicit Euler step
        m.position += m.velocity * dt;

        // Lateral boundary constraints along X inside track width with a small bounce to avoid sticking to edges
        float halfWidth = track.width * 0.5f - m.radius;
        if (m.position.x < -halfWidth) {
            m.position.x = -halfWidth;
            if (m.velocity.x < 0.0f) {
                m.velocity.x *= -0.3f;
            }
        }
        if (m.position.x > halfWidth) {
            m.position.x = halfWidth;
            if (m.velocity.x > 0.0f) {
                m.velocity.x *= -0.3f;
            }
        }

        // Longitudinal clamping along Z to keep marbles within a margin around start and finish lines
        float margin = 3.0f;
        float zMin   = std::min(track.startZ, track.finishZ) - margin;
        float zMax   = std::max(track.startZ, track.finishZ) + margin;
        if (m.position.z < zMin) m.position.z = zMin;
        if (m.position.z > zMax) m.position.z = zMax;

        // Vertical collision against analytic track profile so marbles rest on the surface and bounce with reduced energy
        float groundY = trackHeight(m.position.z);
        float minY    = groundY + m.radius;

        if (m.position.y < minY) {
            m.position.y = minY;
            if (m.velocity.y < 0.0f) {
                m.velocity.y *= -kRestitutionY;
            }
        }

        // Obstacle interaction in XZ plane; overlap check and response keep marbles outside obstacle volumes on the track
        for (const Obstacle& ob : track.obstacles) {
            if (isInsideObstacleXZ(m.position, ob, m.radius)) {
                resolveObstacleCollisionXZ(m, ob);
            }
        }
    }

    // Pairwise marble-marble collision resolution for basic contact dynamics between moving marbles
    const std::size_t count = state.marbles.size();
    for (std::size_t i = 0; i < count; ++i) {
        for (std::size_t j = i + 1; j < count; ++j) {
            resolveMarbleCollision(state.marbles[i], state.marbles[j]);
        }
    }

    // Finish line detection along Z axis to lock in the first marble that reaches or passes the end of the track
    if (state.winnerIndex < 0) {
        for (std::size_t i = 0; i < state.marbles.size(); ++i) {
            const Marble& m = state.marbles[i];

            // Handles both negative and positive track directions by comparing finishZ with startZ
            if (track.finishZ < track.startZ) {
                if (m.position.z <= track.finishZ) {
                    state.winnerIndex = static_cast<int>(i);
                    break;
                }
            } else {
                if (m.position.z >= track.finishZ) {
                    state.winnerIndex = static_cast<int>(i);
                    break;
                }
            }
        }
    }
}

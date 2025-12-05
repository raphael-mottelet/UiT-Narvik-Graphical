#pragma once

#include <glm/glm.hpp>

#include "utils/state.h"
#include "player/camera.h"
#include "physics/track_physics.h"

// Reset race state for current track and configure camera
inline void setupRaceForCurrentTrack(AppState& state, Camera& camera)
{
    state.marbles.clear();      // Remove all marbles from previous race
    state.time        = 0.0f;   // Reset simulation time
    state.winnerIndex = -1;     // Clear winner index so no winner is stored

    float startZ  = state.track.startZ;        // Read start line position along Z axis
    float baseY   = trackHeight(startZ);       // Sample track height at start line
    float startY  = 0.25f;                     // Vertical offset to place marbles above track

    camera.yaw      = -90.0f;  // Set camera horizontal angle behind marbles
    camera.pitch    = -20.0f;  // Tilt camera downward toward track
    camera.distance = 7.0f;    // Set camera distance from target point

    // Player marble
    {
        Marble m;                                      // Local instance for player marble
        m.position        = glm::vec3(0.0f, baseY + startY, startZ + 0.5f); // Place player marble in center lane
        m.velocity        = glm::vec3(0.0f);           // Start with zero initial velocity
        m.radius          = 0.25f;                     // Set player marble radius
        m.mass            = 1.0f;                      // Set player marble mass
        m.isPlayer        = true;                      // Mark marble as player controlled
        m.colorAmbient    = glm::vec3(0.05f, 0.05f, 0.15f); // Set ambient color for player marble
        m.colorDiffuse    = glm::vec3(0.2f, 0.3f, 0.9f);    // Set diffuse color for player marble
        m.colorSpecular   = glm::vec3(1.0f);                // Set specular color for player marble
        m.shininess       = 64.0f;                          // Set shininess for player marble
        state.marbles.push_back(m);                         // Add player marble to marble list
    }

    // AI marble 1
    {
        Marble m;                                      // Local instance for first AI marble
        m.position        = glm::vec3(-0.6f, baseY + startY, startZ + 0.5f); // Place AI marble in left lane
        m.velocity        = glm::vec3(0.0f);           // Start with zero initial velocity
        m.radius          = 0.24f;                     // Set AI marble radius
        m.mass            = 1.0f;                      // Set AI marble mass
        m.isPlayer        = false;                     // Mark marble as AI controlled
        m.colorAmbient    = glm::vec3(0.05f, 0.0f, 0.0f);  // Set ambient color for AI marble
        m.colorDiffuse    = glm::vec3(0.8f, 0.1f, 0.1f);   // Set diffuse color for AI marble
        m.colorSpecular   = glm::vec3(1.0f);               // Set specular color for AI marble
        m.shininess       = 32.0f;                         // Set shininess for AI marble
        state.marbles.push_back(m);                        // Add AI marble to marble list
    }

    // AI marble 2
    {
        Marble m;                                      // Local instance for second AI marble
        m.position        = glm::vec3(0.6f, baseY + startY, startZ + 0.5f); // Place AI marble in right lane
        m.velocity        = glm::vec3(0.0f);           // Start with zero initial velocity
        m.radius          = 0.26f;                     // Set AI marble radius
        m.mass            = 1.1f;                      // Set AI marble mass
        m.isPlayer        = false;                     // Mark marble as AI controlled
        m.colorAmbient    = glm::vec3(0.02f, 0.05f, 0.02f); // Set ambient color for AI marble
        m.colorDiffuse    = glm::vec3(0.1f, 0.8f, 0.1f);    // Set diffuse color for AI marble
        m.colorSpecular   = glm::vec3(1.0f);                // Set specular color for AI marble
        m.shininess       = 32.0f;                          // Set shininess for AI marble
        state.marbles.push_back(m);                         // Add AI marble to marble list
    }
}

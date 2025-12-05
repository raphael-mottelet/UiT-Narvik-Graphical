#pragma once
#include <glm/glm.hpp>
#include <vector>

// Represents a rectangular obstacle on the track
struct Obstacle {
    glm::vec3 center{0.0f, 0.0f, 0.0f}; // World-space center position of the obstacle
    glm::vec2 size{1.0f, 1.0f};         // Half size of the obstacle in X (width) and Z (depth)
};

// Represents the marble track
struct Track {
    float width   = 4.0f;    // Total track width along the X axis
    float startZ  = 0.0f;    // Z position of the start line
    float finishZ = -30.0f;  // Z position of the finish line, more negative means further away

    std::vector<Obstacle> obstacles; // Collection of obstacles placed on the track
};

// Represents a single marble
struct Marble {
    glm::vec3 position{0.0f}; // Current marble position in world space
    glm::vec3 velocity{0.0f}; // Current marble velocity in world space

    float radius = 0.25f;     // Marble radius
    float mass   = 1.0f;      // Marble mass

    glm::vec3 colorAmbient  {0.05f, 0.05f, 0.05f}; // Ambient material color
    glm::vec3 colorDiffuse  {0.8f,  0.8f,  0.8f};  // Diffuse material color
    glm::vec3 colorSpecular {1.0f,  1.0f,  1.0f};  // Specular material color
    float shininess = 32.0f;                       // Phong shininess factor

    bool isPlayer = false; // Flag indicating if marble belongs to the player
};

// Represents the global application state
struct AppState {
    std::vector<Marble> marbles; // All marbles in the scene
    Track track;                 // Track configuration data

    float time        = 0.0f;    // Elapsed simulation time in seconds
    int   winnerIndex = -1;      // Index of the winning marble, -1 when no winner is set
};

// Returns a pointer to the player marble, nullptr when no player marble exists
inline Marble* getPlayerMarble(AppState& state) {
    for (auto& m : state.marbles) {
        if (m.isPlayer) return &m;
    }
    return nullptr;
}

// Returns a const pointer to the player marble, nullptr when no player marble exists
inline const Marble* getPlayerMarble(const AppState& state) {
    for (const auto& m : state.marbles) {
        if (m.isPlayer) return &m;
    }
    return nullptr;
}

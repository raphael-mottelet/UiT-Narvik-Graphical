#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

// Simple orbit camera used to follow the marble
struct Camera {
    glm::vec3 pos{0.0f, 2.0f, 6.0f};  // Camera position in world space

    float yaw   = -90.0f;             // Horizontal orbit angle in degrees
    float pitch = -15.0f;             // Vertical orbit angle in degrees

    float distance = 6.0f;            // Orbit distance from target marble
    float speed    = 5.0f;            // Base movement speed value

    // Forward direction vector based on yaw and pitch angles
    inline glm::vec3 getForward() const {
        float yawRad   = glm::radians(yaw);
        float pitchRad = glm::radians(pitch);

        glm::vec3 front;
        front.x = std::cos(yawRad) * std::cos(pitchRad);
        front.y = std::sin(pitchRad);
        front.z = std::sin(yawRad) * std::cos(pitchRad);
        front   = glm::normalize(front);

        return front;
    }

    // View matrix built from position and forward direction
    glm::mat4 view() const {
        glm::vec3 front = getForward();
        return glm::lookAt(pos, pos + front, glm::vec3(0.0f, 1.0f, 0.0f));
    }
};

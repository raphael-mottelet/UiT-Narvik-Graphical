#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {
    glm::vec3 pos  = glm::vec3(0.0f, 0.0f, 5.0f);
    float yaw      = -90.0f;          // facing -Z by default
    float speed    = 2.5f;            // units per second

    glm::mat4 view() const {
        // forward from yaw in XZ plane
        float rad = glm::radians(yaw);
        glm::vec3 forward = glm::normalize(glm::vec3(std::cos(rad), 0.0f, std::sin(rad)));
        return glm::lookAt(pos, pos + forward, glm::vec3(0,1,0));
    }
};

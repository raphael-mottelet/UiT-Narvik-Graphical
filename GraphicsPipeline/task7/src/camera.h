#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {
    glm::vec3 pos  = glm::vec3(0.0f, 1.0f, 5.0f);
    float yaw      = -90.0f;          // facing -Z
    float speed    = 3.0f;

    glm::mat4 view() const {
        float rad = glm::radians(yaw);
        glm::vec3 forward = glm::normalize(glm::vec3(std::cos(rad), 0.0f, std::sin(rad)));
        return glm::lookAt(pos, pos + forward, glm::vec3(0,1,0));
    }
};

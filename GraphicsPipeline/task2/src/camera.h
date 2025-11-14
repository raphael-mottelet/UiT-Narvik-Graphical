#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {
    glm::vec3 pos{0.0f, 0.0f, 3.0f};
    float yaw   = -90.0f; // looking toward -Z
    float pitch =   0.0f;
    float speed =   3.0f;

    glm::mat4 view() const {
        glm::vec3 front{
            std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch)),
            std::sin(glm::radians(pitch)),
            std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch))
        };
        return glm::lookAt(pos, pos + glm::normalize(front), {0.0f, 1.0f, 0.0f});
    }
};

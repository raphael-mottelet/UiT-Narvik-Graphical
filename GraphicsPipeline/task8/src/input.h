#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "camera.h"
#include "state.h"

inline void processInput(GLFWwindow* window, Camera& camera, AppState&, float dt){
    const glm::vec3 worldUp{0.0f, 1.0f, 0.0f};

    float yawRad = glm::radians(camera.yaw);
    glm::vec3 forward{ std::cos(yawRad), 0.0f, std::sin(yawRad) };
    forward = glm::normalize(forward);
    glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));

    const float v = camera.speed * dt;

    if(glfwGetKey(window, GLFW_KEY_UP)    == GLFW_PRESS) camera.pos -= worldUp * v;
    if(glfwGetKey(window, GLFW_KEY_DOWN)  == GLFW_PRESS) camera.pos += worldUp * v;
    if(glfwGetKey(window, GLFW_KEY_LEFT)  == GLFW_PRESS) camera.pos += right * v;
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) camera.pos -= right * v;

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.pos += forward * v;
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.pos -= forward * v;

    const float yawSpeed = 60.0f;
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.yaw -= yawSpeed * dt;
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.yaw += yawSpeed * dt;

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

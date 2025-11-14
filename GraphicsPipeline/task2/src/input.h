#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "camera.h"
#include "state.h"

inline void processInput(GLFWwindow* window, Camera& camera, AppState& state, float dt){
    const glm::vec3 worldUp{0.0f, 1.0f, 0.0f};

    // Forward vector in XZ from yaw (manual radians to avoid extra includes)
    const float PI = 3.14159265359f;
    float yawRad = camera.yaw * (PI/180.0f);
    glm::vec3 forward{ std::cos(yawRad), 0.0f, std::sin(yawRad) };
    forward = glm::normalize(forward);
    glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));

    const float v = camera.speed * dt;

    // Arrow keys: screen-direction feel
    if(glfwGetKey(window, GLFW_KEY_UP)    == GLFW_PRESS) camera.pos -= worldUp * v;
    if(glfwGetKey(window, GLFW_KEY_DOWN)  == GLFW_PRESS) camera.pos += worldUp * v;
    if(glfwGetKey(window, GLFW_KEY_LEFT)  == GLFW_PRESS) camera.pos += right * v;
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) camera.pos -= right * v;

    // W/S: conventional forward/back
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.pos += forward * v;
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.pos -= forward * v;

    // Rotations around Y
    const float rotSpeed = 90.0f; // deg/sec
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) state.boxAngleY -= rotSpeed * dt;
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) state.boxAngleY += rotSpeed * dt;
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) state.pyrAngleY -= rotSpeed * dt;
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) state.pyrAngleY += rotSpeed * dt;

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

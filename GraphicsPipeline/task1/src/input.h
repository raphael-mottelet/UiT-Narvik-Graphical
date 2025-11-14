#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "camera.h"
#include "state.h"

inline void processInput(GLFWwindow* window, Camera& camera, AppState& state, float dt){
    // world "up" axis (Y points up)
    const glm::vec3 worldUp{0.0f, 1.0f, 0.0f};

    // forward direction in the XZ plane (from yaw only, keep Y flat)
    glm::vec3 forward{
        std::cos(glm::radians(camera.yaw)),
        0.0f,
        std::sin(glm::radians(camera.yaw))
    };
    forward = glm::normalize(forward); // keep it unit length

    // right direction (right-handed: forward x up)
    glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));

    // how far we move this frame
    const float v = camera.speed * dt;

    // arrow keys mapped so the OBJECT seems to move with the arrows
    // up arrow   -> want object go up   -> move camera down
    if(glfwGetKey(window, GLFW_KEY_UP)    == GLFW_PRESS) camera.pos -= worldUp * v;
    // down arrow -> want object go down -> move camera up
    if(glfwGetKey(window, GLFW_KEY_DOWN)  == GLFW_PRESS) camera.pos += worldUp * v;

    // left arrow  -> want object go left  -> move camera right
    if(glfwGetKey(window, GLFW_KEY_LEFT)  == GLFW_PRESS) camera.pos += right * v;
    // right arrow -> want object go right -> move camera left
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) camera.pos -= right * v;

    // classic WASD forward/back (camera-centric)
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.pos += forward * v; // forward
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.pos -= forward * v; // back

    // rotate the red triangle around Y (Q/E)
    const float rotSpeed = 90.0f; // degrees per second
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) state.triAngleY -= rotSpeed * dt; // turn left
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) state.triAngleY += rotSpeed * dt; // turn right

    // quick exit
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
}

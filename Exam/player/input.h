#pragma once
#include <cmath>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/common.hpp>

#include "../player/camera.h"
#include "../utils/state.h"
#include "physics/physics.h"
#include "../player/marble_sizes.h"

inline void processInput(GLFWwindow* window, Camera& camera, AppState& state, float dt){
    Marble* player = getPlayerMarble(state);
    if(!player){
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        return;
    }

    // Camera-relative acceleration values for horizontal movement
    const float accelForward = 10.0f;
    const float accelSide    = 10.0f;

    float yawRad = camera.yaw * 3.14159265f / 180.0f;

    // Forward direction on XZ plane based on camera yaw
    glm::vec3 forwardDir(std::cos(yawRad), 0.0f, std::sin(yawRad));
    if(forwardDir.x != 0.0f || forwardDir.z != 0.0f){
        forwardDir = glm::normalize(forwardDir);
    }

    // Right direction on XZ plane perpendicular to forward
    glm::vec3 rightDir = glm::normalize(glm::cross(forwardDir, glm::vec3(0.0f, 1.0f, 0.0f)));

    glm::vec3 accel(0.0f);

    // WASD movement applied in camera space
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        accel += forwardDir * accelForward;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        accel -= forwardDir * accelForward;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        accel += rightDir * accelSide;
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        accel -= rightDir * accelSide;
    }

    // Apply horizontal acceleration only on XZ components
    player->velocity.x += accel.x * dt;
    player->velocity.z += accel.z * dt;

    // Jump when marble is on track surface and space bar is pressed
    float groundY = trackHeight(player->position.z);
    bool onGround = (player->position.y <= groundY + player->radius + 0.02f);
    if(onGround && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        player->velocity.y = 4.0f;
    }

    // Change player marble size with keys 1, 2, 3
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        setPlayerMarbleSize(state, MarbleSizeKind::Small);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        setPlayerMarbleSize(state, MarbleSizeKind::Medium);
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        setPlayerMarbleSize(state, MarbleSizeKind::Large);
    }

    // Orbit camera around marble using mouse input for yaw and pitch
    {
        static bool   firstMouse = true;
        static double lastX = 0.0;
        static double lastY = 0.0;

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        if(firstMouse){
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float dx = static_cast<float>(xpos - lastX);
        float dy = static_cast<float>(ypos - lastY);
        lastX = xpos;
        lastY = ypos;

        float sens = 0.12f;
        camera.yaw   += dx * sens;
        camera.pitch -= dy * sens;

        camera.pitch = glm::clamp(camera.pitch, -60.0f, 60.0f);
    }

    // Zoom camera in and out with numpad plus and minus
    {
        const float zoomSpeed = 10.0f;
        if(glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS){
            camera.distance -= zoomSpeed * dt;
        }
        if(glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS){
            camera.distance += zoomSpeed * dt;
        }
        camera.distance = glm::clamp(camera.distance, 2.0f, 20.0f);
    }

    // Close window when escape key is pressed
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

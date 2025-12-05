#pragma once

#include <GLFW/glfw3.h>

#include "menu_state.h"

// Keyboard controls for the menu:
//  M           : toggle menu on/off
//  UP / W      : move selection up
//  DOWN / S    : move selection down
//  LEFT / A    : smaller marble
//  RIGHT / D   : larger marble
//  ENTER       : confirm current selection
inline void updateMenuInput(GLFWwindow* window, MenuState& menu) {
    // Toggle menu with M (edge detection)
    static bool prevM = false;
    bool mNow = (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS);
    if (mNow && !prevM) {
        menu.visible = !menu.visible;
    }
    prevM = mNow;

    // Clear selection flag each frame
    menu.justSelected = false;

    if (!menu.visible || menu.maps.empty()) {
        return;
    }

    // Edge detection for navigation keys
    static bool prevUp    = false;
    static bool prevDown  = false;
    static bool prevLeft  = false;
    static bool prevRight = false;
    static bool prevEnter = false;

    bool upNow = (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) ||
                 (glfwGetKey(window, GLFW_KEY_W)  == GLFW_PRESS);
    bool downNow = (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) ||
                   (glfwGetKey(window, GLFW_KEY_S)    == GLFW_PRESS);
    bool leftNow = (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) ||
                   (glfwGetKey(window, GLFW_KEY_A)    == GLFW_PRESS);
    bool rightNow = (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) ||
                    (glfwGetKey(window, GLFW_KEY_D)     == GLFW_PRESS);
    bool enterNow = (glfwGetKey(window, GLFW_KEY_ENTER)    == GLFW_PRESS) ||
                    (glfwGetKey(window, GLFW_KEY_KP_ENTER) == GLFW_PRESS);

    int count = static_cast<int>(menu.maps.size());

    if (upNow && !prevUp && count > 0) {
        menu.selectedIndex = (menu.selectedIndex - 1 + count) % count;
    }
    if (downNow && !prevDown && count > 0) {
        menu.selectedIndex = (menu.selectedIndex + 1) % count;
    }

    // Change the marble size when LEFT/RIGHT is pressed once
    if (leftNow && !prevLeft) {
        int idx = static_cast<int>(menu.marbleSize);
        idx = (idx - 1 + 3) % 3; // wrap 0..2
        menu.marbleSize = static_cast<MarbleSize>(idx);
    }
    if (rightNow && !prevRight) {
        int idx = static_cast<int>(menu.marbleSize);
        idx = (idx + 1) % 3;
        menu.marbleSize = static_cast<MarbleSize>(idx);
    }

    if (enterNow && !prevEnter && count > 0) {
        menu.justSelected = true;
    }

    prevUp    = upNow;
    prevDown  = downNow;
    prevLeft  = leftNow;
    prevRight = rightNow;
    prevEnter = enterNow;
}

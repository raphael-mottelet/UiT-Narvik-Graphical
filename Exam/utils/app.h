#pragma once

// Forward declaration so we do not need to include GLFW here
struct GLFWwindow;

// Runs the full game using an already created window and OpenGL context.
// Returns true on normal exit.
bool runGame(GLFWwindow* window);

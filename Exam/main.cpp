#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "utils/app.h"

// GLFW error callback
static void glfwErrorCallback(int code, const char* desc) {
    std::cerr << "[GLFW ERROR] (" << code << "): " << desc << "\n";
}

int main() {
    std::cout << "Marble Run prototype (track + lighting + sky + menu)\n";

    // Set error callback and init GLFW
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }

    // OpenGL context hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    GLFWwindow* win = glfwCreateWindow(1280, 720, "Marble Run", nullptr, nullptr);
    if (!win) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return 1;
    }

    // Make context current and enable vsync
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);

    // Load OpenGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwDestroyWindow(win);
        glfwTerminate();
        return 1;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << "\n";

    // Run the game loop with this window
    bool ok = runGame(win);

    glfwDestroyWindow(win);
    glfwTerminate();
    return ok ? 0 : 1;
}

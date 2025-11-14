#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "camera.h"
#include "shader_utils.h"
#include "mesh.h"
#include "input.h"
#include "state.h"
#include "shaders_embedded.h"

// Select build via definitions from CMakeLists
#if defined(DRAW_BOTH)
  #define WANT_BOX 1
  #define WANT_PYR 1
#elif defined(DRAW_BOX)
  #define WANT_BOX 1
  #define WANT_PYR 0
#elif defined(DRAW_PYR)
  #define WANT_BOX 0
  #define WANT_PYR 1
#else
  #define WANT_BOX 1
  #define WANT_PYR 1
#endif

static void glfwErrorCallback(int code, const char* desc){
    std::cerr << "[GLFW ERROR " << code << "] " << desc << "\n";
}

int main(){
    std::cout <<
      "Task 2 — Geometry\n"
      "W/S forward-back, arrows screen-direction, A/D rotate BOX, Q/E rotate PYRAMID, Esc quit\n";

    glfwSetErrorCallback(glfwErrorCallback);
    if(!glfwInit()){ std::cerr << "GLFW init failed\n"; return -1; }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* win = glfwCreateWindow(1280, 720, "Task 2 — Geometry", nullptr, nullptr);
    if(!win){ std::cerr << "Window creation failed\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "GLAD load failed\n"; glfwTerminate(); return -1;
    }

    GLuint prog = makeProgram(kVertexSrc, kFragmentSrc);
    GLint uMVP   = glGetUniformLocation(prog, "uMVP");
    GLint uColor = glGetUniformLocation(prog, "uColor");

#if WANT_BOX
    Mesh box = makeBox();
#endif
#if WANT_PYR
    Mesh pyr = makePyramid();
#endif

    glEnable(GL_DEPTH_TEST);

    Camera camera;
    AppState state;
    float last = (float)glfwGetTime();

    while(!glfwWindowShouldClose(win)){
        float now = (float)glfwGetTime();
        float dt  = now - last; last = now;

        processInput(win, camera, state, dt);

        int fbw=1280, fbh=720;
        glfwGetFramebufferSize(win, &fbw, &fbh);
        glViewport(0, 0, fbw, fbh);

        glm::mat4 proj = glm::perspective(glm::radians(60.0f),
                           fbw>0 ? (float)fbw/(float)fbh : 1.7777f, 0.1f, 100.0f);
        glm::mat4 view  = camera.view();

        glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(prog);

#if WANT_BOX
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.2f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(state.boxAngleY), glm::vec3(0,1,0));
            glm::mat4 mvp = proj * view * model;
            glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(mvp));
            glUniform3f(uColor, 0.1f, 0.6f, 1.0f);
            glBindVertexArray(box.vao);
            glDrawElements(GL_TRIANGLES, box.indexCount, GL_UNSIGNED_INT, 0);
        }
#endif

#if WANT_PYR
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(+1.2f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(state.pyrAngleY), glm::vec3(0,1,0));
            glm::mat4 mvp = proj * view * model;
            glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(mvp));
            glUniform3f(uColor, 1.0f, 0.6f, 0.0f);
            glBindVertexArray(pyr.vao);
            glDrawElements(GL_TRIANGLES, pyr.indexCount, GL_UNSIGNED_INT, 0);
        }
#endif

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

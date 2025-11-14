#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <stdexcept>

#include "camera.h"
#include "shader_utils.h"
#include "mesh.h"
#include "input.h"
#include "state.h"
#include "shaders_embedded.h"

// diagnostics: hook GLFW errors and print basic GL info
static void glfwErrorCallback(int code, const char* desc){
    std::cerr << "[GLFW ERROR " << code << "] " << desc << "\n";
}
static void printGLInfo(){
    const GLubyte* ver = glGetString(GL_VERSION);
    const GLubyte* ven = glGetString(GL_VENDOR);
    const GLubyte* ren = glGetString(GL_RENDERER);
    GLint maj=0, min=0; glGetIntegerv(GL_MAJOR_VERSION,&maj); glGetIntegerv(GL_MINOR_VERSION,&min);
    std::cout << "GL_VERSION   : " << (ver ? reinterpret_cast<const char*>(ver) : "(null)") << "\n";
    std::cout << "GL_VENDOR    : " << (ven ? reinterpret_cast<const char*>(ven) : "(null)") << "\n";
    std::cout << "GL_RENDERER  : " << (ren ? reinterpret_cast<const char*>(ren) : "(null)") << "\n";
    std::cout << "GL_MAJOR.MIN : " << maj << "." << min << "\n";
}

int main(){
    // quick help text for controls
    std::cout << "Controls:\n"
                 "  W/S  = forward/back (camera)\n"
                 "  Arrows = screen-direction (object appears to move with arrows)\n"
                 "           Up   -> object up (camera down)\n"
                 "           Down -> object down (camera up)\n"
                 "           Left -> object left (camera right)\n"
                 "           Right-> object right (camera left)\n"
                 "  Q/E  = rotate triangle about Y\n"
                 "  Esc  = quit\n";

    glfwSetErrorCallback(glfwErrorCallback);
    if(!glfwInit()){
        std::cerr << "GLFW init failed\n"; return -1;
    }

    // request OpenGL 3.3 core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // create window + context
    GLFWwindow* win = glfwCreateWindow(1280, 720, "Task 1 — Graphics Pipeline", nullptr, nullptr);
    if(!win){ std::cerr << "Window creation failed\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1); // vsync on

    // load GL functions
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "GLAD load failed\n"; glfwTerminate(); return -1;
    }
    printGLInfo();

    // build program from embedded shader sources
    GLuint prog = makeProgram(kVertexSrc, kFragmentSrc);
    GLint uMVP   = glGetUniformLocation(prog, "uMVP");
    GLint uColor = glGetUniformLocation(prog, "uColor");

    // create a simple triangle mesh
    Mesh tri = makeTriangle();
    glEnable(GL_DEPTH_TEST);

    // runtime state
    Camera camera;            // default at (0,0,3), yaw=-90°
    AppState state;           // holds triAngleY
    float last = (float)glfwGetTime();

    while(!glfwWindowShouldClose(win)){
        // frame timing
        float now = (float)glfwGetTime();
        float dt  = now - last; last = now;

        // input: move camera so the object seems to follow arrow keys
        processInput(win, camera, state, dt);

        // update viewport to current framebuffer size
        int fbw=1280, fbh=720;
        glfwGetFramebufferSize(win, &fbw, &fbh);
        glViewport(0, 0, fbw, fbh);

        // build transform matrices
        glm::mat4 proj = glm::perspective(glm::radians(60.0f),
                            fbw>0 ? (float)fbw/(float)fbh : 1.7777f, 0.1f, 100.0f);
        glm::mat4 view  = camera.view();
        glm::mat4 model = glm::rotate(glm::mat4(1.0f),
                            glm::radians(state.triAngleY), glm::vec3(0.f,1.f,0.f));
        glm::mat4 mvp = proj * view * model;

        // clear screen
        glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw the triangle
        glUseProgram(prog);
        glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform3f(uColor, 1.f, 0.f, 0.f); // red

        glBindVertexArray(tri.vao);
        glDrawElements(GL_TRIANGLES, tri.indexCount, GL_UNSIGNED_INT, 0);

        // present + process events
        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    // clean exit
    glfwTerminate();
    return 0;
}

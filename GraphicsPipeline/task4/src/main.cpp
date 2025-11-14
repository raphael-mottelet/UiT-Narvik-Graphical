#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "camera.h"
#include "shader_utils.h"
#include "shaders_embedded.h"
#include "mesh.h"
#include "input.h"
#include "state.h"

static void glfwErrorCallback(int code, const char* desc){
    std::cerr << "[GLFW ERROR " << code << "] " << desc << "\n";
}

int main(){
    std::cout <<
        "Task 4 — Shaders\n"
        "Controls (AZERTY): Arrows move on screen, Z/S forward/back (W/S in code),\n"
        "A/D rotate BOX (Y), Q/E rotate PYRAMID (Y), Esc = quit\n";

    glfwSetErrorCallback(glfwErrorCallback);
    if(!glfwInit()){ std::cerr << "GLFW init failed\n"; return -1; }

    // OpenGL 3.3 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* win = glfwCreateWindow(1280, 720, "Task 4 — Shaders", nullptr, nullptr);
    if(!win){ std::cerr << "Window creation failed\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "GLAD load failed\n"; glfwTerminate(); return -1;
    }

    // Make sure depth test is on and culling is off (so both sides render)
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Build programs: box & pyramid use different shader pairs
    GLuint progBox = makeProgram(kBoxVertexSrc, kBoxFragmentSrc);
    GLuint progPyr = makeProgram(kPyrVertexSrc, kPyrFragmentSrc);

    // Uniform locations
    GLint uMVP_box = glGetUniformLocation(progBox, "uMVP");
    GLint uMVP_pyr = glGetUniformLocation(progPyr, "uMVP");
    GLint uTime_pyr = glGetUniformLocation(progPyr, "uTime");

    // Geometry
    Mesh box = makeBox();                 // positions only (now 24-vertex, consistent winding)
    Mesh pyr = makePyramidWithFaceId();   // adds integer face attribute

    Camera camera;            // from camera.h (pos=(0,0,5), yaw=-90)
    AppState state;           // rotation angles
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

        // Draw BOX (flatten z in vertex, color by x/y in shader)
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.2f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(state.boxAngleY), glm::vec3(0,1,0));
            glm::mat4 mvp = proj * view * model;

            glUseProgram(progBox);
            glUniformMatrix4fv(uMVP_box, 1, GL_FALSE, glm::value_ptr(mvp));

            glBindVertexArray(box.vao);
            glDrawElements(GL_TRIANGLES, box.indexCount, GL_UNSIGNED_INT, 0);
        }

        // Draw PYRAMID (animated side colors with cosine over time)
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(+1.2f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(state.pyrAngleY), glm::vec3(0,1,0));
            glm::mat4 mvp = proj * view * model;

            glUseProgram(progPyr);
            glUniformMatrix4fv(uMVP_pyr, 1, GL_FALSE, glm::value_ptr(mvp));
            glUniform1f(uTime_pyr, now);

            glBindVertexArray(pyr.vao);
            glDrawElements(GL_TRIANGLES, pyr.indexCount, GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

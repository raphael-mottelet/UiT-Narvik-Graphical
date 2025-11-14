#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "camera.h"
#include "input.h"
#include "mesh.h"
#include "shader_utils.h"
#include "shaders_embedded.h"
#include "state.h"

static void glfwErrorCallback(int code, const char* desc){
    std::cerr << "[GLFW ERROR " << code << "] " << desc << "\n";
}

int main(){
    std::cout << "Task 7 — Lighting (Phong per-fragment, blue sphere + point light)\n";

    glfwSetErrorCallback(glfwErrorCallback);
    if(!glfwInit()){ std::cerr << "GLFW init failed\n"; return -1; }

    // OpenGL 3.3 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* win = glfwCreateWindow(1280, 720, "Task 7 — Lighting", nullptr, nullptr);
    if(!win){ std::cerr << "Window creation failed\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "GLAD load failed\n"; glfwTerminate(); return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Sphere mesh (unit radius), decent tesselation for smooth specular
    Mesh sphere = makeSphere(48, 24, 1.0f);

    // Shaders (Phong lighting per-fragment)
    GLuint prog = makeProgram(kLitVS, kLitFS);
    if(!prog){ std::cerr << "Shader link failed\n"; return -1; }

    // Uniform locations
    GLint uModel      = glGetUniformLocation(prog, "uModel");
    GLint uView       = glGetUniformLocation(prog, "uView");
    GLint uProj       = glGetUniformLocation(prog, "uProj");
    GLint uNormalMat  = glGetUniformLocation(prog, "uNormalMat");
    GLint uViewPos    = glGetUniformLocation(prog, "uViewPos");

    // Light uniforms
    GLint uLightPos   = glGetUniformLocation(prog, "light.position");
    GLint uLa         = glGetUniformLocation(prog, "light.ambient");
    GLint uLd         = glGetUniformLocation(prog, "light.diffuse");
    GLint uLs         = glGetUniformLocation(prog, "light.specular");

    // Material uniforms
    GLint uKa         = glGetUniformLocation(prog, "material.ambient");
    GLint uKd         = glGetUniformLocation(prog, "material.diffuse");
    GLint uKs         = glGetUniformLocation(prog, "material.specular");
    GLint uShin       = glGetUniformLocation(prog, "material.shininess");

    // Camera / state
    Camera camera;          // pos=(0,1,5), yaw=-90
    AppState state;
    float last = (float)glfwGetTime();

    // Fixed transforms
    glm::mat4 proj;
    glm::mat4 model = glm::mat4(1.0f); // sphere at origin
    // Pick a nice light position
    glm::vec3 lightPos = glm::vec3(2.0f, 2.0f, 2.0f);

    while(!glfwWindowShouldClose(win)){
        float now = (float)glfwGetTime();
        float dt  = now - last; last = now;

        processInput(win, camera, state, dt);

        int fbw=1280, fbh=720;
        glfwGetFramebufferSize(win, &fbw, &fbh);
        glViewport(0, 0, fbw, fbh);
        proj = glm::perspective(glm::radians(60.0f), fbw>0 ? (float)fbw/(float)fbh : 1.7777f, 0.1f, 100.0f);
        glm::mat4 view = camera.view();

        glClearColor(0.05f, 0.07f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Normal matrix from model (world space)
        glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(model)));

        glUseProgram(prog);
        glUniformMatrix4fv(uModel,     1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uView,      1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uProj,      1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix3fv(uNormalMat, 1, GL_FALSE, glm::value_ptr(normalMat));
        glUniform3fv(uViewPos, 1, glm::value_ptr(camera.pos));

        // Light (ambient/diffuse/specular)
        glUniform3f(uLightPos, lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(uLa, 0.10f, 0.10f, 0.12f);   // subtle cool ambient
        glUniform3f(uLd, 1.00f, 1.00f, 1.00f);   // white diffuse
        glUniform3f(uLs, 1.00f, 1.00f, 1.00f);   // white specular

        // Material: blue sphere
        glUniform3f(uKa, 0.02f, 0.04f, 0.10f);   // ambient (dark blue)
        glUniform3f(uKd, 0.20f, 0.40f, 1.00f);   // diffuse (blue)
        glUniform3f(uKs, 0.90f, 0.90f, 0.95f);   // specular (nearly white)
        glUniform1f(uShin, 32.0f);               // shininess

        glBindVertexArray(sphere.vao);
        glDrawElements(GL_TRIANGLES, sphere.indexCount, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

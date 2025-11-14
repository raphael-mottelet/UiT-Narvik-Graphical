#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <random>
#include <iostream>
#include <algorithm>

#include "camera.h"
#include "input.h"
#include "mesh.h"
#include "shader_utils.h"
#include "shaders_embedded.h"
#include "state.h"

static void glfwErrorCallback(int code, const char* desc){
    std::cerr << "[GLFW ERROR " << code << "] " << desc << "\n";
}

struct Particle {
    glm::vec3 pos;
    glm::vec3 vel;
    float     age;
    float     life;
    float     size;
};

int main(){
    std::cout <<
        "Task 8 — Particle system (fire from pyramid tip)\n"
        "W/S = forward/back, Arrows = strafe/up/down, A/D = yaw, Esc = quit\n";

    glfwSetErrorCallback(glfwErrorCallback);
    if(!glfwInit()){ std::cerr << "GLFW init failed\n"; return -1; }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* win = glfwCreateWindow(1280, 720, "Task 8 — Particle Fire", nullptr, nullptr);
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

    Mesh pyramid = makePyramid();

    GLuint progSolid = makeProgram(kMVP_VS, kSolid_FS);
    GLuint progPart  = makeProgram(kPart_VS,  kPart_FS);
    if(!progSolid || !progPart){ std::cerr << "Shader link failed\n"; return -1; }

    GLint uMVP_solid = glGetUniformLocation(progSolid, "uMVP");
    GLint uColor     = glGetUniformLocation(progSolid, "uColor");

    GLint uView_p  = glGetUniformLocation(progPart, "uView");
    GLint uProj_p  = glGetUniformLocation(progPart, "uProj");
    GLint uCamR    = glGetUniformLocation(progPart, "uCamRight");
    GLint uCamU    = glGetUniformLocation(progPart, "uCamUp");

    constexpr int NUM = 400;
    std::vector<Particle> particles(NUM);

    std::mt19937 rng(1337);
    std::uniform_real_distribution<float> U01(0.0f, 1.0f);
    auto randRange = [&](float a, float b){ return a + (b - a) * U01(rng); };

    const glm::vec3 spawnPos = glm::vec3(0.0f, 0.5f, 0.0f);

    auto respawn = [&](Particle& p){
        float r  = randRange(0.0f, 0.05f);
        float ang= randRange(0.0f, 6.2831853f);
        glm::vec3 jitter(r * std::cos(ang), 0.0f, r * std::sin(ang));
        p.pos  = spawnPos + jitter;
        glm::vec3 v0(randRange(-0.35f, 0.35f), randRange(1.5f, 3.0f), randRange(-0.35f, 0.35f));
        p.vel  = v0;
        p.age  = 0.0f;
        p.life = randRange(0.8f, 1.6f);
        p.size = randRange(0.06f, 0.12f);
    };
    for(auto& p : particles) respawn(p);

    GLuint vaoParticle=0, vboCorners=0, vboInstances=0;
    {
        glGenVertexArrays(1, &vaoParticle);
        glBindVertexArray(vaoParticle);

        const float corners[8] = {
            -0.5f, -0.5f,
             0.5f, -0.5f,
             0.5f,  0.5f,
            -0.5f,  0.5f
        };
        glGenBuffers(1, &vboCorners);
        glBindBuffer(GL_ARRAY_BUFFER, vboCorners);
        glBufferData(GL_ARRAY_BUFFER, sizeof(corners), corners, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);

        glGenBuffers(1, &vboInstances);
        glBindBuffer(GL_ARRAY_BUFFER, vboInstances);
        glBufferData(GL_ARRAY_BUFFER, NUM * (5*sizeof(float)), nullptr, GL_DYNAMIC_DRAW);

        const GLsizei stride = 5 * sizeof(float);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glVertexAttribDivisor(1, 1);

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)(3*sizeof(float)));
        glVertexAttribDivisor(2, 1);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)(4*sizeof(float)));
        glVertexAttribDivisor(3, 1);

        glBindVertexArray(0);
    }

    Camera camera; camera.pos = glm::vec3(0, 1.0f, 5.0f);
    AppState state{};
    float last = (float)glfwGetTime();

    glm::mat4 modelPyr = glm::mat4(1.0f);

    while(!glfwWindowShouldClose(win)){
        float now = (float)glfwGetTime();
        float dt  = std::min(now - last, 0.033f);
        last = now;

        processInput(win, camera, state, dt);

        int fbw=1280, fbh=720;
        glfwGetFramebufferSize(win, &fbw, &fbh);
        glViewport(0, 0, fbw, fbh);

        glm::mat4 proj = glm::perspective(glm::radians(60.0f),
                           fbw>0 ? (float)fbw/(float)fbh : 1.7777f, 0.1f, 200.0f);
        glm::mat4 view = camera.view();
        glm::mat4 mvp  = proj * view * modelPyr;

        glClearColor(0.05f, 0.06f, 0.09f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(progSolid);
        glUniformMatrix4fv(uMVP_solid, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform3f(uColor, 0.6f, 0.5f, 0.4f);
        glBindVertexArray(pyramid.vao);
        glDrawElements(GL_TRIANGLES, pyramid.indexCount, GL_UNSIGNED_INT, 0);

        const glm::vec3 accel(0.0f, 0.4f, 0.0f);
        const float damping = 1.2f;
        std::vector<float> instanceData; instanceData.reserve(NUM * 5);

        for(auto& p : particles){
            p.vel += accel * dt;
            p.vel *= std::exp(-damping * dt);
            p.pos += p.vel * dt;
            p.age += dt;

            if(p.age >= p.life){
                respawn(p);
            }

            float t = std::clamp(p.age / p.life, 0.0f, 1.0f);
            instanceData.push_back(p.pos.x);
            instanceData.push_back(p.pos.y);
            instanceData.push_back(p.pos.z);
            instanceData.push_back(t);
            instanceData.push_back(p.size);
        }

        glBindBuffer(GL_ARRAY_BUFFER, vboInstances);
        glBufferSubData(GL_ARRAY_BUFFER, 0, instanceData.size()*sizeof(float), instanceData.data());

        // billboard basis from camera yaw (fix: use up x forward for right)
        float yawRad = glm::radians(camera.yaw);
        glm::vec3 forward{ std::cos(yawRad), 0.0f, std::sin(yawRad) };
        forward = glm::normalize(forward);
        glm::vec3 worldUp{0,1,0};
        glm::vec3 camRight = glm::normalize(glm::cross(worldUp, forward));
        glm::vec3 camUp    = glm::normalize(glm::cross(forward, camRight));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);

        glUseProgram(progPart);
        glUniformMatrix4fv(uView_p, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uProj_p, 1, GL_FALSE, glm::value_ptr(proj));
        glUniform3fv(uCamR, 1, glm::value_ptr(camRight));
        glUniform3fv(uCamU, 1, glm::value_ptr(camUp));

        glBindVertexArray(vaoParticle);
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, NUM);

        glBindVertexArray(0);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

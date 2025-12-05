#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "utils/app.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "player/camera.h"
#include "player/input.h"
#include "utils/mesh.h"
#include "graphics/shader_utils.h"
#include "graphics/shaders_embedded.h"
#include "graphics/lighting.h"
#include "utils/state.h"
#include "physics/physics.h"

#include "map/track_loader.h"
#include "utils/race_setup.h"
#include "graphics/track_render.h"
#include "graphics/marble_render.h"
#include "graphics/sky_sun_render.h"
#include "utils/game_logic.h"

#include "graphics/text_renderer.h"

#include "menu/menu_state.h"
#include "menu/menu_input.h"
#include "menu/menu_render.h"

bool runGame(GLFWwindow* win) {
    if (!win) {
        std::cerr << "[runGame] window pointer is null\n";
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    GLuint prog        = makeProgram(kLitVS, kLitFS);
    GLuint skyProg     = makeProgram(kSkyVS, kSkyFS);
    GLuint overlayProg = makeProgram(kOverlayVS, kOverlayFS);

    Mesh groundBox = makeBox();
    Mesh sphere    = makeSphere(48, 24, 1.0f);
    Mesh skyCube   = makeBox();

    glUseProgram(prog);
    GLint uModel      = glGetUniformLocation(prog, "uModel");
    GLint uView       = glGetUniformLocation(prog, "uView");
    GLint uProj       = glGetUniformLocation(prog, "uProj");
    GLint uNormalMat  = glGetUniformLocation(prog, "uNormalMat");
    GLint uViewPos    = glGetUniformLocation(prog, "uViewPos");

    GLint uMatAmbient  = glGetUniformLocation(prog, "material.ambient");
    GLint uMatDiffuse  = glGetUniformLocation(prog, "material.diffuse");
    GLint uMatSpecular = glGetUniformLocation(prog, "material.specular");
    GLint uMatShine    = glGetUniformLocation(prog, "material.shininess");

    glUseProgram(skyProg);
    GLint uSkyModel = glGetUniformLocation(skyProg, "uModel");
    GLint uSkyView  = glGetUniformLocation(skyProg, "uView");
    GLint uSkyProj  = glGetUniformLocation(skyProg, "uProj");
    GLint uSkyColor = glGetUniformLocation(skyProg, "uSkyColor");
    glUniform3f(uSkyColor, 0.7f, 0.85f, 1.0f);

    GLuint overlayVAO = 0, overlayVBO = 0, overlayEBO = 0;
    glGenVertexArrays(1, &overlayVAO);
    glGenBuffers(1, &overlayVBO);
    glGenBuffers(1, &overlayEBO);

    float quadVerts[] = {
        // x,   y,   u,  v
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f
    };
    unsigned int quadIdx[] = { 0, 1, 2, 2, 3, 0 };

    glBindVertexArray(overlayVAO);
    glBindBuffer(GL_ARRAY_BUFFER, overlayVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, overlayEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIdx), quadIdx, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glUseProgram(overlayProg);
    GLint uOverlayColor = glGetUniformLocation(overlayProg, "uColor");

    int fbWidth = 0, fbHeight = 0;
    glfwGetFramebufferSize(win, &fbWidth, &fbHeight);

    TextRenderer textRenderer;
    if (!initTextRenderer(textRenderer, static_cast<unsigned int>(fbWidth))) {
        std::cerr << "Failed to initialize TextRenderer\n";
    }

    AppState  state;
    Camera    camera;
    MenuState menu;

    initMenu(menu);

    if (!tryLoadTrackFromFile("map/01-map.txt", state.track)) {
        std::cerr << "[runGame] Failed to load map/01-map.txt\n";
    }

    setupRaceForCurrentTrack(state, camera);
    SunLight sun = makeSunLightAboveTrack(state.track);

    bool   titleUpdatedOnce = false;
    double lastTime         = glfwGetTime();

    while (!glfwWindowShouldClose(win)) {
        glfwPollEvents();

        double now = glfwGetTime();
        float  dt  = static_cast<float>(now - lastTime);
        lastTime   = now;
        state.time += dt;

        int width, height;
        glfwGetFramebufferSize(win, &width, &height);
        float aspect = (height > 0) ? (float)width / (float)height : 16.0f / 9.0f;

        updateMenuInput(win, menu);

        if (menu.justSelected &&
            menu.selectedIndex >= 0 &&
            menu.selectedIndex < (int)menu.maps.size()) {

            const MapInfo& info = menu.maps[menu.selectedIndex];
            std::string path = info.path;

            if (!tryLoadTrackFromFile(path, state.track)) {
                std::cout << "[runGame] Could not load " << path
                          << ", falling back to map/01-map.txt\n";
                if (!tryLoadTrackFromFile("map/01-map.txt", state.track)) {
                    std::cerr << "[runGame] Fallback map/01-map.txt also failed\n";
                }
            }

            setupRaceForCurrentTrack(state, camera);
            sun = makeSunLightAboveTrack(state.track);

            menu.visible = false;
        }

        if (!menu.visible) {
            processInput(win, camera, state, dt);
            updatePhysics(state, dt);
        }

        updateEndOfRaceUI(win, state, titleUpdatedOnce);

        if (Marble* player = getPlayerMarble(state)) {
            float heightOffset = 1.0f;
            float dist         = camera.distance;

            glm::vec3 forward = camera.getForward();
            glm::vec3 backDir = -glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));

            camera.pos = player->position
                       + glm::vec3(0.0f, heightOffset, 0.0f)
                       + backDir * dist;
        }

        glm::mat4 view = camera.view();
        glm::mat4 proj = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 200.0f);

        glViewport(0, 0, width, height);
        glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderSkybox(skyCube, skyProg, uSkyModel, uSkyView, uSkyProj, view, proj);

        glUseProgram(prog);
        glUniformMatrix4fv(uView,    1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uProj,    1, GL_FALSE, glm::value_ptr(proj));
        glUniform3fv(uViewPos,       1, glm::value_ptr(camera.pos));

        uploadSunLight(prog, sun);

        Track& track = state.track;

        renderSun(sun, sphere, uModel, uNormalMat,
                  uMatAmbient, uMatDiffuse, uMatSpecular, uMatShine);

        glBindVertexArray(groundBox.vao);
        renderBaseGround(track, groundBox,
                         uModel, uNormalMat,
                         uMatAmbient, uMatDiffuse, uMatSpecular, uMatShine);

        renderTrackSurface(track, groundBox,
                           uModel, uNormalMat,
                           uMatAmbient, uMatDiffuse, uMatSpecular, uMatShine);

        renderTrackRails(track, groundBox,
                         uModel, uNormalMat,
                         uMatAmbient, uMatDiffuse, uMatSpecular, uMatShine);

        renderTrackObstacles(track, groundBox,
                             uModel, uNormalMat,
                             uMatAmbient, uMatDiffuse, uMatSpecular, uMatShine);

        renderStartFinishLines(state, groundBox,
                               uModel, uNormalMat,
                               uMatAmbient, uMatDiffuse, uMatSpecular, uMatShine);

        renderMarbles(state, sphere,
                      uModel, uNormalMat,
                      uMatAmbient, uMatDiffuse, uMatSpecular, uMatShine);

        renderVictoryOverlay(state, overlayProg, overlayVAO, uOverlayColor);

        if (menu.visible) {
            renderMenuOverlay(
                menu,
                textRenderer,
                overlayProg,
                overlayVAO,
                uOverlayColor,
                static_cast<unsigned int>(width),
                static_cast<unsigned int>(height));
        }

        glfwSwapBuffers(win);
    }

    shutdownTextRenderer(textRenderer);
    return true;
}

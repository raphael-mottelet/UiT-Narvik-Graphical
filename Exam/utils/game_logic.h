#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils/state.h"
#include "utils/mesh.h"
#include "physics/track_physics.h"

// Race state helpers
inline bool isRaceFinished(const AppState& state)
{
    return state.winnerIndex >= 0 &&
           state.winnerIndex < static_cast<int>(state.marbles.size());
}

inline bool didPlayerWin(const AppState& state)
{
    if (!isRaceFinished(state))
        return false;

    const Marble& winner = state.marbles[state.winnerIndex];
    return winner.isPlayer;
}

// Update window title and console once the race ends.
// The title is updated every frame so it stays clearly visible.
// The console message is printed only once.
inline void updateEndOfRaceUI(
    GLFWwindow*     window,
    const AppState& state,
    bool&           resultReported
) {
    if (!isRaceFinished(state))
        return;

    bool playerWon = didPlayerWin(state);

    // Print to console only once
    if (!resultReported) {
        if (playerWon) {
            std::cout << ">>> Victory! You won the race.\n";
        } else {
            std::cout << ">>> Defeat. An opponent reached the finish line first.\n";
        }
        resultReported = true;
    }

    // Always keep the window title in a clear victory/defeat state
    if (playerWon) {
        glfwSetWindowTitle(window, "Marble Run - VICTORY");
    } else {
        glfwSetWindowTitle(window, "Marble Run - DEFEAT");
    }
}

// Render start (green) and finish (red) lines on the track.
inline void renderStartFinishLines(
    const AppState& state,
    const Mesh&     box,
    GLint           uModel,
    GLint           uNormalMat,
    GLint           uMatAmbient,
    GLint           uMatDiffuse,
    GLint           uMatSpecular,
    GLint           uMatShine
) {
    const Track& track = state.track;

    float halfWidth      = 0.5f * track.width;
    const float thick    = 0.04f;
    const float depth    = 0.25f;
    const float epsilonY = 0.002f;

    // Start line (green)
    {
        float z = track.startZ;
        float h = trackHeight(z);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model,
                               glm::vec3(0.0f,
                                         h + thick * 0.5f + epsilonY,
                                         z));
        model = glm::scale(model,
                           glm::vec3(halfWidth * 2.0f, thick, depth));

        glm::mat4 normalMat = glm::transpose(glm::inverse(model));

        glUniform3f(uMatAmbient,  0.02f, 0.15f, 0.02f);
        glUniform3f(uMatDiffuse,  0.1f,  0.8f,  0.1f);
        glUniform3f(uMatSpecular, 0.4f,  0.9f,  0.4f);
        glUniform1f(uMatShine,    32.0f);

        glUniformMatrix4fv(uModel,     1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uNormalMat, 1, GL_FALSE, glm::value_ptr(normalMat));

        glBindVertexArray(box.vao);
        glDrawElements(GL_TRIANGLES, box.indexCount, GL_UNSIGNED_INT, 0);
    }

    // Finish line (red)
    {
        float z = track.finishZ;
        float h = trackHeight(z);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model,
                               glm::vec3(0.0f,
                                         h + thick * 0.5f + epsilonY,
                                         z));
        model = glm::scale(model,
                           glm::vec3(halfWidth * 2.0f, thick, depth));

        glm::mat4 normalMat = glm::transpose(glm::inverse(model));

        glUniform3f(uMatAmbient,  0.15f, 0.02f, 0.02f);
        glUniform3f(uMatDiffuse,  0.9f,  0.1f, 0.1f);
        glUniform3f(uMatSpecular, 0.9f,  0.4f, 0.4f);
        glUniform1f(uMatShine,    32.0f);

        glUniformMatrix4fv(uModel,     1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uNormalMat, 1, GL_FALSE, glm::value_ptr(normalMat));

        glBindVertexArray(box.vao);
        glDrawElements(GL_TRIANGLES, box.indexCount, GL_UNSIGNED_INT, 0);
    }
}

// Render a full-screen overlay: green on victory, red on defeat.
// This tint is intentionally semi-transparent so the scene is still visible.
inline void renderVictoryOverlay(
    const AppState& state,
    GLuint          overlayProg,
    GLuint          overlayVAO,
    GLint           uOverlayColor
) {
    if (!isRaceFinished(state))
        return;

    bool playerWon = didPlayerWin(state);

    glUseProgram(overlayProg);
    glBindVertexArray(overlayVAO);

    if (playerWon) {
        glUniform4f(uOverlayColor, 0.0f, 1.0f, 0.0f, 0.25f); // translucent green
    } else {
        glUniform4f(uOverlayColor, 1.0f, 0.0f, 0.0f, 0.25f); // translucent red
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

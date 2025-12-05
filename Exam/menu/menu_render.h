#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "menu_state.h"
#include "../graphics/text_renderer.h"

// Draws a dark overlay and the menu text on top of the 3D scene
inline void renderMenuOverlay(const MenuState& menu,
                              const TextRenderer& textRenderer,
                              GLuint overlayProg,
                              GLuint overlayVAO,
                              GLint  uOverlayColor,
                              unsigned int screenWidth,
                              unsigned int screenHeight) {
    if (!menu.visible) {
        return;
    }

    // 1) Dark semi-transparent background over the whole screen
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(overlayProg);
    glUniform4f(uOverlayColor, 0.0f, 0.0f, 0.0f, 0.65f);

    glBindVertexArray(overlayVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    // 2) Text overlay using the shared TextRenderer helper
    int xCenter = static_cast<int>(screenWidth / 2);
    int y = static_cast<int>(screenHeight - 80);

    float titleScale  = 1.3f;
    float textScale   = 1.0f;
    float shadowAlpha = 0.7f;

    glm::vec4 titleColor    = glm::vec4(1.0f, 1.0f, 0.2f, 1.0f);
    glm::vec4 textColor     = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 hintColor     = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
    glm::vec4 selectedColor = glm::vec4(0.2f, 1.0f, 0.2f, 1.0f);

    // Title
    drawTextLine(textRenderer, screenWidth, xCenter, y,
                 titleScale, shadowAlpha, "MARBLE RUN", titleColor);
    y -= 28;

    // Help text
    drawTextLine(textRenderer, screenWidth, xCenter, y,
                 textScale, shadowAlpha, "UP/DOWN or W/S : select map", hintColor);
    y -= 22;

    drawTextLine(textRenderer, screenWidth, xCenter, y,
                 textScale, shadowAlpha, "LEFT/RIGHT or A/D : change marble size", hintColor);
    y -= 22;

    drawTextLine(textRenderer, screenWidth, xCenter, y,
                 textScale, shadowAlpha, "ENTER : start   M : toggle menu", hintColor);
    y -= 30;

    // Current marble size
    std::string sizeLine = std::string("Marble size: ") + marbleSizeLabel(menu.marbleSize);
    drawTextLine(textRenderer, screenWidth, xCenter, y,
                 textScale, shadowAlpha, sizeLine, textColor);
    y -= 28;

    // Maps header
    drawTextLine(textRenderer, screenWidth, xCenter, y,
                 textScale, shadowAlpha, "Maps:", textColor);
    y -= 24;

    // Each map entry, highlight selected
    for (int i = 0; i < static_cast<int>(menu.maps.size()); ++i) {
        const MapInfo& m = menu.maps[i];
        std::string line = (i == menu.selectedIndex ? "> " : "  ") + m.name;
        glm::vec4 col = (i == menu.selectedIndex) ? selectedColor : textColor;

        drawTextLine(textRenderer, screenWidth, xCenter, y,
                     textScale, shadowAlpha, line, col);
        y -= 22;
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

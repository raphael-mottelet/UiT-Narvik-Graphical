#pragma once

#include <algorithm>
#include <cmath>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils/state.h"
#include "utils/mesh.h"
#include "physics/track_physics.h"

// Render a continuous ribbon mesh that follows trackHeight(z) along the Z axis using OpenGL forward rasterization
inline void renderTrackRibbon(
    const Track& track,
    const Mesh&  box,
    GLint        uModel,
    GLint        uNormalMat,
    float        halfWidth,
    float        thickness,
    float        xOffset = 0.0f
) {
    int   segments = 140; // Number of box segments used to approximate the continuous track
    float zStart   = track.startZ;
    float zEnd     = track.finishZ;
    if (zEnd > zStart) std::swap(zStart, zEnd); // Ensure zStart is closer to zero than zEnd
    float dz = (zEnd - zStart) / static_cast<float>(segments); // Uniform step along Z

    for (int i = 0; i < segments; ++i) {
        float z0   = zStart + dz * static_cast<float>(i);
        float z1   = zStart + dz * static_cast<float>(i + 1);
        float zMid = 0.5f * (z0 + z1);                    // Center of the current segment along Z
        float hMid = trackHeight(zMid);                   // Track height at segment center from analytic profile

        float segLen = std::abs(z1 - z0);                 // Physical length of the current box segment along Z

        glm::mat4 model(1.0f);
        model = glm::translate(model,
                               glm::vec3(xOffset,
                                         hMid - thickness * 0.5f, // Align top face with track height
                                         zMid));
        model = glm::scale(model,
                           glm::vec3(halfWidth * 2.0f, thickness, segLen)); // Scale unit box to width and local length

        glm::mat4 normalMat = glm::transpose(glm::inverse(model)); // Normal matrix for correct lighting on scaled geometry
        glUniformMatrix4fv(uModel,     1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uNormalMat, 1, GL_FALSE, glm::value_ptr(normalMat));

        glDrawElements(GL_TRIANGLES, box.indexCount, GL_UNSIGNED_INT, 0); // Draw one ribbon segment in the rasterization pass
    }
}

// Render one side rail that follows the track height profile using scaled box segments
inline void renderSideRail(
    const Track& track,
    const Mesh&  box,
    GLint        uModel,
    GLint        uNormalMat,
    float        xOffset,
    float        railHeight,
    float        railThickness
) {
    int   segments = 140; // Use the same segmentation as the track ribbon for consistent geometry
    float zStart   = track.startZ;
    float zEnd     = track.finishZ;
    if (zEnd > zStart) std::swap(zStart, zEnd);
    float dz = (zEnd - zStart) / static_cast<float>(segments);

    for (int i = 0; i < segments; ++i) {
        float z0   = zStart + dz * static_cast<float>(i);
        float z1   = zStart + dz * static_cast<float>(i + 1);
        float zMid = 0.5f * (z0 + z1);
        float hMid = trackHeight(zMid); // Rail height anchored to analytical track surface

        float segLen = std::abs(z1 - z0);

        glm::mat4 model(1.0f);
        model = glm::translate(model,
                               glm::vec3(xOffset,
                                         hMid + railHeight * 0.5f, // Raise rail above the track surface
                                         zMid));
        model = glm::scale(model,
                           glm::vec3(railThickness, railHeight, segLen)); // Thin box stretched along Z

        glm::mat4 normalMat = glm::transpose(glm::inverse(model));
        glUniformMatrix4fv(uModel,     1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uNormalMat, 1, GL_FALSE, glm::value_ptr(normalMat));

        glDrawElements(GL_TRIANGLES, box.indexCount, GL_UNSIGNED_INT, 0);
    }
}

// Render a wide ground slab below the track as a visual base in the rasterized scene
inline void renderBaseGround(
    const Track& track,
    const Mesh&  groundBox,
    GLint        uModel,
    GLint        uNormalMat,
    GLint        uMatAmbient,
    GLint        uMatDiffuse,
    GLint        uMatSpecular,
    GLint        uMatShine
) {
    float groundY     = -2.0f;                                            // Vertical position of the top surface of the slab
    float groundThick = 0.3f;                                             // Thickness of the slab geometry
    float trackLength = std::abs(track.finishZ - track.startZ) + 40.0f;   // Extra margin in front and behind the track
    float centerZ     = 0.5f * (track.startZ + track.finishZ);            // Center of the base along Z

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model,
                           glm::vec3(0.0f, groundY - groundThick * 0.5f, centerZ)); // Place box so top face is at groundY
    model = glm::scale(model,
                       glm::vec3(track.width * 3.0f, groundThick, trackLength));   // Scale to cover track width and length

    glm::mat4 normalMat = glm::transpose(glm::inverse(model));

    glUniform3f(uMatAmbient,  0.12f, 0.08f, 0.05f); // Dark brown ambient for soil-like base
    glUniform3f(uMatDiffuse,  0.25f, 0.16f, 0.10f); // Slightly lighter diffuse response
    glUniform3f(uMatSpecular, 0.15f, 0.15f, 0.15f); // Low specular to keep ground mostly matte
    glUniform1f(uMatShine,    8.0f);                // Modest shininess value for broad reflections

    glUniformMatrix4fv(uModel,     1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uNormalMat, 1, GL_FALSE, glm::value_ptr(normalMat));

    glDrawElements(GL_TRIANGLES, groundBox.indexCount, GL_UNSIGNED_INT, 0);
}

// Render the wooden track surface that marbles roll on, following the analytic height profile trackHeight(z)
inline void renderTrackSurface(
    const Track& track,
    const Mesh&  groundBox,
    GLint        uModel,
    GLint        uNormalMat,
    GLint        uMatAmbient,
    GLint        uMatDiffuse,
    GLint        uMatSpecular,
    GLint        uMatShine
) {
    glUniform3f(uMatAmbient,  0.25f, 0.15f, 0.05f); // Warm wood-like ambient tone
    glUniform3f(uMatDiffuse,  0.6f,  0.4f,  0.2f);  // Stronger diffuse term for visible lighting gradients
    glUniform3f(uMatSpecular, 0.25f, 0.25f, 0.25f); // Moderate specular highlights to suggest polished wood
    glUniform1f(uMatShine,   16.0f);                // Medium shininess for track reflections

    float halfWidth = track.width * 0.5f - 0.15f;   // Slightly narrower than full width to leave room for rails
    float thickness = 0.25f;                        // Vertical thickness of the track ribbon

    renderTrackRibbon(track, groundBox, uModel, uNormalMat, halfWidth, thickness, 0.0f);
}

// Render red side rails along the left and right edges of the track for visual and physical boundaries
inline void renderTrackRails(
    const Track& track,
    const Mesh&  groundBox,
    GLint        uModel,
    GLint        uNormalMat,
    GLint        uMatAmbient,
    GLint        uMatDiffuse,
    GLint        uMatSpecular,
    GLint        uMatShine
) {
    float railHeight    = 0.4f;                  // Vertical extent of each rail
    float railThickness = 0.12f;                 // Width of the rail geometry in X
    float halfWidth     = track.width * 0.5f;    // Rail aligned to outer track edges

    glUniform3f(uMatAmbient,  0.1f, 0.02f, 0.02f); // Dark red ambient base
    glUniform3f(uMatDiffuse,  0.9f, 0.1f, 0.1f);   // Bright red diffuse color for strong visual contrast
    glUniform3f(uMatSpecular, 0.6f, 0.6f, 0.6f);   // Strong specular to make rails stand out under lighting
    glUniform1f(uMatShine,   32.0f);               // Higher shininess value for tighter highlights

    renderSideRail(track, groundBox, uModel, uNormalMat,
                   -halfWidth, railHeight, railThickness); // Left rail along negative X edge
    renderSideRail(track, groundBox, uModel, uNormalMat,
                    halfWidth, railHeight, railThickness); // Right rail along positive X edge
}

// Render solid obstacles placed along the track surface using scaled boxes aligned with the height profile
inline void renderTrackObstacles(
    const Track& track,
    const Mesh&  groundBox,
    GLint        uModel,
    GLint        uNormalMat,
    GLint        uMatAmbient,
    GLint        uMatDiffuse,
    GLint        uMatSpecular,
    GLint        uMatShine
) {
    glUniform3f(uMatAmbient,  0.15f, 0.05f, 0.05f); // Ambient term for obstacle color
    glUniform3f(uMatDiffuse,  0.8f,  0.1f, 0.1f);   // Diffuse response for visibility under rasterized lighting
    glUniform3f(uMatSpecular, 0.4f,  0.4f, 0.4f);   // Specular highlight to distinguish obstacle surfaces
    glUniform1f(uMatShine,    32.0f);              // Relatively shiny obstacles compared to ground

    for (const Obstacle& ob : track.obstacles) {
        float height = 0.8f;                         // Fixed vertical size for all obstacles
        float hBase  = trackHeight(ob.center.z);     // Base height from track profile at obstacle center

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model,
                               glm::vec3(ob.center.x,
                                         hBase + height * 0.5f, // Place obstacle so its base rests on the track surface
                                         ob.center.z));
        model = glm::scale(model,
                           glm::vec3(ob.size.x, height, ob.size.y)); // Use obstacle size as XZ footprint, height as Y

        glm::mat4 normalMat = glm::transpose(glm::inverse(model));

        glUniformMatrix4fv(uModel,     1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uNormalMat, 1, GL_FALSE, glm::value_ptr(normalMat));

        glDrawElements(GL_TRIANGLES, groundBox.indexCount, GL_UNSIGNED_INT, 0);
    }
}

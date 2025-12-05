#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "utils/state.h"

// Simple sun-like light source used in OpenGL rasterization with Phong-style shading
struct SunLight {
    glm::vec3 position{0.0f};
    glm::vec3 ambient{0.4f, 0.4f, 0.45f};
    glm::vec3 diffuse{1.1f, 1.1f, 1.0f};
    glm::vec3 specular{1.0f, 1.0f, 1.0f};
};

// Creates a sun light placed high above the center of the track for consistent rasterized lighting across the scene
inline SunLight makeSunLightAboveTrack(const Track& track) {
    SunLight s;

    float centerZ = 0.5f * (track.startZ + track.finishZ); // Midpoint of the track along Z

    s.position = glm::vec3(0.0f, 30.0f, centerZ);          // Light placed high above track to simulate sun

    s.ambient  = glm::vec3(0.45f, 0.45f, 0.5f);            // Soft ambient term for base brightness
    s.diffuse  = glm::vec3(1.3f, 1.3f, 1.2f);              // Main diffuse term for lit surfaces
    s.specular = glm::vec3(1.0f, 1.0f, 1.0f);              // Specular term for highlights on marbles and track
    return s;
}

// Uploads light parameters to a GLSL program that performs forward lighting in an OpenGL rasterization shader using light.position, light.ambient, light.diffuse and light.specular uniforms
inline void uploadSunLight(GLuint program, const SunLight& s) {
    GLint posLoc  = glGetUniformLocation(program, "light.position");
    GLint ambLoc  = glGetUniformLocation(program, "light.ambient");
    GLint diffLoc = glGetUniformLocation(program, "light.diffuse");
    GLint specLoc = glGetUniformLocation(program, "light.specular");

    if (posLoc  >= 0) glUniform3fv(posLoc,  1, &s.position[0]);
    if (ambLoc  >= 0) glUniform3fv(ambLoc,  1, &s.ambient[0]);
    if (diffLoc >= 0) glUniform3fv(diffLoc, 1, &s.diffuse[0]);
    if (specLoc >= 0) glUniform3fv(specLoc, 1, &s.specular[0]);
}

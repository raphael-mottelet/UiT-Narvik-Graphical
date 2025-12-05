#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils/mesh.h"
#include "graphics/lighting.h"

// Skybox and sun rendering for an OpenGL 3.3 forward rasterization pipeline (no ray tracing or path tracing)

inline void renderSkybox(
    const Mesh&      skyCube,
    GLuint           skyProg,
    GLint            uSkyModel,
    GLint            uSkyView,
    GLint            uSkyProj,
    const glm::mat4& view,
    const glm::mat4& proj
) {
    glDepthMask(GL_FALSE); // Disable depth writes to keep skybox behind all rasterized scene geometry
    glUseProgram(skyProg); // Activate skybox shader program for the current rasterization pass

    glm::mat4 viewNoTrans = glm::mat4(glm::mat3(view));                 // Remove translation from view matrix so skybox remains visually infinite around camera
    glm::mat4 model       = glm::scale(glm::mat4(1.0f), glm::vec3(80.0f)); // Scale unit cube mesh to a large cube surrounding the camera in world space

    glUniformMatrix4fv(uSkyModel, 1, GL_FALSE, glm::value_ptr(model));       // Upload skybox model transform for vertex positions
    glUniformMatrix4fv(uSkyView,  1, GL_FALSE, glm::value_ptr(viewNoTrans)); // Upload view matrix without translation for infinite sky effect
    glUniformMatrix4fv(uSkyProj,  1, GL_FALSE, glm::value_ptr(proj));        // Upload projection matrix used for skybox rasterization

    glBindVertexArray(skyCube.vao);                                          // Bind cube mesh used to rasterize the sky
    glDrawElements(GL_TRIANGLES, skyCube.indexCount, GL_UNSIGNED_INT, 0);    // Render skybox as indexed triangles in the forward rasterization pipeline

    glDepthMask(GL_TRUE); // Re-enable depth writes for subsequent scene rendering passes
}

inline void renderSun(
    const SunLight&  sun,
    const Mesh&      sphere,
    GLint            uModel,
    GLint            uNormalMat,
    GLint            uMatAmbient,
    GLint            uMatDiffuse,
    GLint            uMatSpecular,
    GLint            uMatShine
) {
    glBindVertexArray(sphere.vao); // Bind shared sphere mesh used to rasterize the sun disk

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, sun.position);       // Position sun mesh at light position above track
    model = glm::scale(model, glm::vec3(2.5f));        // Enlarge sphere to a visible glowing sun size in world space

    glm::mat4 normalMat = glm::transpose(glm::inverse(model)); // Compute normal matrix for correct Phong lighting on scaled sun geometry

    glUniformMatrix4fv(uModel,     1, GL_FALSE, glm::value_ptr(model));     // Upload sun model transform for vertex shader in rasterization
    glUniformMatrix4fv(uNormalMat, 1, GL_FALSE, glm::value_ptr(normalMat)); // Upload normal matrix for correct normal transformation

    glUniform3f(uMatAmbient,  1.4f, 1.3f, 1.1f); // High ambient term to give constant glow in shaded regions
    glUniform3f(uMatDiffuse,  1.6f, 1.5f, 1.2f); // Strong diffuse term to appear as bright light source in rasterized scene
    glUniform3f(uMatSpecular, 1.0f, 1.0f, 1.0f); // White specular term for sharp highlights
    glUniform1f(uMatShine,    8.0f);             // Moderate shininess exponent controlling highlight spread on sun surface

    glDrawElements(GL_TRIANGLES, sphere.indexCount, GL_UNSIGNED_INT, 0); // Render sun with forward rasterization using indexed triangles
}

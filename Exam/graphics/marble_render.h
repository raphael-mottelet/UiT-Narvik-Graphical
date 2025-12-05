#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils/state.h"
#include "utils/mesh.h"

// Renders all marbles as shaded spheres in a forward OpenGL rasterization pass using a Phong-like lighting model
inline void renderMarbles(
    const AppState& state,
    const Mesh&     sphere,
    GLint           uModel,
    GLint           uNormalMat,
    GLint           uMatAmbient,
    GLint           uMatDiffuse,
    GLint           uMatSpecular,
    GLint           uMatShine
) {
    glBindVertexArray(sphere.vao); // Bind shared sphere mesh used for all marbles in the rasterization pipeline

    for (const Marble& m : state.marbles) {
        // Set per-marble material parameters so lighting in the rasterization shader matches marble color and shininess
        glUniform3fv(uMatAmbient,  1, glm::value_ptr(m.colorAmbient));
        glUniform3fv(uMatDiffuse,  1, glm::value_ptr(m.colorDiffuse));
        glUniform3fv(uMatSpecular, 1, glm::value_ptr(m.colorSpecular));
        glUniform1f(uMatShine,        m.shininess);

        glm::mat4 model = glm::mat4(1.0f);                        // Start with identity model matrix
        model = glm::translate(model, m.position);                // Place sphere at marble position in world space
        model = glm::scale(model, glm::vec3(m.radius));           // Scale unit sphere to match marble radius

        glm::mat4 normalMat = glm::transpose(glm::inverse(model)); // Build normal matrix so rasterized lighting uses correct transformed normals

        glUniformMatrix4fv(uModel,     1, GL_FALSE, glm::value_ptr(model));     // Send model matrix to vertex shader for rasterization
        glUniformMatrix4fv(uNormalMat, 1, GL_FALSE, glm::value_ptr(normalMat)); // Send normal matrix to fragment/vertex shader for lighting

        glDrawElements(GL_TRIANGLES, sphere.indexCount, GL_UNSIGNED_INT, 0);    // Draw one marble mesh using indexed triangles in the rasterization pipeline
    }
}

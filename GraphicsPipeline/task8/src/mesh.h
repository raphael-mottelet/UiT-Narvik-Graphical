#pragma once
#include <glad/glad.h>
#include <vector>

struct Mesh {
    GLuint vao = 0, vbo = 0, ebo = 0;
    GLsizei indexCount = 0;
};

inline Mesh makePyramid(){
    // base at y=-0.5, tip at y=+0.5
    const float v[] = {
        -0.5f, -0.5f, -0.5f,  // 0
         0.5f, -0.5f, -0.5f,  // 1
         0.5f, -0.5f,  0.5f,  // 2
        -0.5f, -0.5f,  0.5f,  // 3
         0.0f,  0.5f,  0.0f   // 4 (tip)
    };
    // base faces downward (OK), side faces CCW from outside
    const unsigned int idx[] = {
        // base (downward)
        0, 1, 2,
        2, 3, 0,
        // sides (fixed winding)
        0, 4, 1,
        1, 4, 2,
        2, 4, 3,
        3, 4, 0
    };

    Mesh m{};
    glGenVertexArrays(1, &m.vao);
    glBindVertexArray(m.vao);

    glGenBuffers(1, &m.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);

    glGenBuffers(1, &m.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

    glBindVertexArray(0);
    m.indexCount = sizeof(idx) / sizeof(idx[0]);
    return m;
}

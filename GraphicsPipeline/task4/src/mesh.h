#pragma once
#include <glad/glad.h>

struct Mesh {
    GLuint vao = 0, vbo = 0, ebo = 0;
    GLsizei indexCount = 0;
};

// same geometry style as Task 2: 8 verts + EBO
inline Mesh makeBox(){
    const float v[] = {
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f, -0.5f, 0.5f,-0.5f,
        -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f
    };
    const unsigned idx[] = {
        0,1,2, 2,3,0,   // back
        4,5,6, 6,7,4,   // front
        0,3,7, 7,4,0,   // left
        1,2,6, 6,5,1,   // right
        3,2,6, 6,7,3,   // top
        0,1,5, 5,4,0    // bottom
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
    m.indexCount = (GLsizei)(sizeof(idx)/sizeof(idx[0]));
    return m;
}

//PYRAMID with per-face integer IDs for color animation
inline Mesh makePyramidWithFaceId(){
    struct V { float x,y,z; int face; };

    V verts[] = {
        // base (face = -1)
        {-0.5f,-0.5f,-0.5f,-1}, { 0.5f,-0.5f,-0.5f,-1}, { 0.5f,-0.5f, 0.5f,-1}, { -0.5f,-0.5f, 0.5f,-1},
        // side 0
        {-0.5f,-0.5f,-0.5f, 0}, { 0.5f,-0.5f,-0.5f, 0}, { 0.0f, 0.5f, 0.0f, 0},
        // side 1
        { 0.5f,-0.5f,-0.5f, 1}, { 0.5f,-0.5f, 0.5f, 1}, { 0.0f, 0.5f, 0.0f, 1},
        // side 2
        { 0.5f,-0.5f, 0.5f, 2}, {-0.5f,-0.5f, 0.5f, 2}, { 0.0f, 0.5f, 0.0f, 2},
        // side 3
        {-0.5f,-0.5f, 0.5f, 3}, {-0.5f,-0.5f,-0.5f, 3}, { 0.0f, 0.5f, 0.0f, 3}
    };

    unsigned idx[] = {
        // base
        0,1,2,  2,3,0,
        // sides
        4,5,6,   7,8,9,
        10,11,12, 13,14,15
    };

    Mesh m{};
    glGenVertexArrays(1, &m.vao);
    glBindVertexArray(m.vao);

    glGenBuffers(1, &m.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glGenBuffers(1, &m.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V), (void*)0);
    // integer face id
    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(1, 1, GL_INT, sizeof(V), (void*)(3*sizeof(float)));

    glBindVertexArray(0);
    m.indexCount = (GLsizei)(sizeof(idx)/sizeof(idx[0]));
    return m;
}

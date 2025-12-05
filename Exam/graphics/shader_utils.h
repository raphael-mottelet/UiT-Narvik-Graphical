#pragma once

#include <glad/glad.h>
#include <string>
#include <iostream>
#include <vector>

#include "shaders_embedded.h"

// Helper to compile a GLSL shader and print any compile errors
inline GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint logLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen > 1 ? logLen : 1);
        glGetShaderInfoLog(shader, logLen, nullptr, log.data());
        std::cerr << "[GLSL COMPILE ERROR]\n" << log.data() << std::endl;
    }
    return shader;
}

// Helper to link a GLSL program and print any link errors
inline GLuint linkProgram(GLuint vs, GLuint fs) {
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    GLint ok = GL_FALSE;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint logLen = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen > 1 ? logLen : 1);
        glGetProgramInfoLog(prog, logLen, nullptr, log.data());
        std::cerr << "[GLSL LINK ERROR]\n" << log.data() << std::endl;
    }

    glDetachShader(prog, vs);
    glDetachShader(prog, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

// Convenience wrapper used throughout the project
inline GLuint makeProgram(const char* vsSource, const char* fsSource) {
    GLuint vs = compileShader(GL_VERTEX_SHADER,   vsSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsSource);
    return linkProgram(vs, fs);
}

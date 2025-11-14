#pragma once
#include <glad/glad.h>
#include <string>
#include <stdexcept>
#include <iostream>

inline GLuint makeShader(GLenum type, const char* src){
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok = GL_FALSE;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if(!ok){
        char log[4096];
        glGetShaderInfoLog(s, 4096, nullptr, log);
        std::cerr << "Shader compile failed:\n" << log << std::endl;
        throw std::runtime_error("Shader compile failed");
    }
    return s;
}

inline GLuint makeProgram(const char* vsSrc, const char* fsSrc){
    GLuint vs = makeShader(GL_VERTEX_SHADER,   vsSrc);
    GLuint fs = makeShader(GL_FRAGMENT_SHADER, fsSrc);
    GLuint p  = glCreateProgram();
    glAttachShader(p, vs);
    glAttachShader(p, fs);
    glLinkProgram(p);
    GLint ok = GL_FALSE;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if(!ok){
        char log[4096];
        glGetProgramInfoLog(p, 4096, nullptr, log);
        std::cerr << "Program link failed:\n" << log << std::endl;
        throw std::runtime_error("Program link failed");
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return p;
}

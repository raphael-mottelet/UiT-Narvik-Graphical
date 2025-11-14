#pragma once
#include <glad/glad.h>
#include <string>
#include <stdexcept>
#include <iostream>

// compile a single shader from source and throw if it fails
inline GLuint makeShader(GLenum type, const char* src){
    GLuint s = glCreateShader(type);              // make the shader object
    glShaderSource(s, 1, &src, nullptr);          // feed the GLSL source
    glCompileShader(s);                           // compile it

    GLint ok = GL_FALSE;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);     // check compile result
    if(!ok){
        char log[4096];
        glGetShaderInfoLog(s, 4096, nullptr, log); // grab the error log
        std::cerr << "Shader compile failed:\n" << log << std::endl;
        throw std::runtime_error("Shader compile failed"); // stop early if bad
    }
    return s;                                     // return compiled shader
}

// link a vertex + fragment shader into a program and throw if it fails
inline GLuint makeProgram(const char* vsSrc, const char* fsSrc){
    GLuint vs = makeShader(GL_VERTEX_SHADER,   vsSrc); // build vertex shader
    GLuint fs = makeShader(GL_FRAGMENT_SHADER, fsSrc); // build fragment shader

    GLuint p  = glCreateProgram();                      // make program object
    glAttachShader(p, vs);                              // attach vertex shader
    glAttachShader(p, fs);                              // attach fragment shader
    glLinkProgram(p);                                   // link program

    GLint ok = GL_FALSE;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);             // check link result
    if(!ok){
        char log[4096];
        glGetProgramInfoLog(p, 4096, nullptr, log);     // grab link log
        std::cerr << "Program link failed:\n" << log << std::endl;
        throw std::runtime_error("Program link failed"); // stop early if bad
    }

    glDeleteShader(vs);                                 // shaders no longer needed
    glDeleteShader(fs);
    return p;                                           // return linked program
}

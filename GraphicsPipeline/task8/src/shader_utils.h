#pragma once
#include <glad/glad.h>
#include <string>
#include <iostream>
#include <vector>

inline GLuint compileShader(GLenum type, const char* src){
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, nullptr);
    glCompileShader(sh);
    GLint ok = GL_FALSE;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if(!ok){
        GLint len=0; glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(len);
        glGetShaderInfoLog(sh, len, nullptr, log.data());
        std::cerr << "[SHADER COMPILE ERROR] "
                  << (type==GL_VERTEX_SHADER?"VERTEX":"FRAGMENT") << "\n"
                  << log.data() << "\n";
        glDeleteShader(sh);
        return 0;
    }
    return sh;
}

inline GLuint makeProgram(const char* vs, const char* fs){
    GLuint v = compileShader(GL_VERTEX_SHADER, vs);
    GLuint f = compileShader(GL_FRAGMENT_SHADER, fs);
    if(!v || !f){ if(v) glDeleteShader(v); if(f) glDeleteShader(f); return 0; }

    GLuint prog = glCreateProgram();
    glAttachShader(prog, v);
    glAttachShader(prog, f);
    glLinkProgram(prog);
    glDeleteShader(v);
    glDeleteShader(f);

    GLint ok = GL_FALSE;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if(!ok){
        GLint len=0; glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(len);
        glGetProgramInfoLog(prog, len, nullptr, log.data());
        std::cerr << "[PROGRAM LINK ERROR]\n" << log.data() << "\n";
        glDeleteProgram(prog);
        return 0;
    }
    return prog;
}

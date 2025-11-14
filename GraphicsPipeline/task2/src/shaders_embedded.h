#pragma once

static const char* kVertexSrc = R"(#version 330 core
layout(location=0) in vec3 in_position;
uniform mat4 uMVP;
void main(){
    gl_Position = uMVP * vec4(in_position, 1.0);
}
)";

static const char* kFragmentSrc = R"(#version 330 core
out vec4 frag_color;
uniform vec3 uColor;
void main(){
    frag_color = vec4(uColor, 1.0); // red
}
)";

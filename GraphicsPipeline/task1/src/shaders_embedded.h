#pragma once

// embedded GLSL shader sources (vertex + fragment)

// vertex shader: transforms object positions by the MVP matrix
static const char* kVertexSrc = R"(#version 330 core
// position attribute at location 0
layout(location=0) in vec3 in_position;
// combined model-view-projection matrix
uniform mat4 uMVP;
void main(){
    // project to clip space
    gl_Position = uMVP * vec4(in_position, 1.0);
}
)";

// fragment shader: outputs a solid color
static const char* kFragmentSrc = R"(#version 330 core
// final color written to the framebuffer
out vec4 frag_color;
// RGB color provided by the app
uniform vec3 uColor;
void main(){
    // make it opaque (alpha = 1)
    frag_color = vec4(uColor, 1.0); // red
}
)";

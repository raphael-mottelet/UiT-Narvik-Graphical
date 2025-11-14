#pragma once

// color from x/y; gently flatten along Z (object space) 
static const char* kBoxVertexSrc = R"(#version 330 core
layout(location=0) in vec3 aPos;
uniform mat4 uMVP;
out vec3 vColor;

void main(){
    // Map x,y in [-0.5,0.5] to [0,1] for a simple gradient
    float ux = aPos.x * 0.5 + 0.5;
    float uy = aPos.y * 0.5 + 0.5;
    vColor = vec3(ux, uy, 1.0 - 0.7*ux);

    // Keep the Task 2 cube, just flatten Z a little so it still looks like a box
    const float FLATTEN = 0.95; // close to 1 → faces keep almost same size
    vec3 p = vec3(aPos.x, aPos.y, aPos.z * FLATTEN);

    gl_Position = uMVP * vec4(p, 1.0);
}
)";

static const char* kBoxFragmentSrc = R"(#version 330 core
in vec3 vColor;
out vec4 FragColor;
void main(){
    FragColor = vec4(vColor, 1.0);
}
)";

// PYRAMID: animate side colors with cosine over time
static const char* kPyrVertexSrc = R"(#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in int  aFace;   // 0..3 for sides, -1 for base
uniform mat4 uMVP;
flat out int vFace;

void main(){
    vFace = aFace;                   // do not interpolate face id
    gl_Position = uMVP * vec4(aPos, 1.0);
}
)";

static const char* kPyrFragmentSrc = R"(#version 330 core
flat in int vFace;
uniform float uTime;
out vec4 FragColor;

void main(){
    if(vFace < 0){
        FragColor = vec4(0.2, 0.2, 0.2, 1.0); // base: neutral
        return;
    }

    // phase per face: 0, pi/2, pi, 3pi/2
    float phase = float(vFace) * 1.57079632679;
    float t = 0.5 + 0.5 * cos(uTime + phase);

    vec3 base[4] = vec3[4](
        vec3(1.0,0.2,0.2),
        vec3(0.2,1.0,0.2),
        vec3(0.2,0.6,1.0),
        vec3(1.0,0.8,0.2)
    );

    vec3 c = mix(0.2 * base[vFace], base[vFace], t);
    FragColor = vec4(c, 1.0);
}
)";

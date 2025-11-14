#pragma once

// Simple shader strings for Task 8 (pyramid + fire particles)

// Vertex shader for the solid pyramid (just MVP transform)
static const char* kMVP_VS = R"(#version 330 core
// position from VBO (x,y,z)
layout(location=0) in vec3 aPos;

// uniforms
uniform mat4 uMVP; // Model * View * Projection

void main(){
    gl_Position = uMVP * vec4(aPos, 1.0);
}
)";

// Fragment shader for the solid pyramid (flat color)
static const char* kSolid_FS = R"(#version 330 core
out vec4 FragColor;
uniform vec3 uColor; // RGB color

void main(){
    FragColor = vec4(uColor, 1.0);
}
)";

// Vertex shader for particles (billboarded quads, instanced)
static const char* kPart_VS = R"(#version 330 core
// per-vertex corner of the quad in local space (-0.5..+0.5)
layout(location=0) in vec2 aCorner;

// per-instance attributes
layout(location=1) in vec3 iPos;   // particle center (world)
layout(location=2) in float iT;    // particle life progress 0..1
layout(location=3) in float iSize; // quad size

// camera data
uniform mat4 uView;
uniform mat4 uProj;
uniform vec3 uCamRight; // camera right vector (world)
uniform vec3 uCamUp;    // camera up vector (world)

out float vT; // pass life progress to fragment shader

void main(){
    // Build a world-space billboard from camera basis and corner offsets
    vec3 worldPos = iPos
                  + (aCorner.x * iSize) * uCamRight
                  + (aCorner.y * iSize) * uCamUp;

    vT = iT;
    gl_Position = uProj * uView * vec4(worldPos, 1.0);
}
)";

// Fragment shader for particles (simple fire color ramp + soft alpha)
// App uses additive blending (SRC_ALPHA, ONE)
static const char* kPart_FS = R"(#version 330 core
in float vT;          // 0..1 over particle lifetime
out vec4 FragColor;   // final color

void main(){
    float t = clamp(vT, 0.0, 1.0);

    // Color ramp: orange -> yellow -> dark smoke
    vec3 c1 = vec3(1.0, 0.35, 0.05);  // orange (new flame)
    vec3 c2 = vec3(1.0, 1.00, 0.20);  // yellow (hot)
    vec3 c3 = vec3(0.15, 0.15, 0.15); // smoke (old)

    // Blend early part (0..0.5) to hot, then fade to smoke (0.5..1)
    vec3 hot   = mix(c1, c2, smoothstep(0.0, 0.5, t));
    vec3 color = mix(hot, c3, smoothstep(0.5, 1.0, t));

    // Soft alpha: quick fade-in, fade-out near the end
    float alpha = (1.0 - smoothstep(0.7, 1.0, t)) * smoothstep(0.0, 0.15, t);
    alpha *= 0.8; // overall intensity

    // Additive blending in the app will add RGB; alpha is just a weight here
    FragColor = vec4(color * alpha, alpha);
}
)";

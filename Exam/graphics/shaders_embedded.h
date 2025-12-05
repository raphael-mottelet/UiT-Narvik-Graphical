#pragma once

#include <glad/glad.h>
#include <string>
#include <iostream>
#include <vector>

// Embedded GLSL shader source for a real-time OpenGL 3.3 forward rasterization pipeline using Phong lighting (not ray tracing or path tracing)

// Vertex shader for Phong-lit geometry using a single light and standard model-view-projection transforms
static const char* kLitVS = R"(#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform mat4 uNormalMat;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    FragPos       = worldPos.xyz;

    // Normal matrix used to handle non-uniform scaling before lighting in the rasterization pass
    Normal = mat3(uNormalMat) * aNormal;

    gl_Position = uProj * uView * worldPos;
}
)";

// Fragment shader for Phong lighting in the forward rasterization pass with ambient, diffuse and specular terms
static const char* kLitFS = R"(#version 330 core

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;
    float shininess;
};

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform Light    light;
uniform Material material;
uniform vec3     uViewPos;

void main()
{
    // Normal vector in world space used for Phong shading in the forward rasterization shader
    vec3 norm = normalize(Normal);

    // Direction from fragment position to light position for point-light shading
    vec3 lightDir = normalize(light.position - FragPos);

    // Diffuse factor based on Lambert cosine term between normal and light direction
    float diff = max(dot(norm, lightDir), 0.0);

    // Specular factor based on Phong reflection model using view direction and reflected light direction
    vec3 viewDir    = normalize(uViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // Ambient, diffuse and specular contributions combined with material reflectance
    vec3 ambient  = light.ambient  * material.ambient;
    vec3 diffuse  = light.diffuse  * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;

    vec3 color = ambient + diffuse + specular;

    // Clamp range to keep brightness in a reasonable interval in the rasterized image
    color = clamp(color, 0.0, 2.5);

    FragColor = vec4(color, 1.0);
}
)";

// Vertex shader for a skybox cube surrounding the scene and rendered in the background of the rasterized frame
static const char* kSkyVS = R"(#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec3 vWorldDir;

void main()
{
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vWorldDir     = worldPos.xyz;

    gl_Position = uProj * uView * worldPos;
}
)";

// Fragment shader for a vertical gradient sky color based on world-space direction in the rasterized background
static const char* kSkyFS = R"(#version 330 core

in vec3 vWorldDir;
out vec4 FragColor;

uniform vec3 uSkyColor;

void main()
{
    // Normalized direction used to build a vertical gradient for the rasterized sky
    vec3 dir = normalize(vWorldDir);

    // Gradient factor t in [0,1] based on elevation given by the y component of the direction
    float t = clamp(dir.y * 0.5 + 0.5, 0.0, 1.0);

    // Top and bottom sky colors blended by t to produce a clear-sky appearance
    vec3 topColor    = uSkyColor;            // Typical value such as (0.7, 0.85, 1.0)
    vec3 bottomColor = vec3(0.85, 0.9, 1.0); // Horizon color slightly lighter and less saturated

    vec3 col = mix(bottomColor, topColor, t);
    FragColor = vec4(col, 1.0);
}
)";

// Vertex shader for 2D overlay quads rendered directly in clip space on top of the 3D rasterized scene
static const char* kOverlayVS = R"(#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

out vec2 vUV;

void main()
{
    vUV = aUV;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

// Fragment shader for flat colored overlay elements such as HUD panels or score bars in the final rasterized frame
static const char* kOverlayFS = R"(#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform vec4 uColor;

void main()
{
    FragColor = uColor;
}
)";

#pragma once

// Phong lighting (per-fragment), world-space
static const char* kLitVS = R"(#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform mat3 uNormalMat;

out vec3 vWorldPos;
out vec3 vWorldNormal;

void main(){
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;
    vWorldNormal = normalize(uNormalMat * aNormal);
    gl_Position = uProj * uView * worldPos;
}
)";

static const char* kLitFS = R"(#version 330 core
in vec3 vWorldPos;
in vec3 vWorldNormal;

uniform vec3 uViewPos;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

out vec4 FragColor;

void main(){
    // Normalize
    vec3 N = normalize(vWorldNormal);
    vec3 L = normalize(light.position - vWorldPos);
    vec3 V = normalize(uViewPos - vWorldPos);

    // Ambient
    vec3 ambient = light.ambient * material.ambient;

    // Diffuse (Lambert)
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = light.diffuse * material.diffuse * NdotL;

    // Specular (Phong)
    vec3 R = reflect(-L, N);
    float spec = pow(max(dot(R, V), 0.0), material.shininess);
    vec3 specular = light.specular * material.specular * spec;

    vec3 color = ambient + diffuse + specular;
    FragColor = vec4(color, 1.0);
}
)";

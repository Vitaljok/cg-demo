#version 450 core

layout(location = 1) uniform vec3 lightColor;
layout(location = 0) out vec4 fragColor;

void main() { fragColor = vec4(lightColor, 1.0); }
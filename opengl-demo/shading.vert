#version 450 core

layout(location = 0) in vec3 inPos;

layout(location = 3) uniform mat4 model;
layout(location = 4) uniform mat4 view;
layout(location = 5) uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(inPos.xyz, 1.0);  
}
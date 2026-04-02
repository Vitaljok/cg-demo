#version 450 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 projection;

layout(location = 0) out vec3 pos;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec2 uv;

void main() {
  gl_Position = projection * view * model * vec4(inPos.xyz, 1.0);
  pos = (model * vec4(inPos.xyz, 1.0)).xyz;
  normal = mat3(transpose(inverse(model))) * inNormal;
  uv = inUV;
}
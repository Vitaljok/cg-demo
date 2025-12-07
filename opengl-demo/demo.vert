#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aUV;

layout(location = 0) out vec3 vertexColor;
layout(location = 1) out vec2 uv; 

void main() {
  gl_Position = vec4(aPos.xyz, 1.0);
  vertexColor = aColor;
  uv = aUV;
}
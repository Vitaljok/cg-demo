#version 450 core

layout(location = 0) in vec3 vertexColor;
layout(location = 1) in vec2 uv;

layout(location = 0) uniform float value;
layout(location = 1) uniform sampler2D tex1;
layout(location = 2) uniform sampler2D tex2;

layout(location = 0) out vec4 fragColor;

void main() {
  vec4 clrTex1 = texture(tex1, uv);
  vec4 clrTex2 = texture(tex2, uv);
  vec4 clrVtx = vec4(vertexColor, 1.0f);
  fragColor = mix(clrTex1 * clrVtx, clrTex2, value);
}
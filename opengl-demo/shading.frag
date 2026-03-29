#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

layout(location = 3) uniform vec3 objectColor;
layout(location = 4) uniform vec3 lightColor;
layout(location = 5) uniform vec3 lightPos;
layout(location = 6) uniform vec3 cameraPos;

layout(location = 0) out vec4 fragColor;

void main() {
  vec3 ambient = 0.1 * lightColor * objectColor;

  vec3 norm = normalize(normal);
  vec3 lightDir = normalize(lightPos - pos);
  float diffuseAmount = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diffuseAmount * lightColor * objectColor;

  vec3 cameraDir = normalize(cameraPos - pos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float specularAmount = max(dot(cameraDir, reflectDir), 0.0);
  specularAmount = pow(specularAmount, 32);
  vec3 specular = 0.5 * specularAmount * lightColor * objectColor;

  fragColor = vec4(ambient + diffuse + specular, 1.0);
}
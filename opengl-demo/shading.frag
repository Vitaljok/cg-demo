#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};

struct Light {
  vec3 pos;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

layout(location = 3) uniform Material material;
layout(location = 6) uniform Light light;
layout(location = 10) uniform vec3 cameraPos;

layout(location = 0) out vec4 fragColor;

void main() {
  vec3 ambient = light.ambient * texture(material.diffuse, uv).rgb;

  vec3 norm = normalize(normal);
  vec3 lightDir = normalize(light.pos - pos);
  float diffuseAmount = max(dot(norm, lightDir), 0.0);
  vec3 diffuse =
      diffuseAmount * light.diffuse * texture(material.diffuse, uv).rgb;

  vec3 cameraDir = normalize(cameraPos - pos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float specularAmount = max(dot(cameraDir, reflectDir), 0.0);
  specularAmount = pow(specularAmount, material.shininess);
  vec3 specular =
      specularAmount * light.specular * texture(material.specular, uv).rgb;

  fragColor = vec4(ambient + diffuse + specular, 1.0);
}
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.hpp"

struct Material {
  glm::vec3 diffuse;
  glm::vec3 specular;
  float shininess;

  void use(ShaderProgram shader) {
    shader.setVec3("material.diffuse", glm::value_ptr(diffuse));
    shader.setVec3("material.specular", glm::value_ptr(specular));
    shader.setFloat("material.shininess", shininess);
  }
};

struct Light {
  glm::vec3 pos;
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;

  void use(ShaderProgram shader) {
    shader.setVec3("light.pos", glm::value_ptr(pos));
    shader.setVec3("light.ambient", glm::value_ptr(ambient));
    shader.setVec3("light.diffuse", glm::value_ptr(diffuse));
    shader.setVec3("light.specular", glm::value_ptr(specular));
  }
};
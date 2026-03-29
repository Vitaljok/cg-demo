#include "shader.hpp"

#include <demo/utils.hpp>
#include <exception>
#include <format>
#include <print>

ShaderProgram::ShaderProgram(const char *vertexFileName,
                             const char *fragmentFileName) {
  id = glCreateProgram();

  auto vertexShader = createShader(GL_VERTEX_SHADER, vertexFileName);
  auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentFileName);

  glAttachShader(id, vertexShader);
  glAttachShader(id, fragmentShader);
  glLinkProgram(id);

  GLint success;
  glGetProgramiv(id, GL_LINK_STATUS, &success);
  if (!success) {
    std::string info(512, 0);
    glGetProgramInfoLog(id, 512, NULL, info.data());
    throw std::runtime_error(
        std::format("Shader program linking failed\n{}", info));
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

GLuint ShaderProgram::createShader(const GLenum type, const char *fileName) {
  auto id = glCreateShader(type);

  auto source = readTextFile(fileName);
  const char *src = source.c_str();

  glShaderSource(id, 1, &src, NULL);
  glCompileShader(id);

  GLint success;
  glGetShaderiv(id, GL_COMPILE_STATUS, &success);
  if (!success) {
    std::string info(512, 0);
    glGetShaderInfoLog(id, 512, NULL, info.data());
    throw std::runtime_error(
        std::format("Shader compilation failed\n{}", info));
  }

  return id;
}

GLint ShaderProgram::getLocation(const char *name) {
  if (locationMap.contains(name)) {
    return locationMap[name];
  }

  auto loc = glGetUniformLocation(id, name);
  if (loc < 0) {
    throw std::runtime_error(
        std::format("Unable to find uniform with name {}", name));
  }
  locationMap.emplace(name, loc);

  return loc;
}

void ShaderProgram::setFloat(const char *name, float value) {
  glUniform1f(getLocation(name), value);
}

void ShaderProgram::setVec3(const char *name, const float *value) {
  glUniform3fv(getLocation(name), 1, value);
}

void ShaderProgram::setMatrix(const char *name, const float *value) {
  glUniformMatrix4fv(getLocation(name), 1, GL_FALSE, value);
}

void ShaderProgram::setTexture(const char *name, int setId, GLuint value) {
  glActiveTexture(GL_TEXTURE0 + setId);
  glUniform1i(getLocation(name), setId);
  glBindTexture(GL_TEXTURE_2D, value);
}

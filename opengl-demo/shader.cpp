#include "shader.hpp"

#include <demo/utils.hpp>
#include <exception>
#include <format>

Shader::Shader(GLenum type, const char *fileName) {
  id = glCreateShader(type);

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
}

Shader::~Shader() { glDeleteShader(id); }

ShaderProgram::ShaderProgram(const std::vector<Shader> &shaders) {
  id = glCreateProgram();

  for (const auto &one : shaders) {
    glAttachShader(id, one);
  }
  glLinkProgram(id);

  GLint success;
  glGetProgramiv(id, GL_LINK_STATUS, &success);
  if (!success) {
    std::string info(512, 0);
    glGetProgramInfoLog(id, 512, NULL, info.data());
    throw std::runtime_error(
        std::format("Shader program linking failed\n{}", info));
  }
}

ShaderProgram::~ShaderProgram() { glDeleteProgram(id); }

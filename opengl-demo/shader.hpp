#pragma once

#include <glad/gl.h>
#include <vector>

class Shader {
public:
  GLuint id;
  Shader(const GLenum type, const char *fileName);
  operator GLuint() const { return id; };
  ~Shader();
};

class ShaderProgram {
public:
  GLuint id;
  ShaderProgram(const std::vector<Shader> &shaders);
  operator GLuint() const { return id; };
  ~ShaderProgram();
};
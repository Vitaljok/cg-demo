#pragma once

#include <glad/gl.h>

class Texture {
public:
  GLuint id;
  Texture(const char *fileName, GLint format);
  operator GLuint() const { return id; };
  ~Texture();
};
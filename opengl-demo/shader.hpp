#pragma once

#include <glad/gl.h>
#include <map>
#include <string>

class ShaderProgram {
public:
  GLuint id;
  ShaderProgram() = default;
  ShaderProgram(const char *vertexFileName, const char *fragmentFileName);
  operator GLuint() const { return id; };

  void setFloat(const char *name, float value);
  void setVec3(const char *name, float x, float y, float z);
  void setMatrix(const char *name, const float *value);
  void setTexture(const char *name, int setId, GLuint value);

private:
  GLint getLocation(const char *name);
  std::map<std::string, GLint> locationMap;
  GLuint createShader(const GLenum type, const char *fileName);
};
#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>

struct VertexData {
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec2 uv;
};

class Mesh {
public:
  std::vector<VertexData> vertices;
  std::vector<uint32_t> indices;
  Mesh() = default;
  Mesh(const char *fileName);
  void draw();

private:
  GLuint vertexArray;
  GLuint vertexBuffer;
  GLuint indexBuffer;

  void upload();
};
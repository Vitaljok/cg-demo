#include "mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <format>
#include <stdexcept>

Mesh::Mesh(const char *fileName) {
  Assimp::Importer importer;
  auto scene = importer.ReadFile(fileName, aiProcess_Triangulate);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    throw std::runtime_error(
        std::format("Failed to load mesh: {}", importer.GetErrorString()));
  }

  auto mesh = scene->mMeshes[0]; // for now assume single mesh model

  for (size_t i = 0; i < mesh->mNumVertices; i++) {
    auto vtx = mesh->mVertices[i];
    VertexData vdata;
    vdata.pos = {vtx.x, vtx.y, vtx.z};
    if (mesh->mTextureCoords[0]) {
      auto uvs = mesh->mTextureCoords[0][i];
      vdata.uv = {uvs.x, uvs.y};
    }
    if (mesh->mNormals) {
      auto n = mesh->mNormals[i];
      vdata.normal = {n.x, n.y, n.z};
    }
    vertices.push_back(vdata);
  }

  for (size_t i = 0; i < mesh->mNumFaces; i++) {
    auto face = mesh->mFaces[i];
    for (size_t j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  importer.FreeScene();

  upload();
}

void Mesh::draw() {
  glBindVertexArray(vertexArray);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void *)0);
  glBindVertexArray(0);
}

void Mesh::upload() {
  // VAO
  glGenVertexArrays(1, &vertexArray);
  glBindVertexArray(vertexArray);

  // VBO
  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(),
               vertices.data(), GL_STATIC_DRAW);

  // EBO
  glGenBuffers(1, &indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(),
               indices.data(), GL_STATIC_DRAW);

  // Attribute layout
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                        (void *)offsetof(VertexData, pos));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                        (void *)offsetof(VertexData, normal));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                        (void *)offsetof(VertexData, uv));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0); // unbound
}

#include "texture.hpp"

#include <exception>
#include <format>
#include <stb_image.h>

Texture::Texture(const char *fileName, GLint format) {
  stbi_set_flip_vertically_on_load(true);
  int width, height, nChannels;
  uint8_t *pixels = stbi_load(fileName, &width, &height, &nChannels, 0);
  if (!pixels) {
    throw std::runtime_error(
        std::format("Failed to load texture {}", fileName));
  }
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, pixels);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(pixels);
}

Texture::~Texture() { glDeleteTextures(1, &id); }

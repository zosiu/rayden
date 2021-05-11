#include <rayden/Assert.hpp>

#include <rayden/Texture2D.hpp>

namespace rayden {

Texture2D::Texture2D(uint32_t width, uint32_t height) : _width(width), _height(height) {
  glGenTextures(1, &_id);
  REFUTE(_id == 0, "failed to create texture");
  bind();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _width, _height, 0, GL_RGBA, GL_FLOAT, nullptr);
  glBindImageTexture(0, _id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

Texture2D::~Texture2D() { glDeleteTextures(1, &_id); }

auto Texture2D::width() const -> uint32_t { return _width; }

auto Texture2D::height() const -> uint32_t { return _height; }

void Texture2D::bind() const { glBindTexture(GL_TEXTURE_2D, _id); }

void Texture2D::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

} // namespace rayden

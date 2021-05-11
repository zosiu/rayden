#pragma once

#include <cstdint>

#include <glad/glad.h>

namespace rayden {

class Texture2D {
public:
  Texture2D(uint32_t width, uint32_t height);
  ~Texture2D();
  Texture2D(const Texture2D &) = delete;
  Texture2D(Texture2D &&) = delete;
  auto operator=(const Texture2D &) -> Texture2D & = delete;
  auto operator=(Texture2D &&) -> Texture2D & = delete;

  [[nodiscard]] auto width() const -> uint32_t;
  [[nodiscard]] auto height() const -> uint32_t;

  void bind() const;
  void unbind() const;

private:
  GLuint _id = 0;
  uint32_t _width;
  uint32_t _height;
};

} // namespace rayden

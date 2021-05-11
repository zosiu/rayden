#pragma once

#include <glad/glad.h>

namespace rayden {

class FullScreenQuad {
public:
  FullScreenQuad();
  ~FullScreenQuad();
  FullScreenQuad(const FullScreenQuad &) = delete;
  FullScreenQuad(FullScreenQuad &&) = delete;
  auto operator=(const FullScreenQuad &) -> FullScreenQuad & = delete;
  auto operator=(FullScreenQuad &&) -> FullScreenQuad & = delete;

  void draw() const;

private:
  GLuint _vao = 0;
  GLuint _vbo = 0;
};
} // namespace rayden

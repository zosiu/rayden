#include <rayden/Assert.hpp>

#include <rayden/FullScreenQuad.hpp>

namespace rayden {

FullScreenQuad::FullScreenQuad() {
  float vertices[] = {
      1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, //
      1.0f,  1.0f,  0.0f, 1.0f, 1.0f, //
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, //
      -1.0f, 1.0f,  0.0f, 0.0f, 1.0f  //
  };

  glGenVertexArrays(1, &_vao);
  REFUTE(_vao == 0, "failed to create quad vao");
  glBindVertexArray(_vao);

  glGenBuffers(1, &_vbo);
  REFUTE(_vbo == 0, "failed to create quad vbo");

  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

FullScreenQuad::~FullScreenQuad() {
  glDeleteVertexArrays(1, &_vao);
  glDeleteBuffers(1, &_vbo);
}

void FullScreenQuad::draw() const { glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); }

} // namespace rayden

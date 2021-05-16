#pragma once

#include <glad/glad.h>

#include <initializer_list>
#include <string>
#include <utility>

namespace rayden {

enum class ShaderType : GLenum {
  Fragment = GL_FRAGMENT_SHADER,
  Vertex = GL_VERTEX_SHADER,
  Geometry = GL_GEOMETRY_SHADER,
  TessControl = GL_TESS_CONTROL_SHADER,
  TessEvaluation = GL_TESS_EVALUATION_SHADER,
  Compute = GL_COMPUTE_SHADER
};

class ShaderProgram {
public:
  ShaderProgram(std::initializer_list<std::pair<ShaderType, const char *>> shaders);
  ~ShaderProgram();
  ShaderProgram(const ShaderProgram &) = delete;
  ShaderProgram(ShaderProgram &&) = delete;
  auto operator=(const ShaderProgram &) -> ShaderProgram & = delete;
  auto operator=(ShaderProgram &&) -> ShaderProgram & = delete;

  void bind() const;
  void unbind() const;

  void upload_int_unifom(const std::string &name, int value) const;

private:
  inline auto compile_shader(GLenum shader_type, const char *source_file) -> GLuint;

private:
  GLuint _id = 0;
};

} // namespace rayden

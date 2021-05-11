#include <vector>

#include <rayden/Assert.hpp>
#include <rayden/File.hpp>
#include <rayden/Logger.hpp>

#include <rayden/ShaderProgram.hpp>

namespace rayden {

ShaderProgram::ShaderProgram(std::initializer_list<std::pair<ShaderType, const char *>> list_of_shaders) {
  std::vector<GLuint> shaders;
  for (auto [shader_type, shader_file_name] : list_of_shaders) {
    GLuint shader = compile_shader(static_cast<GLenum>(shader_type), shader_file_name);
    REFUTE(shader == 0, "failed to create shader");
    shaders.push_back(shader);
  }

  const GLuint shader_program = glCreateProgram();
  for (auto shader : shaders)
    glAttachShader(shader_program, shader);

  glLinkProgram(shader_program);
  GLint linking_result = GL_FALSE;
  glGetProgramiv(shader_program, GL_LINK_STATUS, &linking_result);

  if (linking_result == GL_FALSE) {
    GLchar log_message[1024];
    glGetProgramInfoLog(shader_program, 1024, nullptr, log_message);
    LOG_CRITICAL("[SHADER] linking failed: {}", log_message);

    glDeleteProgram(shader_program);
    for (auto shader : shaders)
      glDeleteShader(shader);
  } else {
    glValidateProgram(shader_program);
    for (auto shader : shaders) {
      glDetachShader(shader_program, shader);
      glDeleteShader(shader);
    }

    _id = shader_program;
    REFUTE(_id == 0, "failed to create shader program");
  }
}

ShaderProgram::~ShaderProgram() { glDeleteProgram(_id); }

void ShaderProgram::bind() const { glUseProgram(_id); }

void ShaderProgram::unbind() const { glUseProgram(0); }

auto ShaderProgram::compile_shader(GLenum shader_type, const char *source_file) -> GLuint {
  GLuint shader = glCreateShader(shader_type);
  std::string file_contents = File::contents(source_file);
  const char *shader_source = file_contents.c_str();
  glShaderSource(shader, 1, &shader_source, nullptr);
  glCompileShader(shader);

  GLint compilation_result = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compilation_result);

  if (compilation_result == GL_FALSE) {
    GLchar log_message[1024];
    glGetShaderInfoLog(shader, 1024, nullptr, log_message);
    LOG_CRITICAL("[SHADER] compilation failed: {}", log_message);
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

} // namespace rayden

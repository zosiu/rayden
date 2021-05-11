#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <rayden/Assert.hpp>
#include <rayden/Logger.hpp>

#include <rayden/OpenGLContext.hpp>

namespace rayden {

const std::unordered_map<GLenum, std::string> OPENGL_DEBUG_SOURCES = {
    {GL_DEBUG_SOURCE_API, "API"},
    {GL_DEBUG_SOURCE_APPLICATION, "APPLICATION"},
    {GL_DEBUG_SOURCE_OTHER, "OTHER"},
    {GL_DEBUG_SOURCE_SHADER_COMPILER, "SHADER COMPILER"},
    {GL_DEBUG_SOURCE_THIRD_PARTY, "THIRD PARTY"},
    {GL_DEBUG_SOURCE_WINDOW_SYSTEM, "WINDOW SYSTEM"},
};

const std::unordered_map<GLenum, std::string> OPENGL_DEBUG_TYPES = {
    {GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "DEPRECATED_BEHAVIOR"},
    {GL_DEBUG_TYPE_ERROR, "ERROR"},
    {GL_DEBUG_TYPE_OTHER, "OTHER"},
    {GL_DEBUG_TYPE_MARKER, "MARKER"},
    {GL_DEBUG_TYPE_PERFORMANCE, "PERFORMANCE"},
    {GL_DEBUG_TYPE_POP_GROUP, "POP GROUP"},
    {GL_DEBUG_TYPE_PUSH_GROUP, "PUSH GROUP"},
    {GL_DEBUG_TYPE_PORTABILITY, "PORTABILITY"},
    {GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, "UNDEFINED BEHAVIOR"},
};

const std::unordered_map<GLenum, std::string> OPENGL_DEBUG_SEVERITIES = {
    {GL_DEBUG_SEVERITY_NOTIFICATION, "NOTIFICATION"},
    {GL_DEBUG_SEVERITY_LOW, "LOW"},
    {GL_DEBUG_SEVERITY_MEDIUM, "MEDIUM"},
    {GL_DEBUG_SEVERITY_HIGH, "HIGH"},
};

OpenGLContext::OpenGLContext(GLFWwindow *window_handle) : _glfw_window_handle(window_handle) {
  REFUTE(_glfw_window_handle == nullptr, "nullptr glfw window in OpenGLContext");
}

void OpenGLContext::init() {
  glfwMakeContextCurrent(_glfw_window_handle);
  [[maybe_unused]] int glad_init_success = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
  ASSERT(glad_init_success, "failed to intialize GLAD");

  LOG_INFO("[OpenGL] vendor: {0}", glGetString(GL_VENDOR));
  LOG_INFO("[OpenGL] renderer: {0}", glGetString(GL_RENDERER));
  LOG_INFO("[OpenGL] version: {0}", glGetString(GL_VERSION));

#ifdef OPENGL_DEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  if (glDebugMessageCallback) {
    glDebugMessageCallback(
        [](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei /* length */, const GLchar *message,
           const void * /* user_param */) {
          const std::string &msg_source = OPENGL_DEBUG_SOURCES.at(source);
          const std::string &msg_severity = OPENGL_DEBUG_SEVERITIES.at(severity);
          const std::string &msg_type = OPENGL_DEBUG_TYPES.at(type);
          if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
            LOG_INFO("[OpenGL][{}] <{}> {}: {} ({})", msg_source, msg_severity, msg_type, message, id);
          } else {
            LOG_WARN("[OpenGL][{}] <{}> {}: {} ({})", msg_source, msg_severity, msg_type, message, id);
          }
        },
        nullptr);

    GLuint unusedIds = 0;
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
  } else {
    LOG_INFO("[OpenGL debug] glDebugMessageCallback is not available");
  }
#endif
}

void OpenGLContext::swap_buffers() { glfwSwapBuffers(_glfw_window_handle); }

} // namespace rayden

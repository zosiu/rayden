#pragma once

struct GLFWwindow;

namespace rayden {

class OpenGLContext {
public:
  OpenGLContext(GLFWwindow *window_handle);

  void init();
  void swap_buffers();

private:
  GLFWwindow *_glfw_window_handle;
};

} // namespace rayden

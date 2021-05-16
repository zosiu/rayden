#include <GLFW/glfw3.h>

#include <rayden/Assert.hpp>
#include <rayden/KeyPressedEvent.hpp>
#include <rayden/Logger.hpp>
#include <rayden/WindowClosedEvent.hpp>
#include <rayden/WindowResizedEvent.hpp>

#include <rayden/Window.hpp>

namespace rayden {

constexpr int MINIMUM_OPENGL_MAJOR_VERSION = 4;
constexpr int MINIMUM_OPENGL_MINOR_VERSION = 3;

Window::Window(const WindowProps &props) { create(props); }

Window::~Window() { destroy(); }

auto Window::width() const -> unsigned int { return props.width; }
auto Window::height() const -> unsigned int { return props.height; }
auto Window::native_window() const -> GLFWwindow * { return _native_window; }

void Window::set_event_callback(const EventCallbackFun &callback) { props.handle_event = callback; }

void Window::create(const WindowProps &window_props) {
  props.title = window_props.title;
  props.width = window_props.width;
  props.height = window_props.height;

  LOG_INFO("[create window] {} ({}x{})", props.title, props.width, props.height);
  if (Window::window_count == 0) {
    [[maybe_unused]] int glfw_init_success = glfwInit();
    ASSERT(glfw_init_success, "failed to intialize GLFW");

    glfwSetErrorCallback([](int error_code, const char *error_message) {
      LOG_ERROR("[GLFW Error] {} : {}", error_code, error_message);
    });

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, MINIMUM_OPENGL_MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MINIMUM_OPENGL_MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);

#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#endif

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#endif
  }

  _native_window = glfwCreateWindow((int)props.width, (int)props.height, props.title.c_str(), nullptr, nullptr);
  REFUTE(_native_window == nullptr, "Failed to create GLFW window");
  ++Window::window_count;

  _openGL_context = std::make_unique<OpenGLContext>(_native_window);
  _openGL_context->init();

  glfwSetWindowUserPointer(_native_window, &props);

  register_window_resized_callback();
  register_window_closed_callback();
  register_key_callback();
}

void Window::update() {
  glfwPollEvents();
  _openGL_context->swap_buffers();
}

void Window::register_window_resized_callback() {
  glfwSetFramebufferSizeCallback(_native_window, [](GLFWwindow *window, int width, int height) {
    auto handler = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    handler->width = (uint16_t)width;
    handler->height = (uint16_t)height;

    WindowResizedEvent window_resized((uint16_t)width, (uint16_t)height);
    handler->handle_event(window_resized);
  });
}

void Window::register_window_closed_callback() {
  glfwSetWindowCloseCallback(_native_window, [](GLFWwindow *window) {
    auto handler = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    WindowClosedEvent window_closed;
    handler->handle_event(window_closed);
  });
}

void Window::register_key_callback() {
  glfwSetKeyCallback(_native_window, [](GLFWwindow *window, int key, int /* scancode */, int action, int mods) {
    auto handler = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    switch (action) {
    case GLFW_PRESS: {
      KeyPressedEvent key_pressed_once(static_cast<KeyCode>(key), static_cast<KeyMod>(mods), false);
      handler->handle_event(key_pressed_once);
      break;
    }
    case GLFW_REPEAT: {
      KeyPressedEvent key_pressed_repeatedly(static_cast<KeyCode>(key), static_cast<KeyMod>(mods), true);
      handler->handle_event(key_pressed_repeatedly);
      break;
    }
    }
  });
}

void Window::destroy() {
  glfwDestroyWindow(_native_window);

  --Window::window_count;
  if (Window::window_count == 0) {
    glfwTerminate();
  }
}

} // namespace rayden

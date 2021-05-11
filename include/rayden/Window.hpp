#pragma once

#include <functional>
#include <memory>

#include "Event.hpp"
#include "OpenGLContext.hpp"

struct GLFWwindow;

namespace rayden {

struct WindowProps {
  static constexpr uint16_t DEFAULT_WIDTH = 800;
  static constexpr uint16_t DEFAULT_HEIGHT = 800;
  std::string title;
  uint16_t width;
  uint16_t height;

  WindowProps(std::string title = "some window", uint16_t width = DEFAULT_WIDTH, uint16_t height = DEFAULT_HEIGHT)
      : title(std::move(title)), width(width), height(height) {}
};

// GLFW window wrapper with event handling
class Window {
public:
  using EventCallbackFun = std::function<void(const Event &)>;

  Window(const WindowProps &props);
  ~Window();
  Window(const Window &) = delete;
  Window(Window &&) = delete;
  auto operator=(const Window &) -> Window & = delete;
  auto operator=(Window &&) -> Window & = delete;

  void update();

  [[nodiscard]] auto width() const -> unsigned int;
  [[nodiscard]] auto height() const -> unsigned int;
  [[nodiscard]] auto native_window() const -> GLFWwindow *;
  void set_event_callback(const EventCallbackFun &callback);

private:
  void create(const WindowProps &props);
  void destroy();

  void register_window_resized_callback();
  void register_window_closed_callback();
  void register_key_callback();

private:
  inline static uint8_t window_count = 0;

  GLFWwindow *_native_window = nullptr;
  std::unique_ptr<OpenGLContext> _openGL_context;

  struct WindowData {
    std::string title;
    unsigned int width = 0;
    unsigned int height = 0;

    EventCallbackFun handle_event;
  };

  WindowData props = WindowData();
};

} // namespace rayden

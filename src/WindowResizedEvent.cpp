#include <sstream>

#include <rayden/WindowResizedEvent.hpp>

namespace rayden {

auto WindowResizedEvent::static_type() -> EventType { return EventType::WindowResized; }

WindowResizedEvent::WindowResizedEvent(uint16_t width, uint16_t height) : _width(width), _height(height) {}

auto WindowResizedEvent::type() const -> EventType { return static_type(); }

auto WindowResizedEvent::width() const -> uint16_t { return _width; }
auto WindowResizedEvent::height() const -> uint16_t { return _height; }

auto WindowResizedEvent::name() const -> std::string {
  std::stringstream ss;
  ss << "[application event] window resized to " << width() << " x " << height();
  return ss.str();
}

} // namespace rayden

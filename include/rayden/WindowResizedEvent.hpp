#pragma once

#include "Event.hpp"

namespace rayden {

class WindowResizedEvent : public Event {
public:
  static auto static_type() -> EventType;

  WindowResizedEvent(uint16_t width, uint16_t height);

  [[nodiscard]] auto name() const -> std::string override;
  [[nodiscard]] auto type() const -> EventType override;

  [[nodiscard]] auto width() const -> uint16_t;
  [[nodiscard]] auto height() const -> uint16_t;

private:
  uint16_t _width;
  uint16_t _height;
};

} // namespace rayden

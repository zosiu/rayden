#pragma once

#include "Event.hpp"

namespace rayden {

class WindowClosedEvent : public Event {
public:
  static auto static_type() -> EventType;
  [[nodiscard]] auto name() const -> std::string override;
  [[nodiscard]] auto type() const -> EventType override;

  WindowClosedEvent() = default;
};

} // namespace rayden

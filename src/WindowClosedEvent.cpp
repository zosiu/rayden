#include <rayden/WindowClosedEvent.hpp>

namespace rayden {

auto WindowClosedEvent::static_type() -> EventType { return EventType::WindowClosed; }

auto WindowClosedEvent::type() const -> EventType { return static_type(); }

auto WindowClosedEvent::name() const -> std::string { return "[application event] window closed"; }

} // namespace rayden

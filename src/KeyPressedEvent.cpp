#include <sstream>

#include <rayden/KeyPressedEvent.hpp>

namespace rayden {

KeyPressedEvent::KeyPressedEvent(KeyCode key_code, bool repeated_or_not)
    : _key_code(key_code), _repeated(repeated_or_not) {}

auto KeyPressedEvent::static_type() -> EventType { return EventType::KeyPressed; }

auto KeyPressedEvent::type() const -> EventType { return static_type(); }

auto KeyPressedEvent::key_code() const -> KeyCode { return _key_code; }
auto KeyPressedEvent::is_repeated() const -> bool { return _repeated; }

auto KeyPressedEvent::name() const -> std::string {
  std::stringstream ss;
  ss << "[key event] { " << _key_code << " } key pressed";
  if (_repeated) {
    ss << " (repeatedly)";
  }

  return ss.str();
}

} // namespace rayden

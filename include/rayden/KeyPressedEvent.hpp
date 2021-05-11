#pragma once

#include "Event.hpp"
#include "KeyCodes.hpp"

namespace rayden {

class KeyPressedEvent : public Event {
public:
  KeyPressedEvent(KeyCode key_code, bool repeated_or_not);

  static auto static_type() -> EventType;
  [[nodiscard]] auto name() const -> std::string override;
  [[nodiscard]] auto type() const -> EventType override;

  [[nodiscard]] auto key_code() const -> KeyCode;
  [[nodiscard]] auto is_repeated() const -> bool;

private:
  KeyCode _key_code;
  bool _repeated;
};

} // namespace rayden

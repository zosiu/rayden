#pragma once

#include <rayden/Event.hpp>
#include <rayden/KeyCodes.hpp>
#include <rayden/KeyMods.hpp>

namespace rayden {

class KeyPressedEvent : public Event {
public:
  KeyPressedEvent(KeyCode key_code, KeyMod key_mod, bool repeated_or_not);

  static auto static_type() -> EventType;
  [[nodiscard]] auto name() const -> std::string override;
  [[nodiscard]] auto type() const -> EventType override;

  [[nodiscard]] auto key_code() const -> KeyCode;
  [[nodiscard]] auto key_mod() const -> KeyMod;
  [[nodiscard]] auto is_repeated() const -> bool;

private:
  KeyCode _key_code;
  KeyMod _key_mod;
  bool _repeated;
};

} // namespace rayden

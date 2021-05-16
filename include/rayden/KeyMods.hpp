#pragma once

#include <ostream>

namespace rayden {

using KeyMod = enum class Mod : uint16_t {
  None = 0,
  // from from glfw3.h
  Shift = 0x0001,
  Control = 0x0002,
  Alt = 0x0004,
  Super = 0x0008,
  CapsLock = 0x0010,
  NumLock = 0x0020
};

inline auto operator<<(std::ostream &os, KeyMod key_mod) -> std::ostream & {
  os << static_cast<int32_t>(key_mod);
  return os;
}

} // namespace rayden

#pragma once

#ifdef DEBUG
#define ENABLE_ASSERTS
#define OPENGL_DEBUG
#endif

#ifdef ENABLE_ASSERTS

#include "Logger.hpp"

template <typename... ARGLIST>
constexpr auto ASSERT(bool x, ARGLIST const &...args) {
  if (!(x)) {
    LOG_CRITICAL("[assert failure @ {}:{}] {}", __FILE__, __LINE__, args...);
  }
}

template <typename... ARGLIST>
constexpr auto REFUTE(bool x, ARGLIST const &...args) {
  {
    if (x) {
      LOG_CRITICAL("[refute failure @ {}:{}] {}", __FILE__, __LINE__, args...);
    }
  }
}

#else

#define ASSERT(x, ...)
#define REFUTE(x, ...)

#endif

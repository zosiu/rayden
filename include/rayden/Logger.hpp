#pragma once

#include <spdlog/spdlog.h>

namespace rayden {

class Logger {
public:
  static void init(const std::string &filename = "rayden.log");

  inline static auto instance() -> std::shared_ptr<spdlog::logger> & { return logger_instance; }

private:
  static std::shared_ptr<spdlog::logger> logger_instance;
};

} // namespace rayden

template <typename... ARGLIST>
constexpr auto LOG_TRACE(ARGLIST const &...args) {
  ::rayden::Logger::instance()->trace(args...);
}

template <typename... ARGLIST>
constexpr auto LOG_INFO(ARGLIST const &...args) {
  ::rayden::Logger::instance()->info(args...);
}

template <typename... ARGLIST>
constexpr auto LOG_WARN(ARGLIST const &...args) {
  ::rayden::Logger::instance()->warn(args...);
}

template <typename... ARGLIST>
constexpr auto LOG_ERROR(ARGLIST const &...args) {
  ::rayden::Logger::instance()->error(args...);
}

template <typename... ARGLIST>
constexpr auto LOG_CRITICAL(ARGLIST const &...args) {
  ::rayden::Logger::instance()->critical(args...);
}

#pragma once

#include <ostream>

namespace rayden {

enum class EventType { None = 0, WindowResized, WindowClosed, KeyPressed };

class Event {
public:
  template <typename T, typename H>
  static void dispatch(const Event &event, const H &handler) {
    if (event.type() == T::static_type()) {
      handler(static_cast<const T &>(event));
    }
  }

  [[nodiscard]] virtual auto name() const -> std::string = 0;
  [[nodiscard]] virtual auto type() const -> EventType = 0;

  Event() = default;
  Event(const Event &) = delete;
  auto operator=(const Event &) -> Event & = delete;
  Event(Event &&) = default;
  auto operator=(Event &&) -> Event & = default;
  virtual ~Event() = default;
};

inline auto operator<<(std::ostream &os, const Event &event) -> std::ostream & { return os << event.name(); }

} // namespace rayden

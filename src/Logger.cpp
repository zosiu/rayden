#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <rayden/Logger.hpp>

namespace rayden {

std::shared_ptr<spdlog::logger> Logger::logger_instance;

void Logger::init(const std::string &filename) {

  auto color_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  color_console_sink->set_pattern("%^[%T] [%n] %v%$");

  auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, true);
  file_sink->set_pattern("[%T] [%l] [%n] %v");

  std::array<spdlog::sink_ptr, 2> sinks{color_console_sink, file_sink};

  logger_instance = std::make_shared<spdlog::logger>("RAYDEN", begin(sinks), end(sinks));
  spdlog::register_logger(logger_instance);
  logger_instance->set_level(spdlog::level::trace);
  logger_instance->flush_on(spdlog::level::trace);
}

} // namespace rayden

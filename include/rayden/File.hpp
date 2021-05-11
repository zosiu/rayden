#pragma once

#include <string>

namespace rayden {

class File {
public:
  static auto contents(const char *filename) -> std::string;
};

} // namespace rayden

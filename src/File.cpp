#include <fstream>

#include <rayden/File.hpp>

namespace rayden {

auto File::contents(const char *filename) -> std::string {
  std::ifstream ifs(filename, std::ios::in);
  std::string file_contents((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
  ifs.close();
  return file_contents;
}

} // namespace rayden

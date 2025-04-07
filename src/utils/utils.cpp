#include "utils.h"
#include <sstream>

std::vector<std::string> parse_line(const std::string &line,
                                    const char delimiter) {
  std::vector<std::string> result;
  std::stringstream ss(line);
  std::string item;

  while (std::getline(ss, item, delimiter)) {
    result.push_back(item);
  }

  return result;
}

#include "commandline.hpp"

// C++ standard library
#include <algorithm>

bool hasOption(
    const int argc,
    const char* argv[],
    const std::string& option) {
  return std::find(argv, argv + argc, option) != argv + argc;
}

bool isNumber(
    const std::string& text) {
  try {
    std::stod(text);
  } catch(...) {
    return false;
  }
  
  return true;
}
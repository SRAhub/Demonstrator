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
    
std::string vectorToString(
    const arma::Row<double>& vector) {
  std::string string = std::to_string(vector(0));
  
  for (std::size_t n = 1; n < vector.n_elem; n++) {
    string += " " + std::to_string(vector(n));
  }
  
  return string;
}
    
arma::Row<double> stringToVector(
    std::string string) {
  arma::Row<double> vector;
  
  std::size_t n = 0;
  while (string.size() > 0) {
    vector(n++) = std::stod(string.substr(0, string.find(" ")));
    string = string.substr(string.find(" ") + 1);
  }
  
  return vector;
}
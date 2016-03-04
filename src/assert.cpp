#include "demonstrator_bits/assert.hpp"

// C++ standard library
#include <stdexcept>

namespace demo {
  void verify(
      const bool expression,
      const std::string& errorMessage) {
    if (!expression) {
      // Adds *Demonstrator - * to each error message, to better differentiate between messages from Mantella or other libraries.
      throw std::logic_error("Demonstrator - " + errorMessage);
    }
  }
}

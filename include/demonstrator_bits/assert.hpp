#pragma once

// C++ standard library
#include <string>

namespace demo {

  /**
   * If `expression` is false, throw an std::logic_error with `errorMessage`, prefixed with "Demonstrator - ".
   */
  void verify(
      const bool expression,
      const std::string& errorMessage);
}

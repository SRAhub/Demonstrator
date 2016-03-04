// Catch
#include <catch.hpp>
#include "catchExtension.hpp"

// Demonstrator
#include <demonstrator>

// The effectiveness of each global variable is tested within the test cases for the functions that depend on them.
SCENARIO("isVerbose", "[config][isVerbose]") {
  GIVEN("The default value is unchanged") {
    THEN("Return false") {
      CHECK(::demo::isVerbose == false);
    }
  }

  GIVEN("The default value was changed") {
    WHEN("Set it to true") {
      THEN("Return true") {
        ::demo::isVerbose = true;
        CHECK(::demo::isVerbose == true);
        ::demo::isVerbose = false;
      }
    }
  }
}

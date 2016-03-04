// Catch
#include <catch.hpp>
#include "catchExtension.hpp"

// Demonstrator
#include <demonstrator>

SCENARIO("verify", "[assert][verify]") {
  GIVEN("An expression and a error message") {
    const std::string& errorMessage = "Some message ...";
    CAPTURE(errorMessage);

    WHEN("The expression is false") {
      const bool expression = false;
      CAPTURE(expression);

      THEN("Throw a std::logic_error") {
        CHECK_THROWS_AS(demo::verify(expression, errorMessage), std::logic_error);
      }
    }

    WHEN("The expression is true") {
      const bool expression = true;
      CAPTURE(expression);

      THEN("Throw no exception") {
        CHECK_NOTHROW(demo::verify(expression, errorMessage));
      }
    }
  }
}

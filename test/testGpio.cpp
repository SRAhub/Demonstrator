// Catch
#include <catch.hpp>
#include "catchExtension.hpp"

// Demonstrator
#include <demonstrator>

// The effectiveness of each global variable is tested within the test cases for the functions that depend on them.
SCENARIO("Gpio.allocatePin", "[Gpio][Gpio.allocatePin]") {
  GPIO::allocatePin(3);
}

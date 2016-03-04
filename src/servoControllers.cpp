#include "demonstrator_bits/servoControllers.hpp"

// C++ standard library
#include <algorithm>
#include <chrono>
#include <ratio>
#include <thread>
// IWYU pragma: no_include <ext/alloc_traits.h>

// Demonstrator
#include "demonstrator_bits/assert.hpp"
#include "demonstrator_bits/gpio.hpp"
#include "demonstrator_bits/i2c.hpp"

namespace demo {
  ServoControllers::ServoControllers(
      std::vector<Pin> directionPins,
      const std::vector<unsigned int>& channels)
      : numberOfControllers_(directionPins.size()),
        directionPins_(std::move(directionPins)),
        channels_(channels) {}

  // TODO: Minimum speed is 0.2 or better 0.25! Map 0~1 to 0.25~1
  void ServoControllers::run(
      const std::vector<bool>& forwards,
      const std::vector<double>& speeds) {
    I2c i2c = Gpio::allocateI2c();
    unsigned int oldmode = i2c.get(0x00);
    i2c.set(0x00, (oldmode & 0x7F) | 0x10);
    i2c.set(0xFE, 5);
    i2c.set(0x00, oldmode);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    i2c.set(0x00, oldmode | 0xa1); // This sets the MODE1 register to turn on auto increment.

    for (std::size_t controller = 0; controller < numberOfControllers_; ++controller) {
      verify(0 <= speeds.at(controller) || speeds.at(controller) <= 1, "ServoControllers.run: All speeds must be within the interval [0, 1].");
    }

    for (std::size_t controller = 0; controller < numberOfControllers_; ++controller) {
      directionPins_.at(controller).set(forwards.at(controller) ? Pin::Digital::Low : Pin::Digital::High);

      i2c.set(0x06 + 4 * channels_.at(controller), 0);
      i2c.set(0x07 + 4 * channels_.at(controller), 0);
      i2c.set(0x08 + 4 * channels_.at(controller), static_cast<unsigned int>(4095.0 * speeds.at(controller)) & 0xFF);
      i2c.set(0x09 + 4 * channels_.at(controller), static_cast<unsigned int>(4095.0 * speeds.at(controller)) >> 8);
    }
  }

  void ServoControllers::stop() {
    // TODO run(false, 0);
  }
}

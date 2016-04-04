#include "demonstrator_bits/servoControllers.hpp"

// C++ standard library
#include <algorithm>
#include <chrono>
#include <ratio>
#include <thread>
// IWYU pragma: no_include <ext/alloc_traits.h>

namespace demo {
  ServoControllers::ServoControllers(
      std::vector<Pin> directionPins,
        I2c i2c,
      const std::vector<unsigned int>& channels)
      : numberOfControllers_(directionPins.size()),
        directionPins_(std::move(directionPins)),
        i2c_(std::move(i2c)),
        channels_(channels) {
    if (numberOfControllers_ == 0) {
      throw std::domain_error("ServoControllers: The number of controllers must be greater than 0.");
    } else if (directionPins_.size() != numberOfControllers_) {
      throw std::invalid_argument("ServoControllers: The number of direction pins must be equal to the number of controllers.");
    } if (channels_.size() != numberOfControllers_) {
      throw std::invalid_argument("ServoControllers: The number of channels must be equal to the number of controllers.");
    }
    for (const auto channel : channels_) {
      if (channel > 7) {
        throw std::domain_error("ServoControllers: All channels must be within [0, 7].");
      }
    }
  }

  // TODO: Minimum speed is 0.2 or better 0.25! Map 0~1 to 0.25~1
  void ServoControllers::run(
      const std::vector<double>& velocities) {
    if (velocities.size() != numberOfControllers_) {
      std::invalid_argument("ServoControllers: The number of velocities must be equal to the number of controllers.");
    }
    for (std::size_t n = 0; n < numberOfControllers_; ++n) {
      if(velocities.at(n) < -1.0 || velocities.at(n) > 1.0) {
        throw std::domain_error("ServoControllers.run: All velocities must be within [-1, 1].");
      }
    }
    
    unsigned int oldmode = i2c_.get(0x00);
    i2c_.set(0x00, (oldmode & 0x7F) | 0x10);
    i2c_.set(0xFE, 5);
    i2c_.set(0x00, oldmode);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    i2c_.set(0x00, oldmode | 0xa1); // This sets the MODE1 register to turn on auto increment.

    for (std::size_t n = 0; n < numberOfControllers_; ++n) {
      directionPins_.at(n).set(velocities.at(n) > 0.0 ? Pin::Digital::Low : Pin::Digital::High);

      i2c_.set(0x06 + 4 * channels_.at(n), 0);
      i2c_.set(0x07 + 4 * channels_.at(n), 0);
      i2c_.set(0x08 + 4 * channels_.at(n), static_cast<unsigned int>(4095.0 * velocities.at(n)) & 0xFF);
      i2c_.set(0x09 + 4 * channels_.at(n), static_cast<unsigned int>(4095.0 * velocities.at(n)) >> 8);
    }
  }

  void ServoControllers::stop() {
    run({});
  }
}

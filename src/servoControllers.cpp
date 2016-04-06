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
    
    unsigned int oldmode = i2c_.get(0b00000000);
    i2c_.set(0b00000000, (oldmode & 0b01111111) | 0b00010000);
    i2c_.set(0b11111110, 5);
    i2c_.set(0b00000000, oldmode);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    i2c_.set(0b00000000, oldmode | 0b10100001);
  }

  void ServoControllers::run(
        const std::vector<bool>& forwards,
        const std::vector<double>& speeds) {
    if (forwards.size() != numberOfControllers_) {
      std::invalid_argument("ServoControllers: The number of directions must be equal to the number of controllers.");
    } else if (speeds.size() != numberOfControllers_) {
      std::invalid_argument("ServoControllers: The number of speeds must be equal to the number of controllers.");
    }
    for (std::size_t n = 0; n < numberOfControllers_; ++n) {
      if(speeds.at(n) < 0.0 || speeds.at(n) > 1.0) {
        throw std::domain_error("ServoControllers.run: All speeds must be within [0, 1].");
      }
    }

    for (std::size_t n = 0; n < numberOfControllers_; ++n) {
      directionPins_.at(n).set(forwards.at(n) ? Pin::Digital::Low : Pin::Digital::High);

      i2c_.set(0b00000110 + 4 * channels_.at(n), 0);
      i2c_.set(0b00000111 + 4 * channels_.at(n), 0);
      i2c_.set(0b00001000 + 4 * channels_.at(n), static_cast<unsigned int>(4095.0 * speeds.at(n)) & 0b11111111);
      i2c_.set(0b00001001 + 4 * channels_.at(n), static_cast<unsigned int>(4095.0 * speeds.at(n)) >> 8);
    }
  }

  void ServoControllers::stop() {
    run(std::vector<bool>(numberOfControllers_, true), std::vector<double>(numberOfControllers_, 0.0));
  }
}

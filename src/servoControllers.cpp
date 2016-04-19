#include "demonstrator_bits/servoControllers.hpp"

// C++ standard library
#include <algorithm>
#include <chrono>
#include <cmath>
#include <ratio>
#include <stdexcept>
#include <thread>
// IWYU pragma: no_include <ext/alloc_traits.h>

namespace demo {
  ServoControllers::ServoControllers(
      std::vector<Pin>&& directionPins,
      I2c&& i2c,
      const std::vector<unsigned int>& channels,
      const double maximalSpeed)
      : numberOfControllers_(directionPins.size()),
        directionPins_(std::move(directionPins)),
        i2c_(std::move(i2c)),
        channels_(channels),
        maximalSpeed_(maximalSpeed) {
    if (numberOfControllers_ == 0) {
      throw std::domain_error("ServoControllers: The number of controllers must be greater than 0.");
    } else if (directionPins_.size() != numberOfControllers_) {
      throw std::invalid_argument("ServoControllers: The number of direction pins must be equal to the number of controllers.");
    }
    if (channels_.size() != numberOfControllers_) {
      throw std::invalid_argument("ServoControllers: The number of channels must be equal to the number of controllers.");
    }
    for (const auto channel : channels_) {
      if (channel > 7) {
        throw std::domain_error("ServoControllers: All channels must be within [0, 7].");
      }
    }

    i2c_.set(0x00, 0x00);
    unsigned int oldmode = i2c_.get(0x00);
    i2c_.set(0x00, (oldmode & 0x7F) | 0x10);
    i2c_.set(0xFE, 5);
    i2c_.set(0x00, oldmode);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    i2c_.set(0x00, oldmode | 0xa1);
  }

  ServoControllers::ServoControllers(
      ServoControllers&& servoControllers)
      : ServoControllers(std::move(servoControllers.directionPins_), std::move(servoControllers.i2c_), servoControllers.channels_, servoControllers.maximalSpeed_) {
  }

  ServoControllers& ServoControllers::operator=(
      ServoControllers&& servoControllers) {
    if (numberOfControllers_ != servoControllers.numberOfControllers_) {
      throw std::invalid_argument("ServoControllers.operator=: The number of controllers must be equal.");
    } else if (channels_ != servoControllers.channels_) {
      throw std::invalid_argument("ServoControllers.operator=: The channels must be equal.");
    } else if (std::abs(maximalSpeed_ -  servoControllers.maximalSpeed_) > 0) {
      throw std::invalid_argument("ServoControllers.operator=: The maximal speeds must equal.");
    } 

    directionPins_ = std::move(servoControllers.directionPins_);
    i2c_ = std::move(servoControllers.i2c_);

    return *this;
  }

  void ServoControllers::run(
      const std::vector<bool>& forwards,
      const arma::Row<double>& speeds) {
    if (forwards.size() != numberOfControllers_) {
      std::invalid_argument("ServoControllers: The number of directions must be equal to the number of controllers.");
    } else if (speeds.n_elem != numberOfControllers_) {
      std::invalid_argument("ServoControllers: The number of speeds must be equal to the number of controllers.");
    } else if (arma::any(speeds < 0.0) || arma::any(speeds > 1.0)) {
      throw std::domain_error("ServoControllers.run: All speeds must be within [0, 1].");
    }

    const arma::Row<double>& limitedSpeeds = arma::clamp(speeds, 0, maximalSpeed_);

    for (std::size_t n = 0; n < numberOfControllers_; ++n) {
      directionPins_.at(n).set(forwards.at(n) ? Pin::Digital::Low : Pin::Digital::High);

      i2c_.set(0x06 + 4 * channels_.at(n), 0);
      i2c_.set(0x07 + 4 * channels_.at(n), 0);
      i2c_.set(0x08 + 4 * channels_.at(n), static_cast<unsigned int>(4095.0 * limitedSpeeds(n)) & 0xFF);
      i2c_.set(0x09 + 4 * channels_.at(n), static_cast<unsigned int>(4095.0 * limitedSpeeds(n)) >> 8);
    }
  }

  void ServoControllers::stop() {
    run(std::vector<bool>(numberOfControllers_, true), arma::zeros<arma::Row<double>>(numberOfControllers_));
  }
}

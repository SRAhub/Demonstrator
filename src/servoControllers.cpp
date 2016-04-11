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

    const arma::Row<double>& limitedSpeeds = arma::clamp(speeds, minimalSpeed_, maximalSpeed_);

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

  void ServoControllers::setMinimalSpeed(
      const double minimalSpeed) {
    if (!std::isfinite(minimalSpeed)) {
      throw std::domain_error("ServoControllers.setMinimalSpeed: The minimal speed must be finite.");
    }

    minimalSpeed_ = minimalSpeed;
  }

  double ServoControllers::getMinimalSpeed() const {
    return minimalSpeed_;
  }

  void ServoControllers::setMaximalSpeed(
      const double maximalSpeed) {
    if (!std::isfinite(maximalSpeed)) {
      throw std::domain_error("ServoControllers.setMaximalSpeed: The maximal speed must be finite.");
    }

    maximalSpeed_ = maximalSpeed;
  }

  double ServoControllers::getMaximalSpeed() const {
    return maximalSpeed_;
  }
}

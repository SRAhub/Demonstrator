#include "demonstrator_bits/distanceIndicators.hpp"

// C++ standard library
#include <bitset>
#include <chrono>
#include <cmath>
#include <ratio>
#include <thread>
#include <utility>
// IWYU pragma: no_include <ext/alloc_traits.h>

namespace demo {
  DistanceIndicators::DistanceIndicators(
      Pin clockPin,
      std::vector<Pin> dataPins)
      : numberOfIndicators_(dataPins.size()),
        dataPins_(std::move(dataPins)),
        clockPin_(std::move(clockPin)) {
    if (numberOfIndicators_ == 0) {
      throw std::domain_error("DistanceIndicators: The number of indicators must be greater than 0.");
    } else if (dataPins_.size() != numberOfIndicators_) {
      throw std::invalid_argument("DistanceIndicators: The number of data pins must be equal to the number of indicators.");
    }
          
    clockPin_.set(Pin::Digital::Low);
  }

  void DistanceIndicators::setIndication(
      const std::vector<double>& distances) {
    if (distances.size() != numberOfIndicators_) {
      throw std::invalid_argument("DistanceIndicators.setIndication: The number of distances must be equal to the number of indicators.");
    } else if(minimalDistance_ >= warningDistance_) {
      throw std::logic_error("DistanceIndicators.setIndication: The warning distance must be greater than the minimal one.");
    } else if (warningDistance_ >= maximalDistance_) {
      throw std::logic_error("DistanceIndicators.setIndication: The maximal distance must be greater than the warning distance.");
    }

    // Calculate LED states. The lowest LED (red) is stored at index 0.
    std::vector<std::bitset<12>> states;
    for (std::size_t n = 0; n < distances.size(); ++n) {
      if (distances.at(n) <= warningDistance_) {
        states.push_back(0xff & (~0 << static_cast<unsigned int>(std::floor((1.0 - (distances.at(n) - warningDistance_) / (maximalDistance_ - warningDistance_)) * 8.0))));
      } else {
        states.push_back(300 & (~0 >> static_cast<unsigned int>(std::floor((distances.at(n) - minimalDistance_) / (warningDistance_ - minimalDistance_)))));
      }
    }

    // Send the `state` patterns to the LED bars. Because all MY9221 are connected to the same clock pin, we need to set all data pins ahead one bit at a time, then clock once. The exact order is:
    //  1. Send 16 bit command word 0x310 on all pins. This selects 16 bit grayscale code at 1001 Hz.
    //  2. For every bit in `currentState`, send 16x high (or 16x low, depending on the current bit value) on the respective pin.
    //  3. Conclude with 4 high/low toggles on the data pins while keeping the clock at a constant level.
    unsigned int clock = 0;

    // Step 1: Send command word 0x310. Iterate over the lower 16 bits of that
    // number and set the data pins accordingly.
    for (unsigned int i = 16; i > 0; --i) {
      for (auto& pin : dataPins_) {
        pin.set(0x310 & (1 << (i - 1)));
      }
      clockPin_.set(++clock % 2);
    }

    // Step 2: Send on/off state as 16 bit greyscale values.
    for (unsigned int led = 0; led < 12; ++led) {
      for (std::size_t bar = 0; bar < dataPins_.size(); ++bar) {
        dataPins_.at(bar).set(states.at(bar).test(led));
      }
      for (unsigned int i = 0; i < 16; ++i) {
        clockPin_.set(++clock % 2);
      }
    }

    // Step 3: Send latch command.
    std::this_thread::sleep_for(std::chrono::microseconds(220));
    for (auto& pin : dataPins_) {
      pin.set(Pin::Digital::Low);
      for (unsigned int i = 0; i < 4; ++i) {
        pin.set(Pin::Digital::High);
        pin.set(Pin::Digital::Low);
      }
    }
  }

  void DistanceIndicators::setMinimalDistance(
      const double minimalDistance) {
    if (!std::isfinite(minimalDistance)) {
      throw std::domain_error("DistanceIndicators.setMinimalDistance: The minimal distance must be finite.");
    }

    minimalDistance_ = minimalDistance;
  }

  double DistanceIndicators::getMinimalDistance() const {
    return minimalDistance_;
  }

  void DistanceIndicators::setWarningDistance(
      const double warningDistance) {
    if (!std::isfinite(warningDistance)) {
      throw std::domain_error("DistanceIndicators.setWarningDistance: The warning distance must be finite.");
    }

    warningDistance_ = warningDistance;
  }

  double DistanceIndicators::getWarningDistance() const {
    return warningDistance_;
  }

  void DistanceIndicators::setMaximalDistance(
      const double maximalDistance) {
    if (!std::isfinite(maximalDistance)) {
      throw std::domain_error("DistanceIndicators.setMaximalDistance: The maximal distance must be finite.");
    }

    maximalDistance_ = maximalDistance;
  }

  double DistanceIndicators::getMaximalDistance() const {
    return maximalDistance_;
  }
}

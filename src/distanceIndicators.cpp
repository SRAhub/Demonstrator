#include "demonstrator_bits/distanceIndicators.hpp"

// C++ standard library
#include <algorithm>
#include <bitset>
#include <chrono>
#include <cmath>
#include <ratio>
#include <stdexcept>
#include <thread>
// IWYU pragma: no_include <ext/alloc_traits.h>

namespace demo {
  DistanceIndicators::DistanceIndicators(
      Pin&& clockPin,
      std::vector<Pin>&& dataPins,
      const double minimalDistance,
      const double warningDistance,
      const double maximalDistance)
      : numberOfIndicators_(dataPins.size()),
        clockPin_(std::move(clockPin)),
        dataPins_(std::move(dataPins)),
        minimalDistance_(minimalDistance),
        warningDistance_(warningDistance),
        maximalDistance_(maximalDistance) {
    if (numberOfIndicators_ == 0) {
      throw std::domain_error("DistanceIndicators: The number of indicators must be greater than 0.");
    } else if (dataPins_.size() != numberOfIndicators_) {
      throw std::invalid_argument("DistanceIndicators: The number of data pins must be equal to the number of indicators.");
    }

    clockPin_.set(Pin::Digital::Low);
  }

  DistanceIndicators::DistanceIndicators(
      DistanceIndicators&& distanceIndicator)
      : DistanceIndicators(std::move(distanceIndicator.clockPin_), std::move(distanceIndicator.dataPins_), distanceIndicator.minimalDistance_, distanceIndicator.warningDistance_, distanceIndicator.maximalDistance_) {
  }

  DistanceIndicators& DistanceIndicators::operator=(
      DistanceIndicators&& distanceIndicator) {
    if (numberOfIndicators_ != distanceIndicator.numberOfIndicators_) {
      throw std::invalid_argument("DistanceIndicators.operator=: The number of indicators must be equal.");
    } else if (std::abs(minimalDistance_ -  distanceIndicator.minimalDistance_) > 0) {
      throw std::invalid_argument("DistanceIndicators.operator=: The minimal distances must be equal.");
    } else if (std::abs(warningDistance_ -  distanceIndicator.warningDistance_) > 0) {
      throw std::invalid_argument("DistanceIndicators.operator=: The warning distances values equal.");
    } else if (std::abs(maximalDistance_ -  distanceIndicator.maximalDistance_) > 0) {
      throw std::invalid_argument("DistanceIndicators.operator=: The maximal distances values equal.");
    } 
    
    clockPin_ = std::move(distanceIndicator.clockPin_);
    dataPins_ = std::move(distanceIndicator.dataPins_);

    return *this;
  }

  void DistanceIndicators::setIndication(
      const arma::Row<double>& distances) {
    if (distances.size() != numberOfIndicators_) {
      throw std::invalid_argument("DistanceIndicators.setIndication: The number of distances must be equal to the number of indicators.");
    } else if (minimalDistance_ >= warningDistance_) {
      throw std::logic_error("DistanceIndicators.setIndication: The warning distance must be greater than the minimal one.");
    } else if (warningDistance_ >= maximalDistance_) {
      throw std::logic_error("DistanceIndicators.setIndication: The maximal distance must be greater than the warning distance.");
    }

    const arma::Row<double>& limitedDistances = arma::clamp(distances, minimalDistance_, maximalDistance_);

    // Calculate LED states. The lowest LED (red) is stored at index 0.
    std::vector<std::bitset<12>> states;
    for (std::size_t n = 0; n < distances.n_elem; ++n) {
      const double distance = std::max(std::min(distances.at(n), maximalDistance_), minimalDistance_);

      if (distance > warningDistance_) {
        states.push_back(0b001111111100 & (static_cast<unsigned int>(std::pow(2.0, std::ceil(8.0 * (distance - warningDistance_) / (maximalDistance_ - warningDistance_))) - 1.0) << 2));
      } else {
        states.push_back(0b000000000011 & (2 + (distance <= minimalDistance_ ? 1 : 0)));
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
}

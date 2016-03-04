#include "demonstrator_bits/distanceIndicators.hpp"

// C++ standard library
#include <bitset>
#include <chrono>
#include <cmath>
#include <ratio>
#include <thread>
#include <utility>
// IWYU pragma: no_include <ext/alloc_traits.h>

// Demonstrator
#include "demonstrator_bits/assert.hpp"

namespace demo {
  DistanceIndicators::DistanceIndicators(
      Pin clockPin,
      std::vector<Pin> dataPins)
      : numberOfIndicators_(dataPins.size()),
        dataPins_(std::move(dataPins)),
        clockPin_(std::move(clockPin)) {
    clockPin_.set(Pin::Digital::Low);
  }

  void DistanceIndicators::setIndication(
      const std::vector<double>& distances) {
    verify(distances.size() == numberOfIndicators_, "DistanceIndicators.setIndication: The number of distances must be equal to the number of indicators.");
    // TODO verify ordering between all distance vectors

    std::vector<std::bitset<12>> states;
    for (std::size_t bar = 0; bar < distances.size(); ++bar) {
      if (distances.at(bar) <= warningDistances_.at(bar)) {
        states.push_back(0xff & (~0 << static_cast<unsigned int>(std::floor((1.0 - (distances.at(bar) - warningDistances_.at(bar)) / (maximalDistances_.at(bar) - warningDistances_.at(bar))) * 8.0))));
      } else {
        states.push_back(300 & (~0 >> static_cast<unsigned int>(std::floor((distances.at(bar) - minimalDistances_.at(bar)) / (warningDistances_.at(bar) - minimalDistances_.at(bar))))));
      }
    }

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

  void DistanceIndicators::setMinimalDistances(
      const std::vector<double>& minimalDistances) {
    verify(minimalDistances.size() == dataPins_.size(), "DistanceIndicators.setMinimalDistances: The number of minimal distances must be equal to the number of indicators.");

    minimalDistances_ = minimalDistances;
  }

  std::vector<double> DistanceIndicators::getMinimalDistances() const {
    return minimalDistances_;
  }

  void DistanceIndicators::setWarningDistances(
      const std::vector<double>& warningDistances) {
    verify(warningDistances.size() == dataPins_.size(), "DistanceIndicators.setWarningDistances: The number of warning distances must be equal to the number of indicators.");

    warningDistances_ = warningDistances;
  }

  std::vector<double> DistanceIndicators::getWarningDistances() const {
    return warningDistances_;
  }

  void DistanceIndicators::setMaximalDistances(
      const std::vector<double>& maximalDistances) {
    verify(maximalDistances.size() == dataPins_.size(), "DistanceIndicators.setMaximalDistances: The number of maximal distances must be equal to the number of indicators.");

    maximalDistances_ = maximalDistances;
  }

  std::vector<double> DistanceIndicators::getMaximalDistances() const {
    return maximalDistances_;
  }
}

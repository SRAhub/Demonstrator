#pragma once

// C++ standard library
#include <cstddef>
#include <vector>

// Demonstrator
#include "demonstrator_bits/pin.hpp"

namespace demo {
  /**
   * Part of `demo::LinearActuators`.
   *
   * This class issues commands to an array of [PRODUCT NAME HERE][1] linear actuators.
   *
   * [1]: http://INSERT-PRODUCT-PAGE-HERE
   */
  class ServoControllers {
   public:
    std::size_t numberOfControllers_;

    explicit ServoControllers(
        std::vector<Pin> directionPins,
        const std::vector<unsigned int>& channels);

    void run(
        const std::vector<bool>& forwards,
        const std::vector<double>& speeds);

    void stop();

   protected:
    std::vector<Pin> directionPins_;
    const std::vector<unsigned int> channels_;
  };
}

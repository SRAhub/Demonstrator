#pragma once

// C++ standard library
#include <cstddef>
#include <vector>

// Demonstrator
#include "demonstrator_bits/pin.hpp"

namespace demo {
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

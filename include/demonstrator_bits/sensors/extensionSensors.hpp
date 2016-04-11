#pragma once

// C++ standard library
#include <vector>

// Armadillo
#include <armadillo>

// Demonstrator
#include "demonstrator_bits/sensors.hpp"
#include "demonstrator_bits/spi.hpp"

namespace demo {

  /**
   * Part of `demo::LinearActuators`.
   *
   * Represents an array of extension sensors that are attached to [PRODUCT NAME HERE][1] linear actuators. Sensors can be queried for its current extension through SPI.
   *
   * [1]: http://INSERT-PRODUCT-PAGE-HERE
   */
  class ExtensionSensors : public Sensors {
   public:
    explicit ExtensionSensors(
        Spi spi,
        const std::vector<unsigned int>& channels);

   protected:
    Spi spi_;
    const std::vector<unsigned int> channels_;

    arma::Row<double> measureImplementation() override;
  };
}

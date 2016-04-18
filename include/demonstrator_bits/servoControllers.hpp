#pragma once

// C++ standard library
#include <cstddef>
#include <vector>

// Armadillo
#include <armadillo>

// Demonstrator
#include "demonstrator_bits/i2c.hpp"
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
        std::vector<Pin>&& directionPins,
        I2c&& i2c,
        const std::vector<unsigned int>& channels);

    explicit ServoControllers(
        ServoControllers&& servoControllers);

    ServoControllers& operator=(
        ServoControllers&& servoControllers);

    ServoControllers(ServoControllers&) = delete;
    ServoControllers& operator=(ServoControllers&) = delete;

    void run(
        const std::vector<bool>& forwards,
        const arma::Row<double>& speeds);

    void stop();

    void setMinimalSpeed(
        const double minimalSpeed);
    double getMinimalSpeed() const;

    void setMaximalSpeed(
        const double maximalSpeed);
    double getMaximalSpeed() const;

   protected:
    std::vector<Pin> directionPins_;

    I2c i2c_;
    const std::vector<unsigned int> channels_;

    /**
     * Minimal/maximal allowed extension speed of each actuator, in cm/second.
     */
    double minimalSpeed_;
    double maximalSpeed_;
  };
}

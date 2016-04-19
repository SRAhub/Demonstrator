#pragma once

// C++ standard library
#include <atomic>
#include <cstddef>
#include <thread>

// Armadillo
#include <armadillo>

// Demonstrator
#include "demonstrator_bits/sensors/extensionSensors.hpp"
#include "demonstrator_bits/servoControllers.hpp"

namespace demo {

  /**
   * Represents an array of [INSERT PRODUCT NAME HERE][1] linear actuators.
   *
   * Each actuator can be instructed to approach a certain extension, and queried for its current extension. These features are implemented in the classes `ServoControllers` and `ExtensionSensors` and combined in this one.
   *
   * [1]: http://INSERT-PRODUCT-PAGE-HERE
   */
  class LinearActuators {
   public:
    const std::size_t numberOfActuators_;

    explicit LinearActuators(
        ServoControllers&& servoControllers,
        ExtensionSensors&& extensionSensors);

    explicit LinearActuators(
        LinearActuators&& linearActuators);

    LinearActuators& operator=(
        LinearActuators&& linearActuators);

    LinearActuators(LinearActuators&) = delete;
    LinearActuators& operator=(LinearActuators&) = delete;

    /**
     * Let each actuator approach its new position. This method blocks until all actuators have reached their extension!
     *
     * Both `extensions` and `speeds` need to be between in range [0, 1]. Both values refer to a percentage of their respective intervals: [minimalExtension, maximalExtension] and [minimalSpeeds, maximalSpeeds].
     */
    void setExtensions(
        const arma::Row<double>& extensions,
        const arma::Row<double>& maximalSpeeds);

    arma::Row<double> getExtensions();

    void setMinimalExtension(
        const double minimalExtension);
    double getMinimalExtension() const;

    void setMaximalExtension(
        const double maximalExtension);
    double getMaximalExtension() const;

    void setMaximalExtensionDeviation(
        const double maximalExtensionDeviation);
    double getMaximalExtensionDeviation() const;

    ServoControllers& getServoControllers();
    ExtensionSensors& getExtensionSensors();

   protected:
    ServoControllers servoControllers_;
    ExtensionSensors extensionSensors_;

    /**
     * Minimal/maximal allowed extension of each actuator, in cm.
     *
     * Refers only to the movement margin, not the total length of the actuator.
     */
    double minimalExtension_;
    double maximalExtension_;

    double maximalExtensionDeviation_;

    std::atomic<bool> killReachExtensionThread_;
    std::thread reachExtensionThread_;

    void reachExtension(
        const arma::Row<double>& extensions,
        const arma::Row<double>& maximalSpeeds);
  };
}

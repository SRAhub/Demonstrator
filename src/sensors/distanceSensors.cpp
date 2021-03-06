#include "demonstrator_bits/sensors/distanceSensors.hpp"

// C++ standard library
#include <algorithm>
#include <cstddef>
#include <chrono>
#include <ratio>
#include <thread>
// IWYU pragma: no_include <ext/alloc_traits.h>

namespace demo {
  DistanceSensors::DistanceSensors(
      std::vector<Pin>&& pins,
      const double minimalDistance,
      const double maximalDistance)
      : Sensors(pins.size(), minimalDistance, maximalDistance),
        pins_(std::move(pins)) {
    for (std::size_t n = 0; n < numberOfSensors_; ++n) {
      pins_.at(n).set(Pin::Digital::Low);
      std::this_thread::sleep_for(std::chrono::microseconds(2));
    }
  }

  DistanceSensors::DistanceSensors(
      DistanceSensors&& distanceSensors)
      : DistanceSensors(std::move(distanceSensors.pins_), distanceSensors.minimalMeasurableValue_, distanceSensors.maximalMeasurableValue_) {
    setMeasurementCorrections(distanceSensors.measurementCorrections_);
    setNumberOfSamplesPerMeasurment(distanceSensors.numberOfSamplesPerMeasuement_);
  }

  DistanceSensors& DistanceSensors::operator=(
      DistanceSensors&& distanceSensors) {
    pins_ = std::move(distanceSensors.pins_);
    
    Sensors::operator=(std::move(distanceSensors));
    return *this;
  }

  arma::Row<double> DistanceSensors::measureImplementation() {
    /*
     * 1. Send 10us pulse trigger.
     * 2. Wait for echo signal. Abort after 250 milliseconds because TODO EXPLAIN THIS
     * 3. Calculate the distance in meter using the equation in the data sheet: distance [cm] = us/58
     * 4. Reset the pin to its initial state for the next measurement.
     */

    arma::Row<double> distances(numberOfSensors_);
    for (std::size_t n = 0; n < numberOfSensors_; ++n) {
      pins_.at(n).set(Pin::Digital::High);
      std::this_thread::sleep_for(std::chrono::microseconds(10));
      pins_.at(n).set(Pin::Digital::Low);

      std::this_thread::sleep_for(std::chrono::microseconds(20));
      pins_.at(n).waitForSignalEdge(std::chrono::milliseconds(50));

      // TODO: If we can set `maximalMeasurableDistance_`, we need to adjust the wait time to a dynamic value, too.
      distances(n) = pins_.at(n).waitForSignalEdge(std::chrono::milliseconds(2)).count() / 5800.0;
      pins_.at(n).set(Pin::Digital::Low);
    }

    return distances;
  }
}

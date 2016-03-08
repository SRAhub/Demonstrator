#include "demonstrator_bits/distanceSensors.hpp"

// C++ standard library
#include <algorithm>
#include <chrono>
#include <ratio>
#include <thread>
// IWYU pragma: no_include <ext/alloc_traits.h>
// IWYU pragma: no_include <ext/new_allocator.h>

// Demonstrator
#include "demonstrator_bits/assert.hpp"

namespace demo {
  DistanceSensor::DistanceSensor(
      std::vector<Pin> pins,
      const std::vector<double>& sensorAdjustments)
      : numberOfSensors_(pins.size()),
        pins_(std::move(pins)),
        oldestPreviousDistance_(numberOfSensors_, 0) {
    setSensorAdjustments(sensorAdjustments);

    for (std::size_t sensor = 0; sensor < numberOfSensors_; ++sensor) {
      pins_.at(sensor).set(Pin::Digital::Low);
    }
  }

  std::vector<double> DistanceSensor::getDistances() {
    std::vector<double> distances;
    for (std::size_t sensor = 0; sensor < numberOfSensors_; ++sensor) {
      // Step 1: Send 10µs pulse trigger.
      pins_.at(sensor).set(Pin::Digital::High);
      std::this_thread::sleep_for(std::chrono::microseconds(10));
      pins_.at(sensor).set(Pin::Digital::Low);

      // Step 2: Wait for echo signal. Abort after 250 milliseconds because TODO EXPLAIN THIS
      pins_.at(sensor).waitForSignalEdge(std::chrono::milliseconds(250));

      // TODO: If we can set `maximalMeasureableDistance_`, we need to adjust the wait time to a dynamic value, too.
      auto echoDuration = std::chrono::duration_cast<std::chrono::microseconds>(pins_.at(sensor).waitForSignalEdge(std::chrono::milliseconds(2)));

      // Reset the pin to its initial state for the next measurement.
      pins_.at(sensor).set(Pin::Digital::Low);

      // Step 3: Calculate the distance in meter using the equation in the data
      //         sheet: distance [cm] = µs/58
      double distance = echoDuration.count() / 58.0 / 100.0 * sensorAdjustments_.at(sensor);

      previousDistances_.at(sensor)(oldestPreviousDistance_.at(sensor)) = distance;
      oldestPreviousDistance_.at(sensor) = ++oldestPreviousDistance_.at(sensor) % 5;

      distances.push_back(std::max(minimalMeasureableDistance_.at(sensor), std::min(maximalMeasureableDistance_.at(sensor), arma::median(previousDistances_.at(sensor)))));
    }

    return distances;
  }

  void DistanceSensor::setMinimalMeasureableDistance(
      const std::vector<double>& minimalMeasureableDistance) {
    verify(minimalMeasureableDistance.size() == numberOfSensors_, "DistanceSensor.setMinimalMeasureableDistance: The number of minimal measurable distances must be equal to the number of sensors.");

    minimalMeasureableDistance_ = minimalMeasureableDistance;
  }

  std::vector<double> DistanceSensor::getMinimalMeasureableDistance() const {
    return minimalMeasureableDistance_;
  }

  void DistanceSensor::setMaximalMeasureableDistance(
      const std::vector<double>& maximalMeasureableDistance) {
    verify(maximalMeasureableDistance.size() == numberOfSensors_, "DistanceSensor.setMaximalMeasureableDistance: The number of maximal measurable distances must be equal to the number of sensors.");

    maximalMeasureableDistance_ = maximalMeasureableDistance;
  }

  std::vector<double> DistanceSensor::getMaximalMeasureableDistance() const {
    return maximalMeasureableDistance_;
  }

  void DistanceSensor::setSensorAdjustments(
      const std::vector<double>& sensorAdjustments) {
    verify(sensorAdjustments.size() == numberOfSensors_, "DistanceSensor.setSensorAdjustments: The number of sensor adjustments must be equal to the number of sensors.");

    sensorAdjustments_ = sensorAdjustments;
  }

  std::vector<double> DistanceSensor::getSensorAdjustments() const {
    return sensorAdjustments_;
  }
}

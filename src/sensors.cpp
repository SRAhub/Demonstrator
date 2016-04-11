#include "demonstrator_bits/sensors.hpp"

// C++ standard library
#include <cmath>
#include <stdexcept>

namespace demo {
  Sensors::Sensors(
      const std::size_t numberOfSensors)
      : numberOfSensors_(numberOfSensors) {
    if (numberOfSensors_ == 0) {
      throw std::domain_error("Sensors: The number of sensors must be greater than 0.");
    }

    setNumberOfSamplesPerMeasurment(1);
  }

  arma::Row<double> Sensors::measure() {
    if (maximalMeasurableValue_ < minimalMeasurableValue_) {
      throw std::logic_error("Sensors.measure: The maximal measurable value must be grater than or equal to the minimal one.");
    }

    arma::Mat<double> measurements(numberOfSamplesPerMeasuement_, numberOfSensors_);
    for (std::size_t n = 0; n < numberOfSamplesPerMeasuement_; ++n) {
      measurements.row(n) = arma::Row<double>(measureImplementation());
    }

    // Correction

    return arma::median(static_cast<arma::Mat<double>>(arma::clamp(measurements, minimalMeasurableValue_, maximalMeasurableValue_)));
  }

  void Sensors::setMinimalMeasurableValue(
      const double minimalMeasurableValue) {
    if (!std::isfinite(minimalMeasurableValue)) {
      throw std::domain_error("Sensors.setMinimalMeasurableValue: The minimal measurable value must be finite.");
    }

    minimalMeasurableValue_ = minimalMeasurableValue;
  }

  double Sensors::getMinimalMeasurableValue() const {
    return minimalMeasurableValue_;
  }

  void Sensors::setMaximalMeasurableValue(
      const double maximalMeasurableValue) {
    if (!std::isfinite(maximalMeasurableValue)) {
      throw std::domain_error("Sensors.setMaximalMeasurableValue: The maximal measurable value must be finite.");
    }

    maximalMeasurableValue_ = maximalMeasurableValue;
  }

  double Sensors::getMaximalMeasurableValue() const {
    return maximalMeasurableValue_;
  }

  void Sensors::setMeasurementCorrections(
      const arma::Mat<double>& measurementCorrections) {
    if (measurementCorrections.size() != numberOfSensors_) {
      throw std::domain_error("Sensors.setMeasurementCorrections: The number of measurement corrections must be equal to the number of sensors.");
    } else if (!measurementCorrections.is_finite()) {
      throw std::domain_error("Sensors.setMeasurementCorrections: All measurement corrections must be finite.");
    }

    measurementCorrections_ = measurementCorrections;
  }

  arma::Mat<double> Sensors::getMeasurementCorrections() const {
    return measurementCorrections_;
  }

  void Sensors::setNumberOfSamplesPerMeasurment(
      const std::size_t numberOfSamplesPerMeasuement) {
    if (numberOfSamplesPerMeasuement == 0) {
      throw std::domain_error("Sensors.setNumberOfSamplesPerMeasurment: The number of samples per measurement must be greater than 0.");
    }

    numberOfSamplesPerMeasuement_ = numberOfSamplesPerMeasuement;
  }

  std::size_t Sensors::getNumberOfSamplesPerMeasuement() const {
    return numberOfSamplesPerMeasuement_;
  }
}

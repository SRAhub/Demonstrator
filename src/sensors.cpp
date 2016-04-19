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

  Sensors::Sensors(
      Sensors&& sensors)
      : Sensors(sensors.numberOfSensors_) {
  }

  Sensors& Sensors::operator=(
      Sensors&& sensors) {
    if (numberOfSensors_ != sensors.numberOfSensors_) {
      throw std::invalid_argument("Sensors.operator=: The number of sensors must be equal.");
    }

    return *this;
  }

  arma::Row<double> Sensors::measure() {
    if (maximalMeasurableValue_ < minimalMeasurableValue_) {
      throw std::logic_error("Sensors.measure: The maximal measurable value must be grater than or equal to the minimal one.");
    }

    arma::Mat<double> measurements(numberOfSamplesPerMeasuement_, numberOfSensors_);
    for (std::size_t n = 0; n < numberOfSamplesPerMeasuement_; ++n) {
      arma::Row<double> measurment = measureImplementation();
      
      for (auto& measurmentEntry : measurment) {
        const double measurementIndex = std::min(std::max(measurmentEntry, minimalMeasurableValue_), maximalMeasurableValue_) / measurementCorrections_.n_elem;
        
        const std::size_t lowerMeasurementIndex = static_cast<std::size_t>(std::floor(measurementIndex));
        const std::size_t upperMeasurementIndex = static_cast<std::size_t>(std::ceil(measurementIndex));
        
        if (lowerMeasurementIndex == upperMeasurementIndex) {
          measurmentEntry = measurementCorrections_(lowerMeasurementIndex);
        } else {
          const std::size_t measurementIndexDifferenz = lowerMeasurementIndex - measurementIndex;
          measurmentEntry =  (1 - measurementIndexDifferenz) * measurementCorrections_(lowerMeasurementIndex) +  measurementIndexDifferenz * measurementCorrections_(upperMeasurementIndex);
        }
      }
      
      measurements.row(n) = measurment;
    }

    return arma::median(measurements);
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

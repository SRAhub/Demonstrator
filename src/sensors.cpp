#include "demonstrator_bits/sensors.hpp"

// C++ standard library
#include <cmath>
#include <stdexcept>

// Mantella
#include <mantella>

namespace demo {
  Sensors::Sensors(
      const std::size_t numberOfSensors,
      const double minimalMeasurableValue,
      const double maximalMeasurableValue)
      : numberOfSensors_(numberOfSensors),
        minimalMeasurableValue_(minimalMeasurableValue), 
        maximalMeasurableValue_(maximalMeasurableValue) {
    if (numberOfSensors_ == 0) {
      throw std::domain_error("Sensors: The number of sensors must be greater than 0.");
    } else if (!std::isfinite(minimalMeasurableValue_)) {
      throw std::domain_error("Sensors: The minimal measurable value must be finite.");
    } else if (!std::isfinite(maximalMeasurableValue_)) {
      throw std::domain_error("Sensors: The maximal measurable value must be finite.");
    } else if (maximalMeasurableValue_ < minimalMeasurableValue_) {
      throw std::logic_error("Sensors: The maximal measurable value must be grater than or equal to the minimal one.");
    }

    setMeasurementCorrections(arma::join_cols(arma::zeros<arma::Row<double>>(numberOfSensors_) + minimalMeasurableValue_, arma::zeros<arma::Row<double>>(numberOfSensors_) + maximalMeasurableValue_));
    setNumberOfSamplesPerMeasurment(1);
  }

  Sensors::Sensors(
      Sensors&& sensors)
      : Sensors(sensors.numberOfSensors_, sensors.minimalMeasurableValue_, sensors.maximalMeasurableValue_) {
    setMeasurementCorrections(sensors.measurementCorrections_);
    setNumberOfSamplesPerMeasurment(sensors.numberOfSamplesPerMeasuement_);
  }

  Sensors& Sensors::operator=(
      Sensors&& sensors) {
    if (numberOfSensors_ != sensors.numberOfSensors_) {
      throw std::invalid_argument("Sensors.operator=: The number of sensors must be equal.");
    } else if (std::abs(minimalMeasurableValue_ -  sensors.minimalMeasurableValue_) > 0) {
      throw std::invalid_argument("Sensors.operator=: The minimal measurable values must be equal.");
    } else if (std::abs(maximalMeasurableValue_ - sensors.maximalMeasurableValue_) > 0) {
      throw std::invalid_argument("Sensors.operator=: The maximal measurable values must be equal.");
    }
    
    setMeasurementCorrections(sensors.measurementCorrections_);
    setNumberOfSamplesPerMeasurment(sensors.numberOfSamplesPerMeasuement_);

    return *this;
  }

  arma::Row<double> Sensors::measure() {
    arma::Mat<double> measurements(numberOfSamplesPerMeasuement_, numberOfSensors_);
    for (std::size_t n = 0; n < numberOfSamplesPerMeasuement_; ++n) {
      measurements.row(n) = arma::clamp(measureImplementation(), minimalMeasurableValue_, maximalMeasurableValue_);
    }

    const arma::Row<double>& measurementIndices = (arma::median(measurements) - minimalMeasurableValue_) * (measurementCorrections_.n_rows - 1) / (maximalMeasurableValue_ - minimalMeasurableValue_);
        
    const arma::Row<arma::uword>& lowerMeasurementIndices = arma::conv_to<arma::Row<arma::uword>>::from(arma::floor(measurementIndices));
    const arma::Row<arma::uword>& upperMeasurementIndices = arma::conv_to<arma::Row<arma::uword>>::from(arma::ceil(measurementIndices));
    const arma::Row<double>& measurementIndicesDifferenz = measurementIndices - lowerMeasurementIndices;
    
    arma::Row<double> measuredValues(numberOfSensors_);
    for (std::size_t n = 0; n < numberOfSensors_; ++n) {
      measuredValues(n) = (1.0 - measurementIndicesDifferenz(n)) * measurementCorrections_(lowerMeasurementIndices(n), n) +  measurementIndicesDifferenz(n) * measurementCorrections_(upperMeasurementIndices(n), n);
    }
    
    return measuredValues;
  }

  void Sensors::setMeasurementCorrections(
      const arma::Mat<double>& measurementCorrections) {
    if (measurementCorrections.n_cols != numberOfSensors_) {
      throw std::domain_error("Sensors.setMeasurementCorrections: The number of measurement corrections columns must be equal to the number of sensors.");
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

#pragma once

// C++ standard library
#include <cstddef>

// Armadillo
#include <armadillo>

namespace demo {
  class Sensors {
   public:
    const std::size_t numberOfSensors_;

    explicit Sensors(
        const std::size_t numberOfSensors);

    explicit Sensors(
        Sensors&& sensors);

    Sensors& operator=(
        Sensors&& sensors);

    Sensors(Sensors&) = delete;
    Sensors& operator=(Sensors&) = delete;

    arma::Row<double> measure();

    void setMinimalMeasurableValue(
        const double minimalMeasurableValue);
    double getMinimalMeasurableValue() const;

    void setMaximalMeasurableValue(
        const double maximalMeasurableValue);
    double getMaximalMeasurableValue() const;

    void setMeasurementCorrections(
        const arma::Mat<double>& measurementCorrections);
    arma::Mat<double> getMeasurementCorrections() const;

    void setNumberOfSamplesPerMeasurment(
        const std::size_t numberOfSamplesPerMeasuement);
    std::size_t getNumberOfSamplesPerMeasuement() const;

   protected:
    double minimalMeasurableValue_;
    double maximalMeasurableValue_;

    arma::Mat<double> measurementCorrections_;
    std::size_t numberOfSamplesPerMeasuement_;

    virtual arma::Row<double> measureImplementation() = 0;
  };
}
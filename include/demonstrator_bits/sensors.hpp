#pragma once

// C++ standard library
#include <vector>

// Armadillo
#include <armadillo>

namespace demo {
  class Sensors {
   public:
    const std::size_t numberOfSensors_;
    
    Sensors(
        const std::size_t numberOfSensors);
    
    std::vector<double> measure();
   
    void setMinimalMeasurableValue(
        const double minimalMeasurableValue);
    double getMinimalMeasurableValue() const;

    void setMaximalMeasurableValue(
        const double maximalMeasurableValue);
    double getMaximalMeasurableValue() const;
    
    void setMeasurementCorrections(
        const std::vector<arma::Col<double>>& measurementCorrections);
    std::vector<arma::Col<double>> getMeasurementCorrections() const;

    void setNumberOfSamplesPerMeasurment(
        const std::size_t numberOfSamplesPerMeasuement);
    std::size_t getNumberOfSamplesPerMeasuement() const;
      
   protected:
    double minimalMeasurableValue_;
    double maximalMeasurableValue_;

    std::vector<arma::Col<double>> measurementCorrections_;
    std::size_t numberOfSamplesPerMeasuement_;
    
    virtual std::vector<double> measureImplementation() = 0;
  };
}
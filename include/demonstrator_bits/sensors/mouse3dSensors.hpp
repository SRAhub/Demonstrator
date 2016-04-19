#pragma once

// C++ standard library
#include <atomic>
#include <thread>

// Armadillo
#include <armadillo>

// Demonstrator
#include "demonstrator_bits/sensors.hpp"

namespace demo {
  class Mouse3dSensors : public Sensors {
   public:
    explicit Mouse3dSensors(
        const double minimalDisplacement,
        const double maximalDisplacement);

    explicit Mouse3dSensors(
        Mouse3dSensors&& mouse3d);

    Mouse3dSensors& operator=(
        Mouse3dSensors&& mouse3d);

    Mouse3dSensors(Mouse3dSensors&) = delete;
    Mouse3dSensors& operator=(Mouse3dSensors&) = delete;
    
    void runAsynchronous();

    ~Mouse3dSensors();

   protected:
    int fileDescriptor_;

    arma::Row<double>::fixed<8> displacements_;

    std::atomic<bool> killContinuousMeasurementThread_;
    std::thread continuousMeasurementThread_;

    arma::Row<double> measureImplementation() override;
    
    void asynchronousMeasurement();
  };
}

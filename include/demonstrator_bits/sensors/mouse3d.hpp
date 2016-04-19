#pragma once

// C++ standard library
#include <atomic>
#include <thread>

// Armadillo
#include <armadillo>

// Demonstrator
#include "demonstrator_bits/sensors.hpp"

namespace demo {
  class Mouse3d : public Sensors {
   public:
    explicit Mouse3d();

    explicit Mouse3d(
        Mouse3d&& mouse3d);

    Mouse3d& operator=(
        Mouse3d&& mouse3d);

    Mouse3d(Mouse3d&) = delete;
    Mouse3d& operator=(Mouse3d&) = delete;
    
    void runAsynchronous();

    ~Mouse3d();

   protected:
    int fileDescriptor_;

    arma::Row<double>::fixed<8> displacements_;

    std::atomic<bool> killContinuousMeasurementThread_;
    std::thread continuousMeasurementThread_;

    arma::Row<double> measureImplementation() override;
    
    void asynchronousMeasurement();
  };
}

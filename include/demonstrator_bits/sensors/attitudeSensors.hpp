#pragma once

// Unix library
#include <termios.h>

// Armadillo
#include <armadillo>

// C++ standard library
#include <atomic>
#include <thread>

// Demonstrator
#include "demonstrator_bits/sensors.hpp"
#include "demonstrator_bits/uart.hpp"

namespace demo {
  /**
   * Represents a single 9DoF Razor IMU[1].
   *
   * [1]: https://www.sparkfun.com/products/10736
   */
  class AttitudeSensors : public Sensors {
   public:
    explicit AttitudeSensors(
        Uart&& uart,
        const double minimalAttitude,
        const double maximalAttitude);

    explicit AttitudeSensors(
        AttitudeSensors&& attitudeSensors);

    AttitudeSensors& operator=(
        AttitudeSensors&& attitudeSensors);

    AttitudeSensors(AttitudeSensors&) = delete;
    AttitudeSensors& operator=(AttitudeSensors&) = delete;

    void runAsynchronous();
    
    void reset();

    ~AttitudeSensors();

   protected:
    Uart uart_;

    int fileDescriptor_;
    struct termios newSerial_;
    struct termios oldSerial_;

    arma::Row<double>::fixed<3> attitudes_;

    std::atomic<bool> killContinuousMeasurementThread_;
    std::thread continuousMeasurementThread_;

    arma::Row<double> measureImplementation() override;
    
    void asynchronousMeasurement();
  };
}

#pragma once

// Unix library
#include <termios.h>

// Armadillo
#include <armadillo>

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
    AttitudeSensors(
        Uart uart);

    void reset();

    ~AttitudeSensors();

   protected:
    Uart uart_;

    int fileDescriptor_;
    struct termios newSerial_;
    struct termios oldSerial_;

    arma::Row<double> measureImplementation() override;
  };
}

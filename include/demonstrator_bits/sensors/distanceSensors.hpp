#pragma once

// C++ standard library
#include <vector>

// Armadillo
#include <armadillo>

// Demonstrator
#include "demonstrator_bits/pin.hpp"
#include "demonstrator_bits/sensors.hpp"

/**
 * This class represents an array of HC-SR04[1]. On the demonstrator, this device is used to measure the distance between the demonstrator and other obstacles.
 *
 * Notice that a single raspberry pi pin is connected to both `trigger` and `echo` pins on the device:
 *
 *   [pi]--+-----[trigger]-----+--[Raspberry Pi]
 *         |                   |
 *         +--100 Ohm--[echo]--+
 *
 * [1]: http://www.micropik.com/PDF/HCSR04.pdf
 */
namespace demo {
  class DistanceSensors : public Sensors {
   public:
    DistanceSensors(
        std::vector<Pin> pins);

   protected:
    std::vector<Pin> pins_;

    arma::Row<double> measureImplementation() override;
  };
}

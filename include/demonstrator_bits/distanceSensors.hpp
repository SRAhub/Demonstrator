#pragma once

// C++ standard library
#include <cstddef>
#include <vector>

// Armadillo
#include <armadillo>

// Demonstrator
#include "demonstrator_bits/pin.hpp"

/**
 * This class represents an array of HC-SR04[1]. On the demonstrator, this device is used to measure the distance between the demonstrator and other obstacles.
 *
 * Notice that a single raspberry pi pin is connected to both `trigger` and `echo` pins on the device:
 *
 *   [pi]--+-----------[trigger]
 *         |
 *         +--100 Ohm--[echo]
 *
 * [1]: http://www.micropik.com/PDF/HCSR04.pdf
 */
namespace demo {
  class DistanceSensor {
   public:
    /**
     * The number of distance sensors that are managed.
     */
    const std::size_t numberOfSensors_;

    DistanceSensor(
        std::vector<Pin> pins,
        const std::vector<double>& sensorAdjustments);

    /**
     * Sends a trigger signal on `pins_` to the HC-SR04, then awaits the response on the same pin. Returns per distance sensor the median over the current measurement together with the last 5 ones.
     */
    std::vector<double> getDistances();

    void setMinimalMeasureableDistance(
        const std::vector<double>& minimalMeasureableDistance);
    std::vector<double> getMinimalMeasureableDistance() const;

    void setMaximalMeasureableDistance(
        const std::vector<double>& maximalMeasureableDistance);
    std::vector<double> getMaximalMeasureableDistance() const;

    void setSensorAdjustments(
        const std::vector<double>& sensorAdjustments);
    std::vector<double> getSensorAdjustments() const;

   protected:
    /**
     * This pin is connected to both `trigger` and `echo` pins. In between measurements (and therefore at the start of ever measurement) its value is `Low`.
     */
    std::vector<Pin> pins_;

    /**
     * If the device measures a smaller distance than this value, this value is used instead.
     */
    std::vector<double> minimalMeasureableDistance_;

    /**
     * If the device measures a greater distance than this value, this value is used instead.
     */
    std::vector<double> maximalMeasureableDistance_;

    /**
     * Every measurement is multiplied with this number before it is returned. Use it to adjust results to your environment.
     */
    std::vector<double> sensorAdjustments_;

    /**
     * The 5 last measured distances.
     */
    std::vector<arma::Col<double>::fixed<5>> previousDistances_;

    /**
     * Index into `previousDistances_` at the value that should be replaced next, to mimic FIFO behaviour.
     */
    std::vector<std::size_t> oldestPreviousDistance_;
  };
}

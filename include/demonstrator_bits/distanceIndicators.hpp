#pragma once

// C++ standard library
#include <cstddef>
#include <vector>

// Demonstrator
#include "demonstrator_bits/pin.hpp"

/**
 * This class represents an array of MY9221[1] LED bars. A single LedArray manages many devices, where all of them are wired to the same clock. The number of LED bars is equal to the number of data pins that are passed to the constructor.
 *
 * A MY9221 consists of 10 LEDs. While the device supports modifying the intensity of each LED, this class only allows to turn them on or off. This information is saved in `state_` and can be modified through `setState()`.
 *
 * The MY9221 API expects a 208 bit instruction on a single pin. The instruction format is described in the `update()` method docs. Calling this method sends such an instruction on every data pin.
 *
 * [1]: https://www.seeedstudio.com/wiki/images/9/98/MY9221_DS_1.0.pdf
 */
namespace demo {
  class DistanceIndicators {
   public:
    /**
     * The number of distance indicators that are managed.
     */
    const std::size_t numberOfIndicators_;

    DistanceIndicators(
        Pin clockPin,
        std::vector<Pin> dataPins);

    /**
     * Sends the current state as intensity level for each LED bar. The state of the lowest LED (red) is stored at index 0.
     *
     * Because all MY9221 are connected to the same clock pin, we need to set all data pins ahead one bit at a time, then clock once. The exact order is:
     *   1. Send 16 bit command word 0x310 on all pins. This selects 16 bit grayscale code at 1001 Hz.
     *   2. For every bit in `currentState`, send 16x high (or 16x low, depending on the current bit value) on the respective pin.
     *   3. Conclude with 4 high/low toggles on the data pins while keeping the clock at a constant level.
     */
    void setIndication(
        const std::vector<double>& distances);

    void setMinimalDistances(
        const std::vector<double>& minimalDistances);
    std::vector<double> getMinimalDistances() const;

    void setWarningDistances(
        const std::vector<double>& warningDistances);
    std::vector<double> getWarningDistances() const;

    void setMaximalDistances(
        const std::vector<double>& maximalDistances);
    std::vector<double> getMaximalDistances() const;

   protected:
    /**
     * These pins are connected to the DI pins on the LED bars.
     */
    std::vector<Pin> dataPins_;

    /**
     * This pin is connected to the DCKI (clock) pins of *all* LED bars.
     *
     * The clock signal is only changed during `setIndication()` calls. In between, its value is `low`.
     */
    Pin clockPin_;

    std::vector<double> minimalDistances_;
    std::vector<double> warningDistances_;
    std::vector<double> maximalDistances_;
  };
}

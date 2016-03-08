#pragma once

// C++ standard library
#include <cstddef>
#include <vector>

// Demonstrator
#include "demonstrator_bits/pin.hpp"

/**
 * This class represents an array of MY9221[1] LED bars. A single LedArray manages many devices, where all of them are wired to the same clock. The number of LED bars is equal to the number of data pins that are passed to the constructor.
 *
 * A MY9221 consists of 10 LEDs. In the demonstrator these LED bars are used to indicate the distance between the demonstrator itself and other obstacles. While the device supports modifying the intensity of each LED, this class will only turn them on or off completely.
 *
 * The pattern for the LEDs is as follows, from far to close distance. Notice that the first two LEDs in each bar are red, while the other eight are green.
 *   ..xxxxxxxx
 *   ..xxxxxxx.
 *   ..xxxxxx..
 *   ..xxxxx...
 *   ..xxxx....
 *   ..xxx.....
 *   ..xx......
 *   ..x.......
 *   .x........
 *   xx........
 *
 * The MY9221 API expects a 208 bit instruction on a single pin. The instruction format is described in the `update()` method docs. Calling that method sends such an instruction on every data pin.
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
     * Update the LED bars to indicate the assigned distances, as described in the class documentation.
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

    /**
     * For distances less than this value, the indicator will show the "closest distance" pattern.
     */
    std::vector<double> minimalDistances_;

    /**
     * For distances greater than this value, the indicator will show one of the (red) warning patterns.
     */
    std::vector<double> warningDistances_;

    /**
     * For distances greater than this value, the indicator will show the "farthest distance" pattern.
     */
    std::vector<double> maximalDistances_;
  };
}

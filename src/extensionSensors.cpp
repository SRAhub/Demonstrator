#include "demonstrator_bits/extensionSensors.hpp"

// C++ standard library
#include <algorithm>

// Demonstrator
#include "demonstrator_bits/assert.hpp"
#include "demonstrator_bits/gpio.hpp"
#include "demonstrator_bits/spi.hpp"

namespace demo {
  ExtensionSensors::ExtensionSensors(
      const std::vector<unsigned int>& channels)
      : numberOfSensors_(channels.size()),
        channels_(channels) {}

  std::vector<double> ExtensionSensors::getExtensions() {
    Spi spi = Gpio::allocateSpi();

    std::vector<double> extensions;
    for (std::size_t sensor = 0; sensor < numberOfSensors_; ++sensor) {
      spi.set(Spi::Pin::ChipSelect, Spi::Digital::High);
      spi.set(Spi::Pin::Clock, Spi::Digital::Low);
      spi.set(Spi::Pin::ChipSelect, Spi::Digital::Low);

      // TODO Using a std::bitset instead for `commandOut`?
      unsigned int commandOut = channels_.at(sensor) | 0x18;
      commandOut <<= 3;

      for (int i = 0; i < 5; i++) {
        spi.set(Spi::Pin::Mosi, (commandOut & 0x80) ? Spi::Digital::High : Spi::Digital::Low);

        commandOut <<= 1;
        spi.set(Spi::Pin::Clock, Spi::Digital::High);
        spi.set(Spi::Pin::Clock, Spi::Digital::Low);
      }

      int adcout = 0;
      for (int i = 0; i < 12; i++) {
        spi.set(Spi::Pin::Clock, Spi::Digital::High);
        spi.set(Spi::Pin::Clock, Spi::Digital::Low);

        adcout <<= 1;
        if (spi.get(Spi::Pin::Miso) == Spi::Digital::High) {
          adcout |= 0x1;
        }
      }

      spi.set(Spi::Pin::ChipSelect, Spi::Digital::High);

      adcout >>= 1;

      extensions.push_back(std::max(minimalMeasureableExtension_.at(sensor), std::min(maximalMeasureableExtension_.at(sensor), adcout / 1023 + sensorAdjustments_.at(sensor))));
    }

    return extensions;
  }

  void ExtensionSensors::setMinimalMeasureableExtension(
      const std::vector<double>& minimalMeasureableExtension) {
    verify(minimalMeasureableExtension.size() == numberOfSensors_, "ExtensionSensors.setSensorAdjustments: The number of minimal measurable extensions must be equal to the number of sensors.");

    minimalMeasureableExtension_ = minimalMeasureableExtension;
  }

  std::vector<double> ExtensionSensors::getMinimalMeasureableExtension() const {
    return minimalMeasureableExtension_;
  }

  void ExtensionSensors::setMaximalMeasureableExtension(
      const std::vector<double>& maximalMeasureableExtension) {
    verify(maximalMeasureableExtension.size() == numberOfSensors_, "ExtensionSensors.setSensorAdjustments: The number of maximal measurable extensions must be equal to the number of sensors.");

    maximalMeasureableExtension_ = maximalMeasureableExtension;
  }

  std::vector<double> ExtensionSensors::getMaximalMeasureableExtension() const {
    return maximalMeasureableExtension_;
  }

  void ExtensionSensors::setSensorAdjustments(
      const std::vector<double>& sensorAdjustments) {
    verify(sensorAdjustments.size() == numberOfSensors_, "ExtensionSensors.setSensorAdjustments: The number of sensor adjustments must be equal to the number of sensors.");

    sensorAdjustments_ = sensorAdjustments;
  }

  std::vector<double> ExtensionSensors::getSensorAdjustments() const {
    return sensorAdjustments_;
  }
}

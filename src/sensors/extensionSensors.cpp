#include "demonstrator_bits/sensors/extensionSensors.hpp"

// C++ standard library
#include <algorithm>
#include <cstddef>
#include <stdexcept>

// Demonstrator
#include "demonstrator_bits/spi.hpp"

namespace demo {
  ExtensionSensors::ExtensionSensors(
      Spi&& spi,
      const std::vector<unsigned int>& channels,
      const double minimalExtension,
      const double maximalExtension)
      : Sensors(channels.size(), minimalExtension, maximalExtension),
        spi_(std::move(spi)),
        channels_(channels) {
  }

  ExtensionSensors::ExtensionSensors(
      ExtensionSensors&& extensionSensors)
      : ExtensionSensors(std::move(extensionSensors.spi_), extensionSensors.channels_, extensionSensors.minimalMeasurableValue_, extensionSensors.maximalMeasurableValue_) {
    setMeasurementCorrections(extensionSensors.measurementCorrections_);
    setNumberOfSamplesPerMeasurment(extensionSensors.numberOfSamplesPerMeasuement_);
  }

  ExtensionSensors& ExtensionSensors::operator=(
      ExtensionSensors&& extensionSensors) {
    if (channels_ != extensionSensors.channels_) {
      throw std::invalid_argument("ExtensionSensors.operator=: The channels must be equal.");
    }

    spi_ = std::move(extensionSensors.spi_);

    Sensors::operator=(std::move(extensionSensors));
    return *this;
  }

  arma::Row<double> ExtensionSensors::measureImplementation() {
    arma::Row<double> extensions(numberOfSensors_);
    for (std::size_t n = 0; n < numberOfSensors_; ++n) {
      spi_.set(Spi::Pin::ChipSelect, Spi::Digital::High);
      spi_.set(Spi::Pin::Clock, Spi::Digital::Low);
      spi_.set(Spi::Pin::ChipSelect, Spi::Digital::Low);

      // TODO Using a std::bitset instead for `commandOut`?
      unsigned int commandOut = channels_.at(n) | 0x18;
      commandOut <<= 3;

      for (int i = 0; i < 5; i++) {
        spi_.set(Spi::Pin::Mosi, (commandOut & 0x80) ? Spi::Digital::High : Spi::Digital::Low);

        commandOut <<= 1;
        spi_.set(Spi::Pin::Clock, Spi::Digital::High);
        spi_.set(Spi::Pin::Clock, Spi::Digital::Low);
      }

      int adcout = 0;
      for (int i = 0; i < 12; i++) {
        spi_.set(Spi::Pin::Clock, Spi::Digital::High);
        spi_.set(Spi::Pin::Clock, Spi::Digital::Low);

        adcout <<= 1;
        if (spi_.get(Spi::Pin::Miso) == Spi::Digital::High) {
          adcout |= 0x1;
        }
      }

      spi_.set(Spi::Pin::ChipSelect, Spi::Digital::High);

      adcout >>= 1;

      extensions(n) = static_cast<double>(adcout) / 1023.0;
    }

    return minimalMeasurableValue_ + extensions * (maximalMeasurableValue_ - minimalMeasurableValue_);
  }
}

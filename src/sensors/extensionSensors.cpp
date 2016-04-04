#include "demonstrator_bits/sensors/extensionSensors.hpp"

// C++ standard library
#include <algorithm>

// Demonstrator
#include "demonstrator_bits/gpio.hpp"
#include "demonstrator_bits/spi.hpp"

namespace demo {
  ExtensionSensors::ExtensionSensors(
      Spi spi,
      const std::vector<unsigned int>& channels)
      : Sensors(channels.size()),
        spi_(std::move(spi)),
        channels_(channels) {}

  std::vector<double> ExtensionSensors::measureImplementation() {
    std::vector<double> extensions;
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

      extensions.push_back(adcout / 1023);
    }

    return extensions;
  }
}

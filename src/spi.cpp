#include "demonstrator_bits/spi.hpp"

// WiringPi
#include <wiringPi.h>

// Demonstrator
#include "demonstrator_bits/assert.hpp"
#include "demonstrator_bits/gpio.hpp"

namespace demo {
  Spi::Spi()
      : ownsSpi_(true) {}

  Spi::Spi(Spi&& other)
      : ownsSpi_(other.ownsSpi_) {
    other.ownsSpi_ = false;
  }

  Spi& Spi::operator=(Spi&& other) {
    if (ownsSpi_) {
      Gpio::deallocate(*this);
    }

    ownsSpi_ = other.ownsSpi_;

    other.ownsSpi_ = false;
    return *this;
  }

  void Spi::set(
      const Pin pin,
      const Spi::Digital value) {
    verify(ownsSpi_, "The ownership of the SPI pins has been transferred");

    ::pinMode(static_cast<int>(pin), OUTPUT);
    ::digitalWrite(static_cast<int>(pin), static_cast<int>(value));
  }

  Spi::Digital Spi::get(
      const Pin pin) {
    verify(ownsSpi_, "The ownership of the SPI pins has been transferred");

    ::pinMode(static_cast<int>(pin), INPUT);
    return (::digitalRead(static_cast<int>(pin)) == 0 ? Digital::Low : Digital::High);
  }

  Spi::~Spi() {
    if (ownsSpi_) {
      Gpio::deallocate(*this);
    }
  }
}

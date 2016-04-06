#include "demonstrator_bits/spi.hpp"

// C++ standard library
#include <iostream>

// WiringPi
#include <wiringPi.h>

// Demonstrator
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
    if (::demo::isVerbose) {
      std::cout << "Setting pin " << static_cast<unsigned int>(pin) << " to " << static_cast<unsigned int>(value) << std::endl;
    }
    
    if (!ownsSpi_) {
      throw std::runtime_error("SPI must be owned to be accessed.");
    }

    ::pinMode(static_cast<int>(pin), OUTPUT);
    ::digitalWrite(static_cast<int>(pin), static_cast<int>(value));
  }

  Spi::Digital Spi::get(
      const Pin pin) {
    if (::demo::isVerbose) {
      std::cout << "Reading pin " << static_cast<unsigned int>(pin) << ". ";
    }
    
    if (!ownsSpi_) {
      throw std::runtime_error("SPI must be owned to be accessed.");
    }

    ::pinMode(static_cast<int>(pin), INPUT);
    Digital output = (::digitalRead(static_cast<int>(pin)) == 0 ? Digital::Low : Digital::High);
    
    if (::demo::isVerbose) {
      std::cout << "Received " << static_cast<unsigned int>(output) << "." << std::endl;
    }
    
    return output;
  }

  Spi::~Spi() {
    if (ownsSpi_) {
      Gpio::deallocate(*this);
    }
  }
}

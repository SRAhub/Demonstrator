#include "demonstrator_bits/pin.hpp"
#include "demonstrator_bits/config.hpp" // IWYU pragma: keep

// C++ standard library
#include <iostream>
#include <ratio>
#include <string>
#include <thread>

// WiringPi
#include <wiringPi.h>

// Demonstrator
#include "demonstrator_bits/gpio.hpp"

namespace demo {
  Pin::Pin(
      const unsigned int pinNumber)
      : pinNumber_(pinNumber),
        ownsPin_(true) {}

  Pin::Pin(Pin&& other)
      : pinNumber_(other.pinNumber_),
        ownsPin_(other.ownsPin_) {
    other.ownsPin_ = false;
  }

  Pin& Pin::operator=(Pin&& other) {
    if (ownsPin_) {
      Gpio::deallocate(*this);
    }

    pinNumber_ = other.pinNumber_;
    ownsPin_ = other.ownsPin_;

    other.ownsPin_ = false;
    return *this;
  }

  void Pin::set(
      const Pin::Digital value) {
    if (::demo::isVerbose) {
      std::cout << "Setting pin " << pinNumber_ << " to " << static_cast<unsigned int>(value) << std::endl;
    }
        
    if (!ownsPin_) {
      throw std::runtime_error("The pin must be owned to be accessed.");
    }

    ::pinMode(static_cast<int>(pinNumber_), OUTPUT);
    ::digitalWrite(static_cast<int>(pinNumber_), static_cast<int>(value));
  }

  void Pin::set(
      const unsigned int value) {
    set(value ? Digital::High : Digital::Low);
  }

  Pin::Digital Pin::get() {
    if (::demo::isVerbose) {
      std::cout << "Reading pin " << pinNumber_ << ". ";
    }
    
    if (!ownsPin_) {
      throw std::runtime_error("The pin must be owned to be accessed.");
    }

    ::pinMode(static_cast<int>(pinNumber_), INPUT);
    Digital output = (::digitalRead(static_cast<int>(pinNumber_)) == 0 ? Digital::Low : Digital::High);
    
    if (::demo::isVerbose) {
      std::cout << "Received " << static_cast<unsigned int>(output) << "." << std::endl;
    }
    
    return output;
  }

  std::chrono::microseconds Pin::waitForSignalEdge(
      const std::chrono::microseconds timeout) {
    if (::demo::isVerbose) {
      std::cout << "Waiting for signal edge on pin " << pinNumber_ << ". ";
    }
    
    if (!ownsPin_) {
      throw std::runtime_error("The pin must be owned to be accessed.");
    }

    Digital currentSignal = get();

    auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();
    while (get() == currentSignal) {
      std::this_thread::sleep_for(std::chrono::nanoseconds(500));
      end = std::chrono::steady_clock::now();

      if (end - start >= timeout) {
        return timeout;
      }
    }

    std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    if (::demo::isVerbose) {
      std::cout << "Took " << duration.count() << "us." << std::endl;
    }
    
    return duration;
  }

  Pin::~Pin() {
    if (ownsPin_) {
      Gpio::deallocate(*this);
    }
  }
}

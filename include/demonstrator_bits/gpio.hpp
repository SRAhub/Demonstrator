#pragma once

// C++ standard library
#include <array>
#include <mutex>

// Demonstrator
#include "demonstrator_bits/pin.hpp"
#include "demonstrator_bits/spi.hpp"
#include "demonstrator_bits/i2c.hpp"
#include "demonstrator_bits/uart.hpp"

namespace demo {
  class Gpio {
   public:
    /**
     * Number of GPIO pins on the A+, B+ and Pi 2 models.
     */
    static const int NUMBER_OF_GPIO_PINS = 26;

    Gpio() = delete;
    Gpio(Gpio&) = delete;
    Gpio operator=(Gpio&) = delete;

    /**
     * Asks for ownership of the pin with the specified number.
     *
     * Throws a `std::runtime_error` if the pin is already allocated.
     * Throws a `std::domain_error` if that pin number does not exists on a Raspberry Pi.
     */
    static Pin allocatePin(
        const unsigned int pinNumber);

    /**
     * Asks for ownership of the SPI pins (for a single slave, excluding pin `GPIO7`, i.e. `CE1`).
     * **Note:** We assume the WiringPi was set up using `::wiringPiSetupGpio()`.
     *
     * Throws a `std::runtime_error` if any SPI pin is already allocated.
     */
    static Spi allocateSpi();

    /**
     * Asks for ownership of the I2C pins.
     * **Note:** We assume the WiringPi was set up using `::wiringPiSetupGpio()`.
     *
     * Throws a `std::runtime_error` if any I2C pin is already allocated.
     */
    static I2c allocateI2c();

    /**
     * Asks for ownership of the UART pins.
     * **Note:** We assume the WiringPi was set up using `::wiringPiSetupGpio()`.
     *
     * Throws a `std::runtime_error` if any UART pin is already allocated.
     */
    static Uart allocateUart();

    /**
     * Returns the ownership over `pin` back to the array.
     *
     * **Note:** Pins are automatically deallocated if they go out of scope.
     */
    static void deallocate(
        Pin& pin);

    /**
     * Returns the ownership over the SPI pins back to the array.
     *
     * **Note:** Pins are automatically deallocated if they go out of scope.
     */
    static void deallocate(
        Spi& spi);

    /**
     * Returns the ownership over the I2C pins back to the array.
     *
     * **Note:** Pins are automatically deallocated if they go out of scope.
     */
    static void deallocate(
        I2c& i2c);

    /**
     * Returns the ownership over the UART pins back to the array.
     *
     * **Note:** Pins are automatically deallocated if they go out of scope.
     */
    static void deallocate(
        Uart& uart);

   protected:
    static std::array<bool, NUMBER_OF_GPIO_PINS> ownedPins_;

    static int i2cFileDescriptor_;

    static std::mutex mutex_;
  };
}

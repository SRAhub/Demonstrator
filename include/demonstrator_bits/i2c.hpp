#pragma once

namespace demo {

  /**
   * An instance of this class represents an I²C master node, communicating over the gpio pins of a Raspberry Pi.
   *
   * This class allocates two pins for the duration of its lifetime, and deallocates them automatically on destruction.
   *
   * Like pins, instances of this class must be obtained from `Gpio` – for more details, look at the `Pin` class docs.
   */
  class I2c {
    friend class Gpio;

   public:
    I2c& operator=(I2c&) = delete;
    I2c(I2c&) = delete;

    I2c(I2c&&);

    I2c& operator=(I2c&&);

    /**
     * Write a single byte into the designated register of the slave device.
     */
    void set(
        const unsigned int register,
        const unsigned int value);

    /**
     * Read a single byte from the designated register of the slave device.
     */
    unsigned int get(
        const unsigned int register);

    virtual ~I2c();

   protected:
    I2c(
        const int fileDescriptor);

    /**
     * The file descriptor provided by the [wiringPi library](http://wiringpi.com/reference/i2c-library/).
     */
    int fileDescriptor_;

    bool ownsI2c_;
  };
}

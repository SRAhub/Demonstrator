#pragma once

namespace demo {
  /**
   * An instance of this class represents an [SPI][1] master node, communicating over the gpio pins of a Raspberry Pi with a single slave.
   *
   * This class allocates the four `demo::Spi::Pin`s for the duration of its lifetime, and deallocates them automatically on destruction.
   *
   * Like pins, instances of this class must be obtained from `Gpio` – for more details, look at the `Pin` class docs.
   *
   *
   * [1]: https://en.wikipedia.org/wiki/Serial_Peripheral_Interface_Bus
   */
  class Spi {
    friend class Gpio;

   public:
    enum class Digital : unsigned int {
      Low = 0, //LOW,
      High = 1 //HIGH
    };

    enum class Pin : unsigned int {
      /**
       * Needs to be `Digital::High` for the slave to respond.
       */
      ChipSelect = 7,

      /**
       * "master out slave in"
       */
      Mosi = 8,

      /**
       * "master in slave out"
       */
      Miso = 9,

      /**
       * As the master, this node needs to set the clock.
       */
      Clock = 10
    };

    Spi& operator=(Spi&) = delete;
    Spi(Spi&) = delete;

    Spi(Spi&&);

    Spi& operator=(Spi&&);

    void set(
        const Pin pin,
        const Digital value);

    Digital get(
        const Pin pin);

    virtual ~Spi();

   protected:
    Spi();

    bool ownsSpi_;
  };
}

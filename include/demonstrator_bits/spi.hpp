#pragma once

namespace demo {
  class Spi {
    friend class Gpio;

   public:
    enum class Digital : unsigned int {
      Low = 0, //LOW,
      High = 1 //HIGH
    };

    enum class Pin : unsigned int {
      ChipSelect = 7,
      Mosi = 8,
      Miso = 9,
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

#pragma once

namespace demo {
  // Global variables, to be used in other files.
  extern bool isVerbose;
}

// WiringPi exclusion must be set via CMake, to ensure that we also avoid linking against it.
// Therefore, use `cmake ... -DUSE_WIRINGPI_DUMMY=[ON|OFF]` to decide whether`USE_WIRINGPI_DUMMY` is to be defined or not.
#define USE_WIRINGPI_DUMMY
#if !defined(USE_WIRINGPI_DUMMY)
#include <wiringPi.h>
#include <wiringPiI2C.h>
#endif
#pragma once

namespace demo {
  // Global variables, to be used in other files.

  /**
   * Enables debugging messages from library components. These are written to stdout (not configurable).
   */
  extern bool isVerbose;
}

// WiringPi exclusion must be set via CMake, to ensure that we also avoid linking against it.
// Therefore, use `cmake ... -DUSE_WIRINGPI_DUMMY=[ON|OFF]` to decide whether`USE_WIRINGPI_DUMMY` is to be defined or not.
#cmakedefine USE_WIRINGPI_DUMMY
#if !defined(USE_WIRINGPI_DUMMY)
#include <wiringPi.h>
#include <wiringPiI2C.h>
#endif

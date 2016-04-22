cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(Demonstrator CXX C)

# Retrieves the version numbers from "include/demonstrator", avoiding possible inconsistency.
file(STRINGS "${PROJECT_SOURCE_DIR}/include/demonstrator" DEMONSTRATOR_HEADER_CONTENTS REGEX "#define DEMONSTRATOR_VERSION_[A-Z]+ ")
string(REGEX REPLACE ".*#define DEMONSTRATOR_VERSION_MAJOR ([0-9]+).*" "\\1" DEMONSTRATOR_VERSION_MAJOR "${DEMONSTRATOR_HEADER_CONTENTS}")
string(REGEX REPLACE ".*#define DEMONSTRATOR_VERSION_MINOR ([0-9]+).*" "\\1" DEMONSTRATOR_VERSION_MINOR "${DEMONSTRATOR_HEADER_CONTENTS}")
string(REGEX REPLACE ".*#define DEMONSTRATOR_VERSION_PATCH ([0-9]+).*" "\\1" DEMONSTRATOR_VERSION_PATCH "${DEMONSTRATOR_HEADER_CONTENTS}")
set(DEMONSTRATOR_VERSION "${DEMONSTRATOR_VERSION_MAJOR}.${DEMONSTRATOR_VERSION_MINOR}.${DEMONSTRATOR_VERSION_PATCH}")

# Adds additional cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

message(STATUS "")
message(STATUS "Building Demonstator (version: ${DEMONSTRATOR_VERSION}).")

# ------------------------
# CHECKING PREREQUIREMENTS
# ------------------------

message(STATUS "")
message(STATUS "Checking prerequirements.")

find_package(WiringPi REQUIRED)
include_directories(${WIRINGPI_INCLUDE_DIR})

find_package(Armadillo 6.400.0 REQUIRED)
include_directories(${ARMADILLO_INCLUDE_DIR})

find_package(Mantella 0.12.0 REQUIRED)
include_directories(${MANTELLA_INCLUDE_DIR})

include_directories(BEFORE ${PROJECT_SOURCE_DIR}/include)

# --------------------
# CONFIGURING COMPILER
# --------------------

message(STATUS "")
message(STATUS "Configuring compiler.")

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_COMPILER_IS_CLANGXX ON)
endif ()

if(CMAKE_COMPILER_IS_CLANGXX)
  message(STATUS "- Detected Clang compiler.")
elseif(CMAKE_COMPILER_IS_GNUCXX)
  message(STATUS "- Detected GCC compiler.")
else()
  message(STATUS "- Detected neither Clang, GCC nor MSCV but '${CMAKE_CXX_COMPILER_ID}' instead. Compiler flags may be incorrect.")
endif()

message(STATUS "- Enabling C++14 features.")
add_compile_options(-std=c++14)

message(STATUS "- Using level 3 code optimisation.")
add_compile_options(-O3)

message(STATUS "- Adding POSIX threading.")
add_compile_options(-pthread)
message(STATUS "  - Working around compiler bug 42734: Trivial use of std::thread fails with 'pure virtual method called'.")
add_definitions(-D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8)

message(STATUS "- Setting build type to 'Release'.")
message(STATUS "  - Use 'cmake ... -DCMAKE_BUILD_TYPE=[Release|Debug]' to specify this.")
set(CMAKE_BUILD_TYPE "Release")

# -----------
# COMPILATION
# -----------

message(STATUS "")
message(STATUS "Configuring library compilation.")

if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
endif()

if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
endif()

configure_file(${PROJECT_SOURCE_DIR}/include/demonstrator_bits/config.hpp.cmake ${PROJECT_SOURCE_DIR}/include/demonstrator_bits/config.hpp)

# All paths must start with "src/"
add_library(demonstrator SHARED
  # Mantella (preview)
  src/mantella.cpp

  # Configuration
  src/config.cpp

  # GPIO
  src/gpio.cpp
  src/pin.cpp
  src/spi.cpp
  src/i2c.cpp
  src/uart.cpp

  # Network
  src/network.cpp
  
  # Sensors
  src/sensors.cpp
  src/sensors/attitudeSensors.cpp
  src/sensors/distanceSensors.cpp
  src/sensors/extensionSensors.cpp
  src/sensors/mouse3dSensors.cpp

  # Indicators
  src/distanceIndicators.cpp

  # Controllers and Actuators
  src/servoControllers.cpp
  src/linearActuators.cpp
  src/stewartPlatform.cpp
)

# Linking against prerequirements

target_link_libraries(demonstrator ${WIRINGPI_LIBRARIES})
target_link_libraries(demonstrator ${ARMADILLO_LIBRARIES})
target_link_libraries(demonstrator ${MANTELLA_LIBRARIES})
target_link_libraries(demonstrator pthread)

# -----------
# INSTALLTION
# -----------

message(STATUS "")
message(STATUS "Configuring installation.")

if(NOT INSTALL_INCLUDE_DIR)
  set(INSTALL_INCLUDE_DIR "include")
endif()

set_property(TARGET demonstrator PROPERTY VERSION ${DEMONSTRATOR_VERSION})
set_property(TARGET demonstrator PROPERTY SOVERSION ${DEMONSTRATOR_VERSION_MAJOR})

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION ${INSTALL_INCLUDE_DIR} COMPONENT dev)

if(NOT INSTALL_LIB_DIR)
  set(INSTALL_LIB_DIR "lib")
endif()

install(TARGETS demonstrator
  ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
  LIBRARY DESTINATION ${INSTALL_LIB_DIR})

message(STATUS "")
message(STATUS "Noticable CMAKE variables:")
message(STATUS "- CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}")
message(STATUS "- CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message(STATUS "- CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
message(STATUS "- CMAKE_LIBRARY_OUTPUT_DIRECTORY = ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message(STATUS "- ARCHIVE_OUTPUT_DIRECTORY = ${ARCHIVE_OUTPUT_DIRECTORY}")
message(STATUS "- CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
message(STATUS "- INSTALL_INCLUDE_DIR = ${INSTALL_INCLUDE_DIR}")
message(STATUS "- INSTALL_LIB_DIR = ${INSTALL_LIB_DIR}")
message(STATUS "")

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

# Development build options
option(USE_CCACHE "Use ccache" OFF)
option(BUILD_TESTS "Build tests" OFF)
option(MEASURE_CODE_COVERAGE "Measure code coverage" OFF)
option(USE_WIRINGPI_DUMMY "Use WiringPi dummy" OFF)

mark_as_advanced(
  USE_CCACHE
  BUILD_TESTS
  MEASURE_CODE_COVERAGE
  USE_WIRINGPI_DUMMY
)

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
  message(STATUS "- Detected neither Clang, GCC nor MSCV but '${CMAKE_CXX_COMPILER_ID}' instead. Compiler flags may not be optimal.")
endif()

if(USE_CCACHE)
  find_program(CCACHE_FOUND ccache)
  message(STATUS "- Trying to use ccache to speed up recompilations.")
  message(STATUS "  - Use 'cmake ... -DUSE_CCACHE=OFF' to exclude this.")
  if(CCACHE_FOUND)
    message(STATUS "  - Using ccache.")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
  else()
    message(STATUS "  - ccache was no found on your system.")
  endif()
else()
  message(STATUS "- Excluding ccache usage.")
  message(STATUS "  - Use 'cmake ... -DUSE_CCACHE=ON' to add this.")
endif()

# ----------------------
# SETTING COMPILER FLAGS
# ----------------------

message(STATUS "") 
message(STATUS "Setting compiler flags.")
  if(NOT CMAKE_COMPILER_IS_MSCV)
    message(STATUS "- Enabling C++11 features.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  endif()

message(STATUS "- Adding SSE3, SSE4, AVX, ... support.")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

message(STATUS "- Using level 3 code optimisation.")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
  
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "- Setting build to 'Release'.")
  message(STATUS "  - Use 'cmake ... -DCMAKE_BUILD_TYPE=[Release|Debug]' to specify this.")
  set(CMAKE_BUILD_TYPE "Release")
endif()

# ----------------
# BUILDING LIBRARY
# ----------------

message(STATUS "") 
message(STATUS "Configuring library build.")

if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
endif()

if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
endif()
   
# Include Demonstrator
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/include)

if(USE_WIRINGPI_DUMMY)
  message(STATUS "- Replacing WiringPi instructions by dummy code (debugging setup).")
  message(STATUS "  - Use 'cmake ... -DUSE_WIRINGPI_DUMMY=OFF' to use WiringPi (productive setup).")
else()
  message(STATUS "- Using WiringPi instructions (productive setup).")
  message(STATUS "  - Use 'cmake ... -DUSE_WIRINGPI_DUMMY=ON' to exclude WiringPi (debugging setup).")
endif()
configure_file(${PROJECT_SOURCE_DIR}/include/demonstrator_bits/config.hpp.cmake ${PROJECT_SOURCE_DIR}/include/demonstrator_bits/config.hpp)

# All paths must start with "src/"
add_library(demonstrator SHARED
  # Configuration
  src/config.cpp
  
  # Assertion
  src/assert.cpp
  
  # GPIO
  src/gpio.cpp
  src/pin.cpp
  src/spi.cpp
  src/i2c.cpp
  src/uart.cpp
  
  # Network
  src/network.cpp

  # Sensors
  src/attitudeSensors.cpp
  src/distanceSensors.cpp
  src/extensionSensors.cpp

  # Controllers and Actuators
  src/servoControllers.cpp
  src/linearActuators.cpp
  src/stewartPlatform.cpp

  # Indicators
  src/distanceIndicators.cpp

  # Input devices
  src/mouse3d.cpp
)

# Linking against prerequirements
target_link_libraries(demonstrator ${WIRINGPI_LIBRARIES})
target_link_libraries(demonstrator ${ARMADILLO_LIBRARIES})
target_link_libraries(demonstrator ${MANTELLA_LIBRARIES})


# -------------------
# BUILDING UNIT TESTS
# -------------------

if(BUILD_TESTS)
  message(STATUS "") 
  message(STATUS "Configuring unit tests.")
  message(STATUS "- Use 'cmake ... -DBUILD_TESTS=OFF' to exclude this.")
  
  message(STATUS "- OVERWRITE: Forcing replacement of WiringPi instructions by dummy code (debugging setup).")
  set(USE_WIRINGPI_DUMMY ON)
  configure_file(${PROJECT_SOURCE_DIR}/include/demonstrator_bits/config.hpp.cmake ${PROJECT_SOURCE_DIR}/include/demonstrator_bits/config.hpp)
  
  message(STATUS "- OVERWRITE: Forcing debug build and deactivating code optimisation.")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
  set(CMAKE_BUILD_TYPE "Debug")

  if(MEASURE_CODE_COVERAGE)
    message(STATUS "- Adding code coverage measurement.")
    message(STATUS "  - Use 'cmake ... -DMEASURE_CODE_COVERAGE=OFF' to exclude this.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
  else()
    message(STATUS "- Excluding code coverage measurement.")
    message(STATUS "  - Use 'cmake ... -DMEASURE_CODE_COVERAGE=ON' to add this.")
  endif()
  
  if(CMAKE_COMPILER_IS_CLANGXX OR CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "- Displaying compiler warnings.")
    if(CMAKE_COMPILER_IS_CLANGXX)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-weak-vtables")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-exit-time-destructors")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-global-constructors")
      # Ignore warnings for unknown options, as we make use of options introduced by more recent Clang versions.
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-warning-option")
    elseif(CMAKE_COMPILER_IS_GNUCXX)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-padded")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
    
    message(STATUS "- Handling warnings as errors.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
  endif()
    
  if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
  endif()

  # Check additional prerequirements
  find_package(Catch REQUIRED) # Catch (header only)
  include_directories(${CATCH_INCLUDE_DIRS}) # Catch
  
  # Include catch extensions
  include_directories(${PROJECT_SOURCE_DIR}/test)
  
  # Compile tests
  # All paths must start with "test/"
  add_executable(demonstratorTest
    test/main.cpp
    
    # Catch extensions
    test/catchExtension.cpp
    
    # Configurations
    test/testConfig.cpp
    
    # Assertions
    test/testAssert.cpp
  
    # GPIO
    src/gpio.cpp
  )

  # Link against prerequirements
  target_link_libraries(demonstratorTest demonstrator)
  target_link_libraries(demonstrator ${WIRINGPI_LIBRARIES})
  target_link_libraries(demonstrator ${ARMADILLO_LIBRARIES})
  target_link_libraries(demonstrator ${MANTELLA_LIBRARIES})
else()
  message(STATUS "") 
  message(STATUS "Skipping unit tests.")
  message(STATUS "- Use 'cmake ... -DBUILD_TESTS=ON' to add this.")
endif()

# ---------------
# INSTALL LIBRARY
# ---------------

message(STATUS "")
message(STATUS "Configuring library installation.")

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
message(STATUS "- CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}.")
message(STATUS "- CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message(STATUS "- CMAKE_LIBRARY_OUTPUT_DIRECTORY = ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message(STATUS "- ARCHIVE_OUTPUT_DIRECTORY = ${ARCHIVE_OUTPUT_DIRECTORY}")
message(STATUS "- CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
message(STATUS "- INSTALL_INCLUDE_DIR = ${INSTALL_INCLUDE_DIR}")
message(STATUS "- INSTALL_LIB_DIR = ${INSTALL_LIB_DIR}")
message(STATUS "")
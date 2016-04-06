cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(DemonstratorApplications CXX)

# Adds additional cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")

if(APPLE)
  # Use @rpath on Mac OS X.
  set(CMAKE_MACOSX_RPATH ON)
endif()

mark_as_advanced(
  USE_CCACHE
)

message(STATUS "")
message(STATUS "Building Demonstator applications.")

# ------------------------
# CHECKING PREREQUIREMENTS
# ------------------------

message(STATUS "")
message(STATUS "Checking prerequirements.")

find_package(WiringPi REQUIRED)
include_directories(${WIRINGPI_INCLUDE_DIR})

find_package(Armadillo 6.400.0 REQUIRED)
include_directories(${ARMADILLO_INCLUDE_DIR})

find_package(Demonstrator 0.20.0 REQUIRED)
include_directories(${DEMONSTRATOR_INCLUDE_DIR})

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
    message(STATUS "- Enabling C++14 features.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
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

# ---------------------
# BUILDING Applications
# ---------------------

add_executable(distanceSensors
  distanceSensors.cpp
)

target_link_libraries(distanceSensors ${WIRINGPI_LIBRARIES})
target_link_libraries(distanceSensors ${ARMADILLO_LIBRARIES})
target_link_libraries(distanceSensors ${DEMONSTRATOR_LIBRARIES})

add_executable(distanceIndicators
  distanceIndicators.cpp
)

target_link_libraries(distanceIndicators ${WIRINGPI_LIBRARIES})
target_link_libraries(distanceIndicators ${ARMADILLO_LIBRARIES})
target_link_libraries(distanceIndicators ${DEMONSTRATOR_LIBRARIES})

add_executable(attitudeSensors
  attitudeSensors.cpp
)

target_link_libraries(attitudeSensors ${WIRINGPI_LIBRARIES})
target_link_libraries(attitudeSensors ${ARMADILLO_LIBRARIES})
target_link_libraries(attitudeSensors ${DEMONSTRATOR_LIBRARIES})

add_executable(extensionSensors
  extensionSensors.cpp
)

target_link_libraries(extensionSensors ${WIRINGPI_LIBRARIES})
target_link_libraries(extensionSensors ${ARMADILLO_LIBRARIES})
target_link_libraries(extensionSensors ${DEMONSTRATOR_LIBRARIES})

add_executable(servoControllers
  servoControllers.cpp
)

target_link_libraries(servoControllers ${WIRINGPI_LIBRARIES})
target_link_libraries(servoControllers ${ARMADILLO_LIBRARIES})
target_link_libraries(servoControllers ${DEMONSTRATOR_LIBRARIES})

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

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(DemonstratorApplications CXX)

# Adds additional cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")

message(STATUS "")
message(STATUS "Building demonstator applications.")

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

# ---------------------
# COMPILATION
# ---------------------

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
endif()

message(STATUS "")
message(STATUS "Configuring maintenance applications.")
# All paths must start with "maintenance/"

message(STATUS "- Distance sensors.")
add_executable(maintainDistanceSensors
  commandline.cpp
  maintenance/distanceSensors.cpp
)

target_link_libraries(maintainDistanceSensors ${WIRINGPI_LIBRARIES})
target_link_libraries(maintainDistanceSensors ${ARMADILLO_LIBRARIES})
target_link_libraries(maintainDistanceSensors ${MANTELLA_LIBRARIES})
target_link_libraries(maintainDistanceSensors ${DEMONSTRATOR_LIBRARIES})


message(STATUS "- Distance indicators.")
add_executable(maintainDistanceIndicators
  commandline.cpp
  maintenance/distanceIndicators.cpp
)

target_link_libraries(maintainDistanceIndicators ${WIRINGPI_LIBRARIES})
target_link_libraries(maintainDistanceIndicators ${ARMADILLO_LIBRARIES})
target_link_libraries(maintainDistanceIndicators ${MANTELLA_LIBRARIES})
target_link_libraries(maintainDistanceIndicators ${DEMONSTRATOR_LIBRARIES})

message(STATUS "- Attitude sensors.")
add_executable(maintainAttitudeSensors
  commandline.cpp
  maintenance/attitudeSensors.cpp
)

target_link_libraries(maintainAttitudeSensors ${WIRINGPI_LIBRARIES})
target_link_libraries(maintainAttitudeSensors ${ARMADILLO_LIBRARIES})
target_link_libraries(maintainAttitudeSensors ${MANTELLA_LIBRARIES})
target_link_libraries(maintainAttitudeSensors ${DEMONSTRATOR_LIBRARIES})

message(STATUS "- Extension sensors.")
add_executable(maintainExtensionSensors
  commandline.cpp
  maintenance/extensionSensors.cpp
)

target_link_libraries(maintainExtensionSensors ${WIRINGPI_LIBRARIES})
target_link_libraries(maintainExtensionSensors ${ARMADILLO_LIBRARIES})
target_link_libraries(maintainExtensionSensors ${MANTELLA_LIBRARIES})
target_link_libraries(maintainExtensionSensors ${DEMONSTRATOR_LIBRARIES})

message(STATUS "- Servo controllers.")
add_executable(maintainServoControllers
  commandline.cpp
  maintenance/servoControllers.cpp
)

target_link_libraries(maintainServoControllers ${WIRINGPI_LIBRARIES})
target_link_libraries(maintainServoControllers ${ARMADILLO_LIBRARIES})
target_link_libraries(maintainServoControllers ${MANTELLA_LIBRARIES})
target_link_libraries(maintainServoControllers ${DEMONSTRATOR_LIBRARIES})

message(STATUS "- Linear actuators.")
add_executable(maintainLinearActuators
  commandline.cpp
  maintenance/linearActuators.cpp
)

target_link_libraries(maintainLinearActuators ${WIRINGPI_LIBRARIES})
target_link_libraries(maintainLinearActuators ${ARMADILLO_LIBRARIES})
target_link_libraries(maintainLinearActuators ${MANTELLA_LIBRARIES})
target_link_libraries(maintainLinearActuators ${DEMONSTRATOR_LIBRARIES})

message(STATUS "- Stewart platform.")
add_executable(maintainStewartPlatform
  commandline.cpp
  maintenance/stewartPlatform.cpp
)

target_link_libraries(maintainStewartPlatform ${WIRINGPI_LIBRARIES})
target_link_libraries(maintainStewartPlatform ${ARMADILLO_LIBRARIES})
target_link_libraries(maintainStewartPlatform ${MANTELLA_LIBRARIES})
target_link_libraries(maintainStewartPlatform ${DEMONSTRATOR_LIBRARIES})

message(STATUS "")
message(STATUS "Configuring calibration applications.")
# All paths must start with "calibration/"

message(STATUS "- Extension sensor.")
add_executable(calibrateExtensionSensor
  commandline.cpp
  calibration/extensionSensors.cpp
)

target_link_libraries(calibrateExtensionSensor ${WIRINGPI_LIBRARIES})
target_link_libraries(calibrateExtensionSensor ${ARMADILLO_LIBRARIES})
target_link_libraries(calibrateExtensionSensor ${MANTELLA_LIBRARIES})
target_link_libraries(calibrateExtensionSensor ${DEMONSTRATOR_LIBRARIES})

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

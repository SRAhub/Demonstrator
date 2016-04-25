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
target_link_libraries(maintainDistanceSensors pthread)


message(STATUS "- Distance indicators.")
add_executable(maintainDistanceIndicators
  commandline.cpp
  maintenance/distanceIndicators.cpp
)

target_link_libraries(maintainDistanceIndicators ${WIRINGPI_LIBRARIES})
target_link_libraries(maintainDistanceIndicators ${ARMADILLO_LIBRARIES})
target_link_libraries(maintainDistanceIndicators ${MANTELLA_LIBRARIES})
target_link_libraries(maintainDistanceIndicators ${DEMONSTRATOR_LIBRARIES})
target_link_libraries(maintainDistanceIndicators pthread)

message(STATUS "- Attitude sensors.")
add_executable(maintainAttitudeSensors
  commandline.cpp
  maintenance/attitudeSensors.cpp
)

target_link_libraries(maintainAttitudeSensors ${WIRINGPI_LIBRARIES})
target_link_libraries(maintainAttitudeSensors ${ARMADILLO_LIBRARIES})
target_link_libraries(maintainAttitudeSensors ${MANTELLA_LIBRARIES})
target_link_libraries(maintainAttitudeSensors ${DEMONSTRATOR_LIBRARIES})
target_link_libraries(maintainAttitudeSensors pthread)

message(STATUS "- Extension sensors.")
add_executable(maintainExtensionSensors
  commandline.cpp
  maintenance/extensionSensors.cpp
)

target_link_libraries(maintainExtensionSensors ${WIRINGPI_LIBRARIES})
target_link_libraries(maintainExtensionSensors ${ARMADILLO_LIBRARIES})
target_link_libraries(maintainExtensionSensors ${MANTELLA_LIBRARIES})
target_link_libraries(maintainExtensionSensors ${DEMONSTRATOR_LIBRARIES})
target_link_libraries(maintainExtensionSensors pthread)

message(STATUS "- Servo controllers.")
add_executable(maintainServoControllers
  commandline.cpp
  maintenance/servoControllers.cpp
)

target_link_libraries(maintainServoControllers ${WIRINGPI_LIBRARIES})
target_link_libraries(maintainServoControllers ${ARMADILLO_LIBRARIES})
target_link_libraries(maintainServoControllers ${MANTELLA_LIBRARIES})
target_link_libraries(maintainServoControllers ${DEMONSTRATOR_LIBRARIES})
target_link_libraries(maintainServoControllers pthread)

message(STATUS "- Linear actuators.")
add_executable(maintainLinearActuators
  commandline.cpp
  maintenance/linearActuators.cpp
)

target_link_libraries(maintainLinearActuators ${WIRINGPI_LIBRARIES})
target_link_libraries(maintainLinearActuators ${ARMADILLO_LIBRARIES})
target_link_libraries(maintainLinearActuators ${MANTELLA_LIBRARIES})
target_link_libraries(maintainLinearActuators ${DEMONSTRATOR_LIBRARIES})
target_link_libraries(maintainLinearActuators pthread)

message(STATUS "- Stewart platform.")
add_executable(maintainStewartPlatform
  commandline.cpp
  maintenance/stewartPlatform.cpp
)

target_link_libraries(maintainStewartPlatform ${WIRINGPI_LIBRARIES})
target_link_libraries(maintainStewartPlatform ${ARMADILLO_LIBRARIES})
target_link_libraries(maintainStewartPlatform ${MANTELLA_LIBRARIES})
target_link_libraries(maintainStewartPlatform ${DEMONSTRATOR_LIBRARIES})
target_link_libraries(maintainStewartPlatform pthread)

message(STATUS "- maintainMouse3dSensors.")
add_executable(maintainMouse3dSensors
  commandline.cpp
  maintenance/mouse3dSensors.cpp
)

target_link_libraries(maintainMouse3dSensors ${WIRINGPI_LIBRARIES})
target_link_libraries(maintainMouse3dSensors ${ARMADILLO_LIBRARIES})
target_link_libraries(maintainMouse3dSensors ${MANTELLA_LIBRARIES})
target_link_libraries(maintainMouse3dSensors ${DEMONSTRATOR_LIBRARIES})
target_link_libraries(maintainMouse3dSensors pthread)

message(STATUS "- maintainNetwork.")
add_executable(maintainNetwork
  commandline.cpp
  maintenance/network.cpp
)

target_link_libraries(maintainNetwork ${WIRINGPI_LIBRARIES})
target_link_libraries(maintainNetwork ${ARMADILLO_LIBRARIES})
target_link_libraries(maintainNetwork ${MANTELLA_LIBRARIES})
target_link_libraries(maintainNetwork ${DEMONSTRATOR_LIBRARIES})
target_link_libraries(maintainNetwork pthread)

message(STATUS "")
message(STATUS "Configuring calibration applications.")
# All paths must start with "calibration/"

message(STATUS "- Extension sensors.")
add_executable(calibrateExtensionSensors
  commandline.cpp
  calibration/extensionSensors.cpp
)

target_link_libraries(calibrateExtensionSensors ${WIRINGPI_LIBRARIES})
target_link_libraries(calibrateExtensionSensors ${ARMADILLO_LIBRARIES})
target_link_libraries(calibrateExtensionSensors ${MANTELLA_LIBRARIES})
target_link_libraries(calibrateExtensionSensors ${DEMONSTRATOR_LIBRARIES})
target_link_libraries(calibrateExtensionSensors pthread)

message(STATUS "- Attitude sensors.")
add_executable(calibrateAttitudeSensors
  commandline.cpp
  calibration/attitudeSensors.cpp
)

target_link_libraries(calibrateAttitudeSensors ${WIRINGPI_LIBRARIES})
target_link_libraries(calibrateAttitudeSensors ${ARMADILLO_LIBRARIES})
target_link_libraries(calibrateAttitudeSensors ${MANTELLA_LIBRARIES})
target_link_libraries(calibrateAttitudeSensors ${DEMONSTRATOR_LIBRARIES})
target_link_libraries(calibrateAttitudeSensors pthread)

message(STATUS "")
message(STATUS "Configuring demonstation applications.")
# All paths must start with "demonstration/"

message(STATUS "- Sensors.")
add_executable(demonstrateSensor
  commandline.cpp
  demonstration/sensors.cpp
)

target_link_libraries(demonstrateSensor ${WIRINGPI_LIBRARIES})
target_link_libraries(demonstrateSensor ${ARMADILLO_LIBRARIES})
target_link_libraries(demonstrateSensor ${MANTELLA_LIBRARIES})
target_link_libraries(demonstrateSensor ${DEMONSTRATOR_LIBRARIES})
target_link_libraries(demonstrateSensor pthread)

message(STATUS "- Motors.")
add_executable(demonstrateMotor
  commandline.cpp
  demonstration/motors.cpp
)

target_link_libraries(demonstrateMotor ${WIRINGPI_LIBRARIES})
target_link_libraries(demonstrateMotor ${ARMADILLO_LIBRARIES})
target_link_libraries(demonstrateMotor ${MANTELLA_LIBRARIES})
target_link_libraries(demonstrateMotor ${DEMONSTRATOR_LIBRARIES})
target_link_libraries(demonstrateMotor pthread)

message(STATUS "- End-effector.")
add_executable(demonstrateEndEffector
  commandline.cpp
  demonstration/endEffector.cpp
)

target_link_libraries(demonstrateEndEffector ${WIRINGPI_LIBRARIES})
target_link_libraries(demonstrateEndEffector ${ARMADILLO_LIBRARIES})
target_link_libraries(demonstrateEndEffector ${MANTELLA_LIBRARIES})
target_link_libraries(demonstrateEndEffector ${DEMONSTRATOR_LIBRARIES})
target_link_libraries(demonstrateEndEffector pthread)

message(STATUS "- 3D Mouse.")
add_executable(demonstrateMouse3d
  commandline.cpp
  demonstration/mouse3d.cpp
)

target_link_libraries(demonstrateMouse3d ${WIRINGPI_LIBRARIES})
target_link_libraries(demonstrateMouse3d ${ARMADILLO_LIBRARIES})
target_link_libraries(demonstrateMouse3d ${MANTELLA_LIBRARIES})
target_link_libraries(demonstrateMouse3d ${DEMONSTRATOR_LIBRARIES})
target_link_libraries(demonstrateMouse3d pthread)

message(STATUS "- Evasion.")
add_executable(demonstrateEvasion
  commandline.cpp
  demonstration/evasion.cpp
)

target_link_libraries(demonstrateEvasion ${WIRINGPI_LIBRARIES})
target_link_libraries(demonstrateEvasion ${ARMADILLO_LIBRARIES})
target_link_libraries(demonstrateEvasion ${MANTELLA_LIBRARIES})
target_link_libraries(demonstrateEvasion ${DEMONSTRATOR_LIBRARIES})
target_link_libraries(demonstrateEvasion pthread)

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

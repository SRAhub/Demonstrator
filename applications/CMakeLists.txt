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
add_executable(distanceSensors
  commandline.cpp
  maintenance/distanceSensors.cpp
)

target_link_libraries(distanceSensors ${WIRINGPI_LIBRARIES})
target_link_libraries(distanceSensors ${ARMADILLO_LIBRARIES})
target_link_libraries(distanceSensors ${MANTELLA_LIBRARIES})
target_link_libraries(distanceSensors ${DEMONSTRATOR_LIBRARIES})
set_target_properties(distanceSensors PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/maintenance")


message(STATUS "- Distance indicators.")
add_executable(distanceIndicators
  commandline.cpp
  maintenance/distanceIndicators.cpp
)

target_link_libraries(distanceIndicators ${WIRINGPI_LIBRARIES})
target_link_libraries(distanceIndicators ${ARMADILLO_LIBRARIES})
target_link_libraries(distanceIndicators ${MANTELLA_LIBRARIES})
target_link_libraries(distanceIndicators ${DEMONSTRATOR_LIBRARIES})
set_target_properties(distanceIndicators PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/maintenance")

message(STATUS "- Attitude sensors.")
add_executable(attitudeSensors
  commandline.cpp
  maintenance/attitudeSensors.cpp
)

target_link_libraries(attitudeSensors ${WIRINGPI_LIBRARIES})
target_link_libraries(attitudeSensors ${ARMADILLO_LIBRARIES})
target_link_libraries(attitudeSensors ${MANTELLA_LIBRARIES})
target_link_libraries(attitudeSensors ${DEMONSTRATOR_LIBRARIES})
set_target_properties(attitudeSensors PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/maintenance")

message(STATUS "- Extension sensors.")
add_executable(extensionSensors
  commandline.cpp
  maintenance/extensionSensors.cpp
)

target_link_libraries(extensionSensors ${WIRINGPI_LIBRARIES})
target_link_libraries(extensionSensors ${ARMADILLO_LIBRARIES})
target_link_libraries(extensionSensors ${MANTELLA_LIBRARIES})
target_link_libraries(extensionSensors ${DEMONSTRATOR_LIBRARIES})
set_target_properties(extensionSensors PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/maintenance")

message(STATUS "- Servo controllers.")
add_executable(servoControllers
  commandline.cpp
  maintenance/servoControllers.cpp
)

target_link_libraries(servoControllers ${WIRINGPI_LIBRARIES})
target_link_libraries(servoControllers ${ARMADILLO_LIBRARIES})
target_link_libraries(servoControllers ${MANTELLA_LIBRARIES})
target_link_libraries(servoControllers ${DEMONSTRATOR_LIBRARIES})
set_target_properties(servoControllers PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/maintenance")

message(STATUS "- Linear actuators.")
add_executable(linearActuators
  commandline.cpp
  maintenance/linearActuators.cpp
)

target_link_libraries(linearActuators ${WIRINGPI_LIBRARIES})
target_link_libraries(linearActuators ${ARMADILLO_LIBRARIES})
target_link_libraries(linearActuators ${MANTELLA_LIBRARIES})
target_link_libraries(linearActuators ${DEMONSTRATOR_LIBRARIES})
set_target_properties(linearActuators PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/maintenance")

message(STATUS "- Stewart platform.")
add_executable(stewartPlatform
  commandline.cpp
  maintenance/stewartPlatform.cpp
)

target_link_libraries(stewartPlatform ${WIRINGPI_LIBRARIES})
target_link_libraries(stewartPlatform ${ARMADILLO_LIBRARIES})
target_link_libraries(stewartPlatform ${MANTELLA_LIBRARIES})
target_link_libraries(stewartPlatform ${DEMONSTRATOR_LIBRARIES})
set_target_properties(stewartPlatform PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/maintenance")

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

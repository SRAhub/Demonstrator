#pragma once

// Demonstrator
#include <demonstrator>

demo::Network&& createNetwork();
demo::AttitudeSensors&& createAttitudeSensors();
demo::DistanceSensors&& createDistanceSensors();
demo::ExtensionSensors&& createExtensionSensors();
demo::Mouse3dSensors&& createMouse3dSensors();
demo::DistanceIndicators&& createDistanceIndicators();
demo::ServoControllers&& createServoControllers();
demo::LinearActuators&& createLinearActuators();
demo::StewartPlatform&& createStewartPlatform();
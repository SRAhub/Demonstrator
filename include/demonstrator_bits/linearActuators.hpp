#pragma once

// C++ standard library
#include <vector>

// Demonstrator
#include "demonstrator_bits/extensionSensors.hpp"
#include "demonstrator_bits/servoControllers.hpp"
namespace demo {
  class Pin;
}

namespace demo {
  class LinearActuators {
   public:
    explicit LinearActuators(
        std::vector<Pin> servoControllerDirectionPins,
        const std::vector<unsigned int>& servoControllerChannels,
        const std::vector<unsigned int>& extensionSensorChannels);

    void setExtensions(
        const std::vector<double>& extensions,
        const std::vector<double>& speeds);

    void setMinimalExtensions(
        const std::vector<double>& minimalExtensions);
    std::vector<double> getMinimalExtensions() const;

    void setMaximalExtensions(
        const std::vector<double>& maximalExtensions);
    std::vector<double> getMaximalExtensions() const;

    void setMinimalSpeeds(
        const std::vector<double>& minimalSpeeds);
    std::vector<double> getMinimalSpeeds() const;

    void setMaximalSpeeds(
        const std::vector<double>& maximalSpeeds);
    std::vector<double> getMaximalSpeeds() const;

   protected:
    const ServoControllers servoControllers_;
    const ExtensionSensors extensionSensors_;

    std::vector<double> minimalExtensions_;
    std::vector<double> maximalExtensions_;

    std::vector<double> minimalSpeeds_;
    std::vector<double> maximalSpeeds_;
  };
}

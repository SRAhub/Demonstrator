#include "demonstrator_bits/linearActuators.hpp"

// C++ standard library
#include <chrono>

namespace demo {
  LinearActuators::LinearActuators(
      std::vector<Pin> directionPins,
      I2c i2c,
      const std::vector<unsigned int>& i2cChannels,
      Spi spi,
      const std::vector<unsigned int>& spiChannels)
    : numberOfActuators_(directionPins.size()),
      servoControllers_(std::move(directionPins), std::move(i2c), i2cChannels),
      extensionSensors_(std::move(spi), spiChannels) {
    if (servoControllers_.numberOfControllers_ != extensionSensors_.numberOfSensors_) {
      throw std::logic_error("LinearActuators: The number of controllers must be equal to the number of sensors.");
    }
    
    extensionSensors_.setMinimalMeasurableValue(0.1);
    extensionSensors_.setMaximalMeasurableValue(1.0);
  }
  
  void LinearActuators::setExtensions(
        const std::vector<double>& extensions,
        const std::vector<double>& speeds) {
          
    std::vector<bool> forwards = {true, false, false, false, false, false};
    servoControllers_.run(forwards, speeds);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    servoControllers_.stop();
          
    stopReachExtension_ = false;
    reachExtension(extensions, speeds);
    // reachExtensionThread_ = std::thread([=] {reachExtension(extensions, speeds);});
  }
  
  void LinearActuators::reachExtension(
      const std::vector<double>& extensions,
      const std::vector<double>& maximalSpeeds) {
    std::vector<double> currentExtension = extensionSensors_.measure();
        
    bool hasReachedPosition = true;
    for (std::size_t n = 0; n < numberOfActuators_; ++n) { 
      if (std::abs(extensions.at(n) - currentExtension.at(n)) > maximalExtensionDeviation_) {
        hasReachedPosition = false;
      }
    }
    
    std::vector<bool> forwards = {false, false, false, false, false, false};
    std::vector<double> speeds = maximalSpeeds;
    while (!hasReachedPosition && !stopReachExtension_) {
      for (std::size_t n = 0; n < numberOfActuators_; ++n) {  
        const double deviation = currentExtension.at(n) - extensions.at(n);
      
        if (std::abs(deviation) <= maximalExtensionDeviation_) {
          speeds.at(n) = 0.0;
        } else {
          speeds.at(n) = std::max(0.25, maximalSpeeds.at(n) * std::min(1.0, 2.0 * deviation));
          
          if (deviation > 0) {
            forwards.at(n) = false;
          } else {
            forwards.at(n) = true;
          }
        }
      }
      
      servoControllers_.run(forwards, speeds);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      currentExtension = extensionSensors_.measure();
    
      hasReachedPosition = true;
      for (std::size_t n = 0; n < numberOfActuators_; ++n) { 
        if (std::abs(extensions.at(n) - currentExtension.at(n)) > maximalExtensionDeviation_) {
          hasReachedPosition = false;
        }
      }
    }
    
    servoControllers_.stop();
  }
  
  void LinearActuators::setMinimalExtension(
      const double minimalExtension) {
    if (!std::isfinite(minimalExtension)) {
      throw std::domain_error("LinearActuators.setMinimalExtension: The minimal extension must be finite.");
    }
    
    minimalExtension_ = minimalExtension;
  }
  
  double LinearActuators::getMinimalExtension() const {
    return minimalExtension_;
  }

  void LinearActuators::setMaximalExtension(
      const double maximalExtension) {
    if (!std::isfinite(maximalExtension)) {
      throw std::domain_error("LinearActuators.setMaximalExtension: The maximal extension must be finite.");
    }
    
    maximalExtension_ = maximalExtension;
  }
  
  double LinearActuators::getMaximalExtension() const {
    return maximalExtension_;
  }

  void LinearActuators::setMaximalExtensionDeviation(
      const double maximalExtensionDeviation) {
    if (!std::isfinite(maximalExtensionDeviation)) {
      throw std::domain_error("LinearActuators.setMaximalExtensionDeviation: The maximal extension deviation must be finite.");
    }
    
    maximalExtensionDeviation_ = maximalExtensionDeviation;
  }
  
  double LinearActuators::getMaximalExtensionDeviation() const {
    return maximalExtensionDeviation_;
  }

  void LinearActuators::setMinimalSpeed(
      const double minimalSpeed) {
    if (!std::isfinite(minimalSpeed)) {
      throw std::domain_error("LinearActuators.setMinimalSpeed: The minimal speed must be finite.");
    }
    
    minimalSpeed_ = minimalSpeed; 
  }
  
  double LinearActuators::getMinimalSpeed() const {
    return minimalSpeed_;
  }

  void LinearActuators::setMaximalSpeed(
      const double maximalSpeed) {
    if (!std::isfinite(maximalSpeed)) {
      throw std::domain_error("LinearActuators.setMaximalSpeed: The maximal speed must be finite.");
    }
    
    maximalSpeed_ = maximalSpeed;
  }
  
  double LinearActuators::getMaximalSpeed() const {
    return maximalSpeed_;
  }
}
#include "demonstrator_bits/linearActuators.hpp"

namespace demo {
  LinearActuators::LinearActuators(
      std::vector<Pin> directionPins,
      I2c i2c,
      const std::vector<unsigned int>& i2cChannels,
      Spi spi,
      const std::vector<unsigned int>& spiChannels)
    : servoControllers_(std::move(directionPins), std::move(i2c), i2cChannels),
      extensionSensors_(std::move(spi), spiChannels) {
    if (servoControllers_.numberOfControllers_ != extensionSensors_.numberOfSensors_) {
      throw std::logic_error("LinearActuators: The number of controllers must be equal to the number of sensors.");
    }
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
    
  void LinearActuators::setExtensionCorrections(
      const std::vector<arma::Col<double>>& extensionCorrections) {
    if (extensionCorrections.size() != servoControllers_.numberOfControllers_) {
      throw std::domain_error("LinearActuators.setExtensionCorrections: The number of extension corrections must be equal to the number of controllers.");
    }
    for (const auto& extensionCorrection : extensionCorrections) {
      if (!extensionCorrection.is_finite()) {
        throw std::domain_error("LinearActuators.setExtensionCorrections: All extension corrections must be finite.");
      }
    }
    
    extensionCorrections_ = extensionCorrections;
  }
  
  std::vector<arma::Col<double>> LinearActuators::getExtensionCorrections() const {
    return extensionCorrections_;
  }
  
  void LinearActuators::setSpeedCorrections(
      const std::vector<arma::Col<double>>& speedCorrections) {
    if (speedCorrections.size() != servoControllers_.numberOfControllers_) {
      throw std::domain_error("LinearActuators.setSpeedCorrections: The number of speed corrections must be equal to the number of controllers.");
    }
    for (const auto& speedCorrection : speedCorrections) {
      if (!speedCorrection.is_finite()) {
        throw std::domain_error("LinearActuators.setSpeedCorrections: All speed corrections must be finite.");
      }
    }
    
    speedCorrections_ = speedCorrections;
  }
      
  std::vector<arma::Col<double>> LinearActuators::getSpeedCorrections() const {
    return speedCorrections_;
  }
}
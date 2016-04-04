#include "demonstrator_bits/linearActuators.hpp"

namespace demo {
  void LinearActuators::setMinimalExtension(
      const double minimalExtension) {
    minimalExtension_ = minimalExtension;
  }
  
  double LinearActuators::getMinimalExtension() const {
    return minimalExtension_;
  }

  void LinearActuators::setMaximalExtension(
      const double maximalExtension) {
    maximalExtension_ = maximalExtension;
  }
  
  double LinearActuators::getMaximalExtension() const {
    return maximalExtension_;
  }

  void LinearActuators::setMinimalSpeed(
      const double minimalSpeed) {
    minimalSpeed_ = minimalSpeed; 
  }
  
  double LinearActuators::getMinimalSpeed() const {
    return minimalSpeed_;
  }

  void LinearActuators::setMaximalSpeed(
      const double maximalSpeed) {
    maximalSpeed_ = maximalSpeed;
  }
  
  double LinearActuators::getMaximalSpeed() const {
    return maximalSpeed_;
  }
    
  void LinearActuators::setExtensionCorrections(
      const std::vector<arma::Col<double>>& extensionCorrections) {
    extensionCorrections_ = extensionCorrections;
  }
  
  std::vector<arma::Col<double>> LinearActuators::getExtensionCorrections() const {
    return extensionCorrections_;
  }
  
  void LinearActuators::setSpeedCorrections(
      const std::vector<arma::Col<double>>& speedCorrections) {
    speedCorrections_ = speedCorrections;
  }
      
  std::vector<arma::Col<double>> LinearActuators::getSpeedCorrections() const {
    return speedCorrections_;
  }
}
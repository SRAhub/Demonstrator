#include "demonstrator_bits/stewartPlatform.hpp"

namespace demo {
  StewartPlatform::StewartPlatform(
      LinearActuators linearActuators)
    : linearActuators_(std::move(linearActuators)) {
      
  }
  
  void StewartPlatform::setEndEffectorPose(
      const arma::Col<double>::fixed<6>& endEffectorPose) {
    arma::Mat<double>::fixed<3, 6> baseJointsPiosition({
       -0.0302769856567722, -0.0664251770004387, 0.023,
       0.0726643835616438, -0.0069919497714374, 0.023,
       0.0726643835616438, 0.0069919497714374, 0.023,
       -0.0302769856567722, 0.0664251770004387, 0.023,
       -0.0423873979048716, 0.0594332272290012, 0.023,
       -0.0423873979048716, -0.0594332272290012, 0.023
    });

    arma::Mat<double>::fixed<3, 6> relativeEndeffectorJointsPosition({
      0.0302769856567722, -0.0664251770004387, -0.023,
      0.0423873979048716, -0.0594332272290012, -0.023,
      0.0423873979048716, 0.0594332272290012, -0.023,
      0.0302769856567722, 0.0664251770004387, -0.023,
      -0.0726643835616438, 0.0069919497714374, -0.023,
      -0.0726643835616438, -0.0069919497714374, -0.023
    });
    
    std::vector<double> extensions = arma::conv_to<std::vector<double>>::from(arma::sqrt(arma::sum(arma::square(static_cast<arma::Mat<double>>(relativeEndeffectorJointsPosition + endEffectorPose.head(3) + 0.22 - baseJointsPiosition)))));
    
    linearActuators_.setExtensions(extensions, std::vector<double>(extensions.size(), 1.0));
  }
  
  void StewartPlatform::setRelativeEndEffectorPose(
      const arma::Col<double>::fixed<6>& endEffectorPose) {
      
  }
}
#pragma once

// C++ standard library
#include <vector>

// Armadillo
#include <armadillo>

namespace demo {
  namespace pre_mant {
    extern double machinePrecision;
    
    std::vector<arma::Col<double>::fixed<3>> circleSphereIntersections(
        const arma::Col<double>::fixed<3>& circleCentre,
        const double circleRadius,
        const arma::Col<double>::fixed<3>& circleNormal,
        const arma::Col<double>::fixed<3>& sphereCentre,
        const double sphereRadius);
  }
}
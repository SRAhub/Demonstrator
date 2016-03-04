#pragma once

// Armadillo
#include <armadillo>

namespace demo {
  class Mouse3D {
   public:
    explicit Mouse3D();

    arma::Col<double>::fixed<6> getDisplacement() const;

   protected:
    int fileDescriptor_;
  };
}

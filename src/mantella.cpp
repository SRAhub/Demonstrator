#include "demonstrator_bits/mantella.hpp"

namespace demo {
  namespace pre_mant {
    double machinePrecision(1e-12);
    
    std::vector<arma::Col<double>::fixed<3>> circleSphereIntersections(
        const arma::Col<double>::fixed<3>& circleCentre,
        const double circleRadius,
        const arma::Col<double>::fixed<3>& circleNormal,
        const arma::Col<double>::fixed<3>& sphereCentre,
        const double sphereRadius) {
      if (!circleCentre.is_finite()) {
        throw std::domain_error("circleSphereIntersections: The circle centre must be finite.");
      } else if (!std::isfinite(circleRadius)) {
        throw std::domain_error("circleSphereIntersections: The circle radius must be finite.");
      } else if (circleRadius < 0) {
        throw std::domain_error("circleSphereIntersections: The circle radius must be positive (including 0).");
      } else if (!circleNormal.is_finite()) {
        throw std::domain_error("circleSphereIntersections: The circle normal must be finite.");
      } else if (!sphereCentre.is_finite()) {
        throw std::domain_error("circleSphereIntersections: The sphere centre must be finite.");
      } else if (!std::isfinite(sphereRadius)) {
        throw std::domain_error("circleSphereIntersections: The sphere radius must be finite.");
      } else if (sphereRadius < 0) {
        throw std::domain_error("circleSphereIntersections: The sphere radius must be positive (including 0).");
      }

      /* The circle sphere intersection points are calculated as follows:
       * 0. We assume that both centers are on the x-axis, ´circleNormal` is perpendicular to the x- and y-axis and `circleCentre` is at (0, 0, 0). 
       *    **Note:** This assumptions are lifted later on.
       * 1. Calculate the shortest distance between the sphere's centre and circle's plane.
       * 2. Calculate the centre of the sphere's circle segment, placed an the same plane as the given circle.
       * 3. Calculate the radius of the sphere's circle segment.
       *    Given the provided circle and the sphere's circle, the problem is now reduced to a circle circle intersection.
       * 4. Calculate the x-part of the intersection point.
       *    At this point, we either got 2 intersection points, when the distance between the x-part of the intersection point and `circleCentre` is less than `circleRadius`, 1 intersection point if both are equal and no intersection if its greater.
       *
       * A: We got 1 intersection
       *   5. Generate a unit vector pointing from `circleCentre` to `innerCentre`.
       *   6. Set the y-part to 0.
       *   7. Scale and translate the unit vector to be within the actual coordinate system (removing the assumption from 1.).
       *   
       * B: We got 2 intersections
       *   5. Generate a unit vector pointing from `circleCentre` to `innerCentre`.
       *   6. Calculate the y-part.
       *   7. Generate a second unit vector, perpendicular to `xUnitVector` and `circleNormal`.
       *   8. Scale and translate both unit vectors to be within the actual coordinate system (removing the assumption from 1.).
       */

      const double innerDistance = arma::dot(circleNormal, circleCentre - sphereCentre);
      if (std::abs(innerDistance) > sphereRadius + ::demo::pre_mant::machinePrecision) {
        // The circle's plane does not intersect with the sphere.
        return {};
      };

      const arma::Col<double>::fixed<3>& innerCentre = sphereCentre + innerDistance * circleNormal;
      // Due to rounding errors, the inner result might be negative instead of being 0.
      const double innerRadius = std::sqrt(std::max(0.0, std::pow(sphereRadius, 2.0) - std::pow(innerDistance, 2.0)));

      const double distance = arma::norm(innerCentre - circleCentre);
      if (distance < ::demo::pre_mant::machinePrecision && std::abs(innerRadius - circleRadius) < ::demo::pre_mant::machinePrecision) {
        // Both circles are identical ...
        if (circleRadius > ::demo::pre_mant::machinePrecision) {
          // ... but have a non-zero radius.
          throw std::invalid_argument("circleSphereIntersections: Both centers and radii (> 0) are identical, resulting in infinite intersections.");
        }

        // ... and dots.
        return {circleCentre};
      }

      if (distance > circleRadius + innerRadius + ::demo::pre_mant::machinePrecision || distance < std::abs(circleRadius - innerRadius) - ::demo::pre_mant::machinePrecision) {
        // Both circles are either to far away or to close.
        return {};
      };

      const double x = (std::pow(circleRadius, 2.0) - std::pow(innerRadius, 2.0) + std::pow(distance, 2.0)) / (2.0 * distance);
      const arma::Col<double>::fixed<3>& xUnitVector = (innerCentre - circleCentre) / distance;

      if (std::abs(circleRadius - std::abs(x)) < ::demo::pre_mant::machinePrecision) {
        // One intersection
        return {circleCentre + x * xUnitVector};
      } else {
        // Two intersections
        const double y = std::sqrt(std::pow(circleRadius, 2.0) - std::pow(x, 2.0));
        const arma::Col<double>::fixed<3>& yUnitVector = arma::normalise(arma::cross(xUnitVector, circleNormal));
        return {circleCentre + x * xUnitVector + y * yUnitVector, circleCentre + x * xUnitVector - y * yUnitVector};
      }
    } 
  } 
}
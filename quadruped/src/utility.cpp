/**
 * @file utility.cpp
 * utility functions
 */
#include "include/utility.h"

namespace Q1 {

// ---------------------------------------------------------------NormalizeAngle
/** @brief normalize an angle so it's in the (-pi, pi) range*/
double NormalizeAngle(double angle) {
  while (angle < -M_PI) {
    angle += M_2PI;
  }
  while (angle > M_PI) {
    angle -= M_2PI;
  }
  return angle;
}

}  // namespace Q1

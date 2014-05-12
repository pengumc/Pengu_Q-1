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

// ----------------------------------------------------------------CrossProduct3
/** @brief crossproduct of two 3 dimensional vectors
 * 
 * v_out = v_left x v_right
 */
void CrossProduct3(const double* v_left, const double* v_right,
                   double* v_out) {
    v_out[0] = v_left[1] * v_right[2] - v_left[2] * v_right[1];
    v_out[1] = v_left[2] * v_right[0] - v_left[0] * v_right[2];
    v_out[2] = v_left[0] * v_right[1] - v_left[1] * v_right[0];
}

// -------------------------------------------------------------VectorDotProduct
/** @brief dot product of two equally long vectors*/
double VectorDotProduct(const double* v1, const double* v2,
                       int element_count) {
  double out = 0.0;
  for (int i = 0; i < element_count; ++i) {
    out += v1[i] * v2[i];
  }
  return out;
}

}  // namespace Q1

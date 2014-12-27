/**
 * @file utility.h
 */

#ifndef QUADRUPED_INCLUDE_UTILITY_H_
#define QUADRUPED_INCLUDE_UTILITY_H_

#include <cmath>

#ifndef M_PI
  #define M_PI 3.14159265358979323846264338327950288
#endif

#ifndef M_2PI
  #define M_2PI 6.28318530717958647692528676655900576
#endif

namespace Q1 {

double NormalizeAngle(double angle);
void CrossProduct3(const double* v_left, const double* v_right,
                   double* v_out);
double VectorDotProduct(const double* v1, const double* v2,
                      int element_count);
double Get2DAngle(double x1, double x2, double y1, double y2);

}  // namespace Q1

#endif  // QUADRUPED_INCLUDE_UTILITY_H_

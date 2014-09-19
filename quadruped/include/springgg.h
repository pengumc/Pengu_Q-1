/**
 * @file springgg.h
 */

#ifndef QUADRUPED_INCLUDE_SPRINGGG_H_
#define QUADRUPED_INCLUDE_SPRINGGG_H_

#include <stdlib.h>
#include <stdio.h>

#include <cmath>

namespace Q1 {

/** @brief connection point for a spring */
class SpringPoint {
 public:
  SpringPoint();
  void ClearForce();
  void AddForce(double x, double y);
  void SetPosition(double x, double y);
  double x();
  double y();
  double Fx();
  double Fy();
 private:
  double Fx_;  ///< net force in x direction
  double Fy_;  ///< net force in y direction
  double x_;  ///< x position
  double y_;  ///< y position
};

/** @brief A spring, as in F = -K*x*/
class Spring {
 public:
  Spring();
  void Connect(SpringPoint* A, SpringPoint* B);
  void ZeroForce();
  SpringPoint* A();
  SpringPoint* B();
  double K();
  double CalculateForce();
 private:
  double K_;  ///< K factor
  SpringPoint* A_;  ///< connection point A
  SpringPoint* B_;  ///< connection point B
  double base_distance_;  ///< distance for which F = 0
};

/** @brief spring based gaitgenerator
 *
 * info needed: feet and com location on ground (xy) plane. K for each spring
 *
 *
 */
class SpringGG {
 public:
  static const int kSpringCount = 10;
  SpringGG();
  void SetFootPosition(int index, double x, double y);
  void SetCoMPosition(double x, double y);
  void ZeroForces();
  void CalculateForces();
  void PrintForces();
 private:
  SpringPoint feet_[4];
  SpringPoint com_;
  Spring springs[kSpringCount];
};



}  // namespace Q1

#endif  // QUADRUPED_INCLUDE_SPRINGGG_H_

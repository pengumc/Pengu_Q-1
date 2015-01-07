/**
 * @file springgg.h
 */

#ifndef QUADRUPED_INCLUDE_SPRINGGG_H_
#define QUADRUPED_INCLUDE_SPRINGGG_H_

#include <stdlib.h>
#include <stdio.h>

#include <cmath>

#include "include/utility.h"

namespace Q1 {

/** @brief connection point for a spring */
class SpringPoint {
 public:
  SpringPoint();
  void ClearForce();
  void AddForce(double x, double y);
  void SetPosition(double x, double y);
  void ChangePosition(double x, double y);
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
  void set_K(double new_K);
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
 * and base locations (for spring equilibrium). Added in this version are the
 * restpositions for each foot. These have a static relation to the cob but
 * since the cob is not included in this model, they should be updated at 
 * the same time as the com.
 *
 */
class SpringGG {
 public:
  static const int kMaxIter = 100;  ///< maximum iterations for MoveToForce
  static const double kMaxError = 0.1;  ///< maximum error for MoveToForce
  static const int kSpringCount = 14;  ///< number of springs
  SpringGG();
  void SetFootPosition(int index, double x, double y);
  void SetCoMPosition(double x, double y);
  void SetRestPosition(int index, double x, double y);
  void ZeroForces();
  void CalculateForces();
  bool IsInside(int index, double x, double y);
  bool CoMInside(int index, double margin);
  void PrintForces();
  int GetLegWithHighestForce(double direction_angle);
  void GetDeltaVector(int index, double angle, double F, double* vector_out);
  bool MoveToForce(int index, double Fx, double Fy);
 private:
  SpringPoint feet_[4];
  SpringPoint com_;
  SpringPoint restpos_[4];
  Spring springs[kSpringCount];
};

}  // namespace Q1

#endif  // QUADRUPED_INCLUDE_SPRINGGG_H_

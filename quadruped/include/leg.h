/**
 * @file leg.h
 */

#ifndef QUADRUPED_INCLUDE_LEG_H_
#define QUADRUPED_INCLUDE_LEG_H_

#include <stdint.h>
#include "include/pivot.h"
#include "include/hmatrix.h"
#include "include/invkinematic.h"

namespace Q1 {

/** @brief represents a leg of the quadruped
 * 
 * transform tree:<br>
 * H_0_cob -- pivots_[0] -- pivots_[1] -- pivots_[2] -- foot_<br>
 */
class Leg {
 public:
  // constants
  static const int kPivotCount = 3;/**< @brief number of joints in the leg*/
  static const int kMaxIter = 100;/**< @brief max iterations for IK*/
  // con/destructors
  explicit Leg(uint8_t index, HMatrix* H_cob);
  ~Leg();
  // functions
  const double* GetHMatrixArray(int index);
  const double* GetRelativeHMatrixArray(int index);
  void SetPivotPos(int index, double x, double y, double z);
  void SetPivotConfig(int index, double offset, double abs_max);
  void ConfigurePivotRot(int index, Axis axis, double angle);
  bool ChangePivotAngle(int index, double angle);
  bool ChangeFootPos(double dx, double dy, double dz);

 private:
  uint8_t index_;/**< @brief index of the leg (0..3)*/
  Pivot* pivots_[kPivotCount];/**< @brief pivots in the leg (excl foot)*/
  Pivot* foot_;/**< @brief pivot for the foot*/
  InvKinematic* ik_engine_;/**< @brief the inverse kinematics engine*/
};

}  // namespace Q1

#endif  // QUADRUPED_INCLUDE_LEG_H_

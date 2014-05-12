/**
 * @file invkinematic.h
 */

#ifndef QUADRUPED_INCLUDE_INVKINEMATIC_H_
#define QUADRUPED_INCLUDE_INVKINEMATIC_H_

#include <stdint.h>
#include <stdlib.h>

#include "include/pivot.h"

namespace Q1 {

/** @brief Inverse kinematics engine
 * 
 * supports only single end effectors, so jacobian = 1 x n x 3 big
 * ... that's actually the transposed jacobian?
 */
class InvKinematic {
 public:
  // types
  typedef enum {
    NOTHING_WRONG = 0,
    NOT_ALL_PIVOT_IN_CHAIN = 1
  } IKFlags;
  // de/constructors
  explicit InvKinematic(int pivot_count);
  ~InvKinematic();
  // functions
  void SetPivot(int pivot_index, Pivot* pivot);
  void SetTargetPos(double x, double y, double z);
  void ConstructJacobian();
  void Step();

 private:
  int pivot_count_;/**< @brief number of pivots in chain (n)*/
  Pivot** pivots_;/**< @brief array of pointers to the pivots in the chain*/
  double* jacobianT_;/**< @brief jacobian matrix values (n x 3 doubles)*/
  IKFlags flag_;/** @brief last known status of engine*/
  double target_[3];/**< @brief x,y,z of target pos*/
};

}  // namespace Q1

#endif  // QUADRUPED_INCLUDE_INVKINEMATIC_H_

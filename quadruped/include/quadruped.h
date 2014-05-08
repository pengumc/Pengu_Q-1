/**
 * @file quadruped.h
 */

#ifndef QUADRUPED_INCLUDE_QUADRUPED_H_
#define QUADRUPED_INCLUDE_QUADRUPED_H_

#include <stdint.h>
#include <cstddef>

#include "include/leg.h"

namespace Q1 {

/** @brief the main quadruped class
 *
 */
class Quadruped {
 public:
  // constants
  static const int kLegCount = 4;
  // con/destructors
  Quadruped();
  ~Quadruped();
  // functions
  const double* GetHMatrixArrayByIndex(int index);
  const double* GetEndpoint(int index);
  void SetPivotPos(int leg_index, int pivot_index, double x, double y,
                   double z);
  void ConfigurePivotRot(int leg_index, int pivot_index, Axis axis,
                         double angle);
 private:
  Leg* legs_[kLegCount]; /**< @brief leg pointers*/
};

}  // namespace Q1

#endif  // QUADRUPED_INCLUDE_QUADRUPED_H_

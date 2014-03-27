/**
 * @file quadruped.h
 */

#ifndef QUADRUPED_INCLUDE_QUADRUPED_H_
#define QUADRUPED_INCLUDE_QUADRUPED_H_

#include <cstddef>
#include <stdint.h>

#include "include/leg.h"

namespace Q1 {

/** @brief the main quadruped class
 * 
 */
class Quadruped {
 public:
  // constants
  const static int kLegCount = 4;
  // con/destructors
  Quadruped();
  ~Quadruped();
  // functions
  const double* GetPivotHMatrixArrayByIndex(int index);
  
 private:
  Leg* legs_[kLegCount]; /**< @brief leg pointers*/
};

}  // namespace Q1

#endif  // QUADRUPED_INCLUDE_QUADRUPED_H_

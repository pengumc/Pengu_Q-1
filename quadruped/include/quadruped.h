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

 private:
  Leg* legs_[kLegCount]; /**< @brief leg pointers*/
};

}  // namespace Q1

#endif  // QUADRUPED_INCLUDE_QUADRUPED_H_

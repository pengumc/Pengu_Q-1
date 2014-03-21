/**
 * @file pivot.h
 */

#ifndef QUADRUPED_INCLUDE_PIVOT_H_
#define QUADRUPED_INCLUDE_PIVOT_H_

#include <stdint.h>
#include "include/hmatrix.h"

namespace Q1 {

/** @brief represents a pivot of a leg
 * 
 */
class Pivot {
 private:
  double abs_max_angle_;/**< @brief the absolute maximum angle*/
  double angle_;/**< @brief the current angle*/
  HMatrix H_frame_;
  /**< @brief HMatrix for this pivot relative to the previous (or COB)*/
};

}  // namespace Q1

#endif  // QUADRUPED_INCLUDE_PIVOT_H_

/**
 * @file leg.h
 */

#ifndef QUADRUPED_INCLUDE_LEG_H_
#define QUADRUPED_INCLUDE_LEG_H_

#include <stdint.h>
#include "include/pivot.h"

namespace Q1 {

/** @brief represents a leg of the quadruped
 * 
 */
class Leg {
 public:
  explicit Leg(uint8_t index);
 
 private:
  uint8_t index_;/** @brief index of the leg (0..3)*/
};

}  // namespace Q1

#endif  // QUADRUPED_INCLUDE_LEG_H_

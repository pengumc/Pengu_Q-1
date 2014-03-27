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
  Quadruped();
  ~Quadruped();
 private:
  Leg* legs_[4]; /**< @brief leg pointers*/
};

}  // namespace Q1

#endif  // QUADRUPED_INCLUDE_QUADRUPED_H_

/**
 * @file areas.h
 */

#ifndef ROSEGAITGENERATOR_INCLUDE_AREAS_H_
#define ROSEGAITGENERATOR_INCLUDE_AREAS_H_

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <cmath>

#include "include/rogg/log.h"

namespace ROGG {

/** @brief provides per bit storage and access to an array of bits
 * 
 * Usage:<br>
 * 1. create an instance with a size in bits 
 * \code BaseArea A = BaseArea(225); \endcode
 * The array starts out as all 0.<br>
 * - set any bit to 1 with \ref Set \code A.Set(32) \endcode
 * - clear any bit (set to 0) with \ref Clear \code A.Clear(32) \endcode
 * - set or clear all bits with \ref SetAll or \ref ClearAll
 * - check if there are any 1's in the array with \ref HasAtLeastOne 
 * \code bool at_least_one_one = A.HasAtLeastOne(); \endcode
 * - perform a logical AND with another area (make sure areas are same size):
 * \code 
 * BaseArea B = BaseArea(225);
 * A.ClearAll();
 * B.Set(1);
 * A.And(&B);
 * bool second_bit_of_A = A.Get(1); // is now a 1 
 * \endcode
 */
class BaseArea {
 public:
  // constants
  static const int kBits = 32;/**< @brief the number of bits in an uint32_t*/
  // constructors, destructors
  explicit BaseArea(int element_count);
  ~BaseArea();
  // methods
  bool Get(int search_array_entry);
  void Set(int search_array_entry);
  void Clear(int search_array_entry);
  void ClearAll();
  void SetAll();
  void LogArea(Logger* logger);
  void And(BaseArea* area);
  bool HasAtLeastOne();

 private:
  uint32_t* array_;/**< @brief bit storage*/
  int array_length_;/**< @brief length of \ref array_*/
  int element_count_;/**< @brief actual number of bits used from \ref array_*/
};

}  // namespace ROGG

#endif  // ROSEGAITGENERATOR_INCLUDE_AREAS_H_

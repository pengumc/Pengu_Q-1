/**
 * @file wrapper.cpp
 * extern "C" functions
 */

#include "include/quadruped.h"

namespace Q1 {

// --------------------------------------------------------------quadruped_alloc
/** @brief create a quadruped instance an return its pointer
 * 
 * Remeber to free the allocate memory again with \ref Quadruped_free when
 * you're done.
 */
extern "C" Quadruped* quadruped_alloc() {
  return new Quadruped();
}

// ---------------------------------------------------------------quadruped_free
/** @brief free the memory allocate by \ref quadruped_alloc */
extern "C" void quadruped_free(Quadruped* q) {
  delete q;
}

}  // namespace Q1

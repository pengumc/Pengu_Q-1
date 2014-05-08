/**
 * @file wrapper.cpp
 * extern "C" functions
 */

#include "include/quadruped.h"

namespace Q1 {

// --------------------------------------------------------------QuadrupedAlloc
/** @brief create a quadruped instance an return its pointer
 *
 * Remeber to free the allocate memory again with \ref QuadrupedFree when
 * you're done.
 */
extern "C" Quadruped* QuadrupedAlloc() {
  return new Quadruped();
}

// ---------------------------------------------------------------QuadrupedFree
/** @brief free the memory allocated by \ref QuadrupedAlloc */
extern "C" void QuadrupedFree(Quadruped* q) {
  delete q;
}

// ----------------------------------------------------------QuadrupedGetHMatrix
/** @brief return a pointer to the values of a hmatrix
 *
 * @param q pointer to a quadruped.
 * @param index The index of the HMatrix. numbering: <br>
 * 0 = leg 0, pivot 0<br>
 * 1 = leg 0, pivot 1<br>
 * 3 = leg 0, foot/endpoint<br>
 * 14 = leg 3, pivot 2<br>
 * 15 = leg 3, foot/endpoint<br>
 */
extern "C" const double* QuadrupedGetHMatrix(Quadruped* q, int index) {
  int mod = Leg::kPivotCount + 1;  // i.e. 4
  int l = index / mod;  // leg index
  int p = index % mod;  // pivot index
  if (p >= Leg::kPivotCount) {
    // 3rd pivot is endpoint
    return q->GetEndpoint(l);
  } else {
    return q->GetHMatrixArrayByIndex(l * Leg::kPivotCount + p);
  }
}

}  // namespace Q1

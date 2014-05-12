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
/** @brief return a pointer to the values of a hmatrix (pivots and feet)
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

// ---------------------------------------------------------QuadrupedSetPivotPos
/** @brief set x y and z coordinates of a pivot
 *
 * @param q pointer to a quadruped
 * @param leg_index index of the leg
 * @param pivot_index index of the pivot
 * @param x new x coordinate
 * @param y new y coordinate
 * @param z new z coordinate
 */
extern "C" void QuadrupedSetPivotPos(Quadruped* q, int leg_index,
                                     int pivot_index, double x, double y,
                                     double z) {
  q->SetPivotPos(leg_index, pivot_index, x, y, z);
}

// ---------------------------------------------------QuadrupedConfigurePivotRot
/** @brief configure the base rotation of a pivot. see
 * \ref Quadruped::ConfigurePivotRot*/
extern "C" void QuadrupedConfigurePivotRot(Quadruped* q, int leg_index,
                                        int pivot_index, Axis axis,
                                        double angle) {
  q->ConfigurePivotRot(leg_index, pivot_index, axis, angle);
}

// --------------------------------------------------QuadrupedGetRelativeHMatrix
/** @brief get the values of a hmatrix relative to the cob*/
extern "C" const double* QuadrupedGetRelativeHMatrix(Quadruped* q,
                                                     int leg_index,
                                                     int pivot_index) {
  return q->GetRelativeHMatrixArray(leg_index, pivot_index);
}

// ------------------------------------------------------QuadrupedSetPivotConfig
/** @brief set the offset and axis of a pivot
 *
 * @param q pointer to a quadruped
 * @param leg_index index for the leg (0..3)
 * @param pivot_index index of the pivot (0..3)
 * @param offset the offset angle (i.e. the neutral angle)
 * @param abs_max the absolute maximum angle relative to the offset for the 
 * pivot
 */
extern "C" void QuadrupedSetPivotConfig(Quadruped* q, int leg_index,
                                         int pivot_index, double offset,
                                         double abs_max) {
  q->SetPivotConfig(leg_index, pivot_index, offset, abs_max);
}

// --------------------------------------------------- QuadrupedChangePivotAngle
/** @brief change the angle of a pivot, false on out of range*/
extern "C" bool QuadrupedChangePivotAngle(Quadruped* q, int leg_index,
                                            int pivot_index, double angle) {
  return q->ChangePivotAngle(leg_index, pivot_index, angle);
}

}  // namespace Q1

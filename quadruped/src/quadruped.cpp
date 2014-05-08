/**
 * @file quadruped.cpp
 * main file
 */
#include "include/quadruped.h"

namespace Q1 {

// --------------------------------------------------------Constructor Quadruped
/** @brief Constructor*/
Quadruped::Quadruped() {
  for (uint8_t i = 0; i< kLegCount; ++i) {
    legs_[i] = new Leg(i);
  }
}

// ---------------------------------------------------------Destructor Quadruped
/** @brief Destructor*/
Quadruped::~Quadruped() {
  for (int i = 0; i < kLegCount; ++i) {
    delete legs_[i];
  }
}

// -------------------------------------------------------GetHMatrixArrayByIndex
/** @brief returns the \ref HMatrix::array_ of the HMatrix at index
 *
 * numbering: leg0 pivot0, leg0 pivot1 ... leg3 pivot2
 */
const double* Quadruped::GetHMatrixArrayByIndex(int index) {
  if (index < kLegCount * Leg::kPivotCount) {  // index < 11
    Leg* L = legs_[index / Leg::kPivotCount];
    return L->GetHMatrixArray(index % Leg::kPivotCount);
  } else {
    return NULL;
  }
}

// ------------------------------------------------------------------GetEndpoint
/** @brief returns the \ref HMatrix::array_ of the foot of leg index
 *
 * @param index index of the leg
 */
const double* Quadruped::GetEndpoint(int index) {
  if (index < kLegCount) {  // index < 4
    return legs_[index]->GetHMatrixArray(Leg::kPivotCount);
  } else {
    return NULL;
  }
}

// ------------------------------------------------------------------SetPivotPos
/** @brief set the x,y,z coordinates of a pivot of a leg*/
void Quadruped::SetPivotPos(int leg_index, int pivot_index, double x, double y,
                            double z) {
  legs_[leg_index]->SetPivotPos(pivot_index, x, y, z);
}

}  // namespace Q1

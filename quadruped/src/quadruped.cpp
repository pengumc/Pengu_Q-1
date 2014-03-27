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
/** @brief returns the array_ of the HMatrix at index
 * 
 * numbering: leg0 pivot0, leg0 pivot1 ... leg3 pivot2
 */
// TODO(michiel): rewrite
const double* Quadruped::GetHMatrixArrayByIndex(int index) {
  if (index < kLegCount * Leg::kPivotCount) {  // index < 11
    const 
    return legs_[index / Leg::kPivotCount]->pivot(index % Leg::kPivotCount);
  } else {
    return NULL;
  }
}

}  // namespace Q1

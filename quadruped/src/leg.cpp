/**
 * @file leg.cpp
 * leg functions
 */
#include "include/leg.h"

namespace Q1 {

// --------------------------------------------------------------Constructor Leg
/** @brief Constructor*/
Leg::Leg(uint8_t index) {
  index = index;
  for (int i = 0; i < kPivotCount; ++i) {
    pivots_[i] = new Pivot();
  }
  foot_ = new Pivot();
}


// ---------------------------------------------------------------Destructor Leg
/** @brief Destructor*/
Leg::~Leg() {
  for (int i = 0; i < kPivotCount; ++i) {
    delete pivots_[i];
  }
  delete foot_;
}

// --------------------------------------------------------------Accessor Pivots
/** @brief returns a pivot*/
const Pivot* Leg::pivot(int index) {
  return pivots_[index];
}

// ----------------------------------------------------------------Accessor foot
/** @brief returns the foot*/
const Pivot* Leg::foot() {
  return foot_;
}

// --------------------------------------------------------------GetHMatrixArray
/** @brief returns the HMatrix array of one of the pivots or the foot
 * 
 * index = 0..2: pivots<br>
 * index >= 3 : foot
 */
const double* Leg::GetHMatrixArray(int index) {
  if (index < kPivotCount) {
    return pivots_[index]->GetHMatrixArray();
  } else {
    return foot_->GetHMatrixArray();
  }
}


}  // namespace Q1

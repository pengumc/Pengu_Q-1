/**
 * @file leg.cpp
 * leg functions
 */
#include "include/leg.h"

namespace Q1 {

// --------------------------------------------------------------Constructor Leg
/** @brief Constructor*/
Leg::Leg(uint8_t index, HMatrix* H_cob) {
  index = index;
  pivots_[0] = new Pivot(H_cob);
  for (int i = 1; i < kPivotCount; ++i) {
    pivots_[i] = new Pivot(pivots_[i-1]->H_framep());
  }
  foot_ = new Pivot(pivots_[kPivotCount-1]->H_framep());
}


// ---------------------------------------------------------------Destructor Leg
/** @brief Destructor*/
Leg::~Leg() {
  for (int i = 0; i < kPivotCount; ++i) {
    delete pivots_[i];
  }
  delete foot_;
}

// --------------------------------------------------------------GetHMatrixArray
/** @brief returns the HMatrix array of one of the pivots or the foot
 *
 * index = 0..2: pivots<br>
 * index >= \ref kPivotCount : foot
 */
const double* Leg::GetHMatrixArray(int index) {
  if (index < kPivotCount) {
    return pivots_[index]->GetHMatrixArray();
  } else {
    return foot_->GetHMatrixArray();
  }
}

// ------------------------------------------------------------------SetPivotPos
/** @brief set the x,y,z coordinates of a pivot h-matrix*/
void Leg::SetPivotPos(int index, double x, double y, double z) {
  if (index < kPivotCount) {
    pivots_[index]->SetPosition(x, y, z);
  } else if (index == kPivotCount) {
    foot_->SetPosition(x, y, z);
  }
}

// ------------------------------------------------------------ConfigurePivotRot
/** @brief configure <b>R</b> of a pivot without touching Pivot::angle_*/
void Leg::ConfigurePivotRot(int index, Axis axis, double angle) {
  if (index < kPivotCount) {
    pivots_[index]->ConfigureRot(axis, angle);
  }
}

// ------------------------------------------------------GetRelativeHMatrixArray
/** @brief return the hmatrix array of one of the pivots or the foot relative
 * to the COB*/
const double* Leg::GetRelativeHMatrixArray(int index) {
  if (index < kPivotCount) {
    return pivots_[index]->GetRelativeHMatrixArray();
  } else {
    return foot_->GetRelativeHMatrixArray();
  }
}

// ---------------------------------------------------------------SetPivotConfig
/** @brief set the offset and rotational angle of a pivot
 *
 * @param index the pivot index, 0..2
 * @param offset the offset angle \ref Pivot::offset_angle_
 * @param abs_max the absolute maximum angle \ref Pivot::abs_max_angle_
 */
void Leg::SetPivotConfig(int index, double offset, double abs_max) {
  if (index < kPivotCount) {
    pivots_[index]->set_offset_angle(offset);
    pivots_[index]->set_abs_max_angle(abs_max);
  } else if (index == kPivotCount) {
    foot_->set_offset_angle(offset);
    foot_->set_abs_max_angle(abs_max);
  }
}

// -------------------------------------------------------------ChangePivotAngle
/** @brief change the angle of a pivot, false on out of bounds
 * 
 * index > kPivotCount returns always false
 */
bool Leg::ChangePivotAngle(int index, double angle) {
  if (index < kPivotCount) {
    pivots_[index]->ChangeAngle(angle);
  } else {
    return false;
  }
}

}  // namespace Q1

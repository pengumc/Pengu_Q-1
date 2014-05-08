/**
 * @file pivot.cpp
 * pivot functions
 */
#include "include/pivot.h"

namespace Q1 {

// ------------------------------------------------------------Constructor Pivot
/** @brief Constructor*/
Pivot::Pivot() {
  abs_max_angle_ = 30 * M_PI/180;  // 30 degrees
  angle_ = 0.0;
  offset_angle_ = 0.0;
  axis_ = X_AXIS;
}

// ------------------------------------------------------------set_abs_max_angle
/** @brief mutator for \ref abs_max_angle_*/
void Pivot::set_abs_max_angle(double angle) {
  abs_max_angle_ = NormalizeAngle(angle);
  // TODO(michiel): update angle and hmatrix if we fall out of range
}

// -------------------------------------------------------------set_offset_angle
/** @brief mutator for \ref offset_angle_*/
void Pivot::set_offset_angle(double angle) {
  offset_angle_ = NormalizeAngle(angle);
  // TODO(michiel): update angle and hmatrix
}

// --------------------------------------------------------------------set_angle
/** @brief mutator for \ref angle_. false if angle is out of range*/
bool Pivot::set_angle(double angle) {
  return ChangeAngle(angle - angle_);
}

// ---------------------------------------------------------------------set_axis
/** @brief mutator for \ref axis_
 * 
 * Changing the axis resets \ref abs_max_angle_, \ref offset_angle_ and 
 * \ref angle_ to 0.0;
 */
void Pivot::set_axis(Axis axis) {
  axis_ = axis;
  angle_ = 0.0;
  abs_max_angle_ = 0.0;
  offset_angle_ = 0.0;
  ChangeAngle(0.0);
}

// ------------------------------------------------------------------SetPosition
/** @brief Set x y z relative to previous pivot or COB*/
void Pivot::SetPosition(double x, double y, double z) {
  H_frame_.SetX(x);
  H_frame_.SetY(y);
  H_frame_.SetZ(z);
}

// ------------------------------------------------------------------ChangeAngle
/** @brief change the angle of rotation. false if new angle is out of range
 * 
 * if false is returned, nothing has changed.
 */
bool Pivot::ChangeAngle(double delta_angle) {
  const double a = offset_angle_ + angle_ + delta_angle;
  if (!IsInRange(a)) {
    return false;
  } else {
    H_frame_.SelfDot(HMatrix(axis_, delta_angle));
    return true;
  }
}

// --------------------------------------------------------------------IsInRange
/** @brief true if angle is within the range specified by the offset and max
 * 
 * \image html angles.png
 */
bool Pivot::IsInRange(double angle) {
  const double test = NormalizeAngle(angle - offset_angle_);
  return (test <= abs_max_angle_ && test >= -abs_max_angle_);
}

// --------------------------------------------------------------GetHMatrixArray
/** @brief return a readable 16 value array with the elements of \ref H_frame_*/
const double* Pivot::GetHMatrixArray() {
  return H_frame_.array();
}

}  // namespace Q1

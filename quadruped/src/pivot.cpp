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
}

// -----------------------------------------------------------------Constructor2
/** @brief Constructor, set parent*/
Pivot::Pivot(HMatrix* parent) {
  // same as normal constructor
  abs_max_angle_ = 30 * M_PI/180;  // 30 degrees
  angle_ = 0.0;
  offset_angle_ = 0.0;
  // just add parent
  H_frame_.set_parent(parent);
}

// ----------------------------------------------------------------------H_frame
/** @brief accessor for \ref H_frame_*/
HMatrix Pivot::H_frame() {
  return H_frame_;
}

// ---------------------------------------------------------------------H_framep
/** @brief returns a pointer to \ref H_frame_ (for use as parent)*/
HMatrix* Pivot::H_framep() {
  return &H_frame_;
}

// ------------------------------------------------------------------------H_rel
/** @brief accessor for \ref H_rel_ (update H_rel as well)*/
HMatrix Pivot::H_rel() {
  // first update rel by traversing the tranform tree till we find a matrix
  // with parent NULL
  HMatrix* p = H_frame_.parent();
  if (p == NULL) {
    // H_frame_ is already relative to origin
    H_rel_.Copy(H_frame_);
  } else {
    H_rel_ = H_frame_.Inverse();
    while (p != NULL) {
      H_rel_.SelfDot(p->Inverse());
      p = p->parent();
    }
    H_rel_ = H_rel_.Inverse();
  }
  return H_rel_;
}

// -------------------------------------------------------------------set_parent
/** @brief mutator for \ref parent_*/
void Pivot::set_parent(HMatrix* parent) {
  H_frame_.set_parent(parent);
}

// ------------------------------------------------------------set_abs_max_angle
/** @brief mutator for \ref abs_max_angle_
 *
 * \ref angle_ is reset to 0.0 
 */ 
void Pivot::set_abs_max_angle(double angle) {
  abs_max_angle_ = std::abs(NormalizeAngle(angle));
  set_angle(0.0);
}

// -------------------------------------------------------------set_offset_angle
/** @brief mutator for \ref offset_angle_
 * 
 * \image offsetchange.png
 */
void Pivot::set_offset_angle(double angle) {
  const double diff = angle - offset_angle_;
  H_frame_.SelfDot(HMatrix(Z_AXIS, diff));
  offset_angle_ = NormalizeAngle(angle);
}

// --------------------------------------------------------------------set_angle
/** @brief mutator for \ref angle_. false if angle is out of range*/
bool Pivot::set_angle(double angle) {
  return ChangeAngle(angle - angle_);
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
 * if false is returned, nothing has changed.<br>
 * \ref angle_ is kept up to date
 */
bool Pivot::ChangeAngle(double delta_angle) {
  const double a = offset_angle_ + angle_ + delta_angle; // target true angle
  if (!IsInRange(a)) {
    return false;
  } else {
    H_frame_.SelfDot(HMatrix(Z_AXIS, delta_angle));
    angle_ += delta_angle;
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

// ------------------------------------------------------GetRelativeHMatrixArray
/** @vbrief return a readable 16 value array with the elements of \ref H_rel_*/
const double* Pivot::GetRelativeHMatrixArray() {
  H_rel();  // update H_rel_
  return H_rel_.array();
}

// -----------------------------------------------------------------ConfigureRot
/** @brief configre the base orientation of the pivot
 * 
 * This resets the offset and the angle back to 0
 */
void Pivot::ConfigureRot(Axis axis, double angle) {
  set_offset_angle(0.0);
  set_angle(0.0);
  H_frame_.SelfDot(HMatrix(axis, angle));
}

}  // namespace Q1

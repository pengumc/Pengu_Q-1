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
  mass_ = 1.0;
  pw_0_ = 1500e-6;
  pw_60_ = 1000e-6;
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
  mass_ = 1.0;
  pw_0_ = 1500e-6;
  pw_60_ = 1000e-6;
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

// ------------------------------------------------------------------updateH_rel
/** @brief accessor for \ref H_rel_ (updates H_rel as well)*/
void Pivot::UpdateH_rel() {
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
}

// ---------------------------------------------------------------angle accessor
/** @brief \ref angle_ accessor*/
double Pivot::angle() {
  return angle_;
}

// -------------------------------------------------------abs_max_angle accessor
/** @brief \ref abs_max_angle_ accessor*/
double Pivot::abs_max_angle() {
  return abs_max_angle_;
}

// -----------------------------------------------------------------GetSpecificH
/** @brief a h-matrix from this to a specified matrix in its upward tree path
 * 
 * @param target pointer to a target hmatrix. if target is not in the upward
 * path, the parent will not be set.
 * @return H_this_target or a parentless HMatrix if target was not encountered
 */
HMatrix Pivot::GetSpecificH(HMatrix* target) {
  HMatrix* p = H_frame_.parent();
  if (p == NULL && target != NULL) {
    return HMatrix();
  } else {
    HMatrix H_this_target = H_frame_.Inverse();
    while (p != target) {
      H_this_target.SelfDot(p->Inverse());
      p = p->parent();
      if (p == NULL) {
        return HMatrix();
      }
    }
    H_this_target.set_parent(&H_frame_);
    return H_this_target;
  }
}

// -------------------------------------------------------------------set_parent
/** @brief mutator for \ref HMatrix::parent_, NULL means origin*/
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
 * \image html offsetchange.png
 */
void Pivot::set_offset_angle(double angle) {
  // const double diff = angle - offset_angle_;
  // H_frame_.SelfDot(HMatrix(Z_AXIS, diff));
  offset_angle_ = NormalizeAngle(angle);
}

// --------------------------------------------------------------------set_angle
/** @brief mutator for \ref angle_. false if angle is out of range*/
bool Pivot::set_angle(double angle) {
  return ChangeAngle(angle - angle_);
  // TODO(michiel): should offset be included here?
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
// const double a = offset_angle_ + angle_ + delta_angle;  // target true angle
  const double a = angle_ + delta_angle;  // target angle
  if (!IsInRange(a)) {
    return false;
  } else {
    H_frame_.SelfDot(HMatrix(Z_AXIS, delta_angle));
    angle_ = NormalizeAngle(angle_ + delta_angle);
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
/** @brief return a readable 16 value array with the elements of \ref H_rel_*/
const double* Pivot::GetRelativeHMatrixArray() {
  UpdateH_rel();
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

// ---------------------------------------------------------------------set_mass
/** mutator for \ref mass_ */
void Pivot::set_mass(double mass) {
  if (mass  > 0.0) mass_ = mass;
}

// ----------------------------------------------------------------mass accessor
/** Accessor for \ref mass_ */
double Pivot::mass() {
  return mass_;
}

// -----------------------------------------------------------GetServoPulsewidth
/** @brief calculate a proper servo pulsewidth to set a specific angle 
 *
 * pw = pw<sub>0</sub> - (angle / full range) * (pw<sub>max</sub -
 * pw<sub>min</sub>
 */
double Pivot::GetServoPulsewidth(double angle) {
  return pw_0_ + angle / k60Range * (pw_60_ - pw_0_); 
}

// -----------------------------------------------------------GetServoPulsewidth
/** @brief calls \ref GetServoPulsewidth with \ref angle_ */
double Pivot::GetServoPulsewidth() {
  return GetServoPulsewidth(angle_);
}

// ----------------------------------------------------------SetPulsewidthConfig
/** @brief set \ref pw_0_ and \ref pw_60_ */
void Pivot::SetPulsewidthConfig(double pw_0, double pw_60) {
  if (pw_0_ > 0.0 ) pw_0_ = pw_0;
  if (pw_60_ > 0.0 ) pw_60_ = pw_60;
}

// -------------------------------------------------------GetAngleFromPulsewidth
/** @brief calculate what angle a given pulsewidth should cause */
double Pivot::GetAngleFromPulsewidth(double pulsewidth) {
  return (pulsewidth - pw_0_ ) / (pw_60_ - pw_0_) * k60Range;
}


}  // namespace Q1

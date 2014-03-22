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
void set_abs_max_angle(double angle) {
  abs_max_angle_ = angle;
  // TODO(michiel): update angle and hmatrix if we fall out of range
}

// -------------------------------------------------------------set_offset_angle
/** @brief mutator for \ref offset_angle_*/
void set_offset_angle(double angle) {
  offset_angle_ = angle;
  // TODO(michiel): update angle and hmatrix
}

// --------------------------------------------------------------------set_angle
/** @brief mutator for \ref angle_. false if angle is out of range*/
bool set_angle(double angle) {
  return ChangeAngle(angle - angle_);
}

// ---------------------------------------------------------------------set_axis
/** @brief mutator for \ref axis_*/
void set_axis(Axis axis) {
  axis_ = axis;
  // TODO(michiel): rebuild Hmatrix
}

// ------------------------------------------------------------------SetPosition
/** @brief Set x y z relative to previous pivot or COB*/
void SetPosition(double x, double y, double z) {
  H_frame_.SetX(x);
  H_frame_.SetY(y);
  H_frame_.SetZ(z);
}

// ------------------------------------------------------------------ChangeAngle
/** @brief change the angle of rotation. false if new angle is out of range*/
bool ChangeAngle(double delta_angle) {
  const double a = offset_angle_ + angle_ + delta_angle;
  // TODO(michiel): do range check
  switch (axis_) {
    case Z_AXIS: {
      const double newH[HMatrix::kMagic16] = [
        std::cos(a), -std::sin(a), 0, H_frame_.GetX(),
        std::sin(a),  std::cos(a), 0, H_frame_.GetY(),
        0,           0,            1, H_frame_.GetZ(),
        0,           0,            0, 1];
      H_frame_.set_array(&newH);
      break;
    }
    case Y_AXIS: {
      const double newH[HMatrix::kMagic16] = [
        std::cos(a), -std::sin(a), 0, H_frame_.GetX(),
        0,            1,           0, H_frame_.GetY(),
        std::sin(a),  std::cos(a), 1, H_frame_.GetZ(),
        0,            0,           0, 1];
      H_frame_.set_array(&newH);
      break;
    }
    case X_AXIS: {
      const double newH[HMatrix::kMagic16] = [
        1,            0,           0, H_frame_.GetX(),
        std::cos(a), -std::sin(a), 0, H_frame_.GetY(),
        std::sin(a),  std::cos(a), 1, H_frame_.GetZ(),
        0,            0,           0, 1];
      H_frame_.set_array(&newH);
      break;
    }
  }
}

}  // namespace Q1

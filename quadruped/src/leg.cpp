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
  ik_engine_ = new InvKinematic(kPivotCount + 1);
  pivots_[0] = new Pivot(H_cob);
  ik_engine_->SetPivot(0, pivots_[0]);
  for (int i = 1; i < kPivotCount; ++i) {
    pivots_[i] = new Pivot(pivots_[i-1]->H_framep());
    ik_engine_->SetPivot(i, pivots_[i]);
  }
  foot_ = new Pivot(pivots_[kPivotCount-1]->H_framep());
  ik_engine_->SetPivot(kPivotCount, foot_);
}


// ---------------------------------------------------------------Destructor Leg
/** @brief Destructor*/
Leg::~Leg() {
  for (int i = 0; i < kPivotCount; ++i) {
    delete pivots_[i];
  }
  delete foot_;
  delete ik_engine_;
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
/** @brief set the x,y,z coordinates of a pivot or foot h-matrix
 *
 * index \ref kPivotCount will be the \ref foot_
 */
void Leg::SetPivotPos(int index, double x, double y, double z) {
  if (index < kPivotCount) {
    pivots_[index]->SetPosition(x, y, z);
  } else if (index == kPivotCount) {
    foot_->SetPosition(x, y, z);
  }
  UpdateCoM();
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
 * to 0*/
const double* Leg::GetRelativeHMatrixArray(int index) {
  if (index < kPivotCount) {
    return pivots_[index]->GetRelativeHMatrixArray();
  } else {
    return foot_->GetRelativeHMatrixArray();
  }
}

/** @brief return the hmatrix transformation from the pivot at start_index to
 * the one at end_index
 */
HMatrix Leg::GetRelativeHMatrix(int start_index, int end_index) {
  if (start_index < end_index) {
    if (end_index == kPivotCount) {
      if (start_index == kPivotCount-1) {
        // just to the foot
        return HMatrix(foot_->H_frame().array());
      } else {
        // from start_index to end_index
        // first transform is to start_index+1
        HMatrix A(pivots_[start_index+1]->H_framep()->array());
        // rest, starting from start_index + 2
        int i = 0;
        for (i = start_index + 2; i < kPivotCount; ++i) {
          A.SelfDot(pivots_[i]->H_frame());
        }
        // are we not done yet?
        if (i-1 < end_index) {
          // also add foot
          A.SelfDot(foot_->H_frame());
        }
        return A;
      }
    }
  }
  return HMatrix();
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
    UpdateCoM();
    return true;
  } else {
    return false;
  }
}

// ----------------------------------------------------------------SetPivotAngle
/** @brief set the angle of a pivot, false on out of bounds
 *
 * index > kPivotCount returns always false
 */
bool Leg::SetPivotAngle(int index, double angle) {
  if (index < kPivotCount) {
    pivots_[index]->set_angle(angle);
    UpdateCoM();
    return true;
  } else {
    return false;
  }
}


// ----------------------------------------------------------------GetPivotAngle
/** @brief get the \ref Pivot::angle_ of a pivot
 *
 * @param index the index of the pivot
 */
double Leg::GetPivotAngle(int index) {
  return pivots_[index]->angle();
}

// -----------------------------------------------------SetPivotPulsewidthConfig
/** @brief sets \ref Pivot::pw_0_ and \ref Pivot::pw_60_
 * @param index 0..kPivotCount-1
 */
void Leg::SetPivotPulsewidthConfig(int index, double pw_0, double pw_60) {
  pivots_[index]->SetPulsewidthConfig(pw_0, pw_60);
}

// -----------------------------------------------------------GetPivotPulsewidth
/** @brief return what the pivot at index would need for pulsewidth to
  * cause its current angle
  *
  * @param index index of pivot 0..kPivotCount-1
  */
double Leg::GetPivotPulsewidth(int index) {
  return pivots_[index]->GetServoPulsewidth();
}

// -------------------------------------------------------GetAngleFromPulsewidth
/** @brief find the angle that a given pulsewidth would cause for the pivot at
 * index
  *
  * @param index index of pivot 0..kPivotCount-1
  * @param pulsewidth the value to find the angle for
 */
double Leg::GetAngleFromPulsewidth(int index, double pulsewidth) {
  return pivots_[index]->GetAngleFromPulsewidth(pulsewidth);
}

// ----------------------------------------------------------------ChangeFootPos
/** @brief change the position of a foot (relative to Origin) */
bool Leg::ChangeFootPos(double dx, double dy, double dz) {
  const double* H_0_s = foot_->GetRelativeHMatrixArray();
  ik_engine_->SetTargetPos(
    dx + H_0_s[HMatrix::kX],
    dy + H_0_s[HMatrix::kY],
    dz + H_0_s[HMatrix::kZ]);
  int r = ik_engine_->Iterate(kMaxIter);
  UpdateCoM();
  if (r == kMaxIter) {
    return false;
  } else if (ik_engine_->flag() == InvKinematic::NOTHING_WRONG) {
    return true;
  } else {
    return false;
  }
}

// --------------------------------------------------------------------UpdateCoM
/** @brief recalculates the center of mass and updates \ref H_com_
 *
 */
void Leg::UpdateCoM() {
  // position of center of mass = 1 / total_mass * sum(position[i] * mass[i])
  double m_i;
  const double* H;
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
  total_mass_ = 0.0;

  for (int i = 0; i < kPivotCount; ++i) {
    m_i = pivots_[i]->mass();
    H = GetRelativeHMatrixArray(i);  // everything in 0 frame
    x = x + H[HMatrix::kX] * m_i;
    y = y + H[HMatrix::kY] * m_i;
    z = z + H[HMatrix::kZ] * m_i;  // TODO(michiel): Z axis is not needed?
    total_mass_ = total_mass_ + m_i;
  }
  H_com_ = HMatrix(x / total_mass_, y / total_mass_, z / total_mass_);
}


// -----------------------------------------------------------------------GetCoM
/** @brief Get the latest \ref H_cob_com_ for this leg. use \ref UpdateCoM to
 *  recalculte.
 */
HMatrix Leg::GetCoM() {
  return H_com_;
}

// ---------------------------------------------------------------get_total_mass
/** @brief accessor for \ref total_mass_ */
double Leg::get_total_mass() {
  return total_mass_;
}

// ----------------------------------------------------------GetPivotAbsMaxAngle
/** @brief returns \ref Pivot::abs_max_angle_ for the pivot at index*/
double Leg::GetPivotAbsMaxAngle(int index) {
  return pivots_[index]->abs_max_angle();
}

}  // namespace Q1

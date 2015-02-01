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
    legs_[i] = new Leg(i, &H_cob_);
  }
  sgg_.SetCoMPosition(0.0, 0.0);
  for (uint8_t i = 0; i < kLegCount; ++i) {
    for (uint8_t j = 0; j < 3; ++j) {
      rest_vectors_[i][j] = 0.0;
    }
  }
}

// ---------------------------------------------------------Destructor Quadruped
/** @brief Destructor*/
Quadruped::~Quadruped() {
  for (int i = 0; i < kLegCount; ++i) {
    delete legs_[i];
  }
}

// -----------------------------------------------------SetPivotPulsewidthConfig
/** @brief calls Leg::SetPivotPulsewidthConfig
 *
 * pivot_index is in range 0..kPivotCount-1
 */
void Quadruped::SetPivotPulsewidthConfig(int leg_index, int pivot_index,
                                         double pw_0, double pw_60) {
  legs_[leg_index]->SetPivotPulsewidthConfig(pivot_index, pw_0, pw_60);
}

// ------------------------------------------------------------------SetPivotPos
/** @brief set the x,y,z coordinates of a pivot of a leg*/
void Quadruped::SetPivotPos(int leg_index, int pivot_index, double x, double y,
                            double z) {
  legs_[leg_index]->SetPivotPos(pivot_index, x, y, z);
}

// ---------------------------------------------------------------SetPivotConfig
/** @brief set the axis, offset and max of a pivot*/
void Quadruped::SetPivotConfig(int leg_index, int pivot_index, double offset,
                               double abs_max) {
  legs_[leg_index]->SetPivotConfig(pivot_index, offset, abs_max);
}

// ------------------------------------------------------------ConfigurePivotRot
/** @brief modify R of a pivot while keeping Pivot::angle_ the same*/
void Quadruped::ConfigurePivotRot(int leg_index, int pivot_index, Axis axis,
                                  double angle) {
  legs_[leg_index]->ConfigurePivotRot(pivot_index, axis, angle);
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

// ------------------------------------------------------GetRelativeHMatrixArray
/** @brief return the hmatrix values for a hmatrix (pivot or foot) of a leg
 * relative to origin.*/
const double* Quadruped::GetRelativeHMatrixArray(int leg_index,
                                                 int pivot_index) {
  return legs_[leg_index]->GetRelativeHMatrixArray(pivot_index);
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

// -----------------------------------------------------------------------GetCoM
/** @brief returns the center of mass position as HMatrix relative to world
 *
 * The xy-plane (of the origin frame) is assumed to be normal to gravity\n
 * Also updates the CoM in the gaitgenerator
 */
const double* Quadruped::GetCoM() {
  double total_mass = 0.0;
  double m_l;
  HMatrix unwweighted;
  for (int l = 0; l < kLegCount; ++l) {
    m_l = legs_[l]->get_total_mass();
    total_mass = total_mass + m_l;
    unwweighted.SelfDotScaled(legs_[l]->GetCoM(), m_l);
  }
  H_com_.Clear();
  H_com_.SelfDotScaled(unwweighted, 1.0 / total_mass);

  return H_com_.array();
}

// ------------------------------------------------------------GetFootRestVector
/** @brief get 3d vector for rest vector with a specific body R, frame 0 
 *
 * warning: this breaks if \ref H_cob_ has non-zero x,y,z
 */
const double* Quadruped::GetFootRestVector(int leg_index,
                                           HMatrix body_rotation) {
  HMatrix H_cob_foot = H_cob_.Dot(body_rotation).Dot(
    HMatrix(rest_vectors_[leg_index][0], rest_vectors_[leg_index][1],
            rest_vectors_[leg_index][2]));
  last_rest_vector_[0] = H_cob_foot.GetX();
  last_rest_vector_[1] = H_cob_foot.GetY();
  last_rest_vector_[2] = H_cob_foot.GetZ();
  return last_rest_vector_;
}

// -------------------------------------------------------------ChangePivotAngle
/** @brief change a pivots angle, false on out of bounds*/
bool Quadruped::ChangePivotAngle(int leg_index, int pivot_index,
                                  double angle) {
  if (legs_[leg_index]->ChangePivotAngle(pivot_index, angle)) {
    return true;
  } else {
    return false;
  }
}

// ----------------------------------------------------------------ChangeFootPos
/** @brief change the position of a foot, false if IK fails. COB frame*/
bool Quadruped::ChangeFootPos(int leg_index, double dx, double dy,
                              double dz) {
  return legs_[leg_index]->ChangeFootPos(dx, dy, dz);
}

// -------------------------------------------------------------ChangeAllFeetPos
/** @brief change the position of all feet by dx, dy, dz. frame 0*/
bool Quadruped::ChangeAllFeetPos(double dx, double dy, double dz) {
  int i;
  for (i = 0; i < kLegCount; ++i) {
    // try to move a leg
    if (!ChangeFootPos(i, dx, dy, dz)) {
      // on fail undo all changed legs
      while (i > 0) {
        --i;
        ChangeFootPos(i, -dx, -dy, -dz);
      }
      return false;
    }
  }
  return true;
}

// -------------------------------------------------------------------SetFootPos
/** @brief set x y  z for foot. false if IK fails. 0 frame*/
bool Quadruped::SetFootPos(int leg_index, double x, double y, double z) {
  const double* H = legs_[leg_index]->GetRelativeHMatrixArray(
                                                              Leg::kPivotCount);
  const double dx = x - H[HMatrix::kX];
  const double dy = y - H[HMatrix::kY];
  const double dz = z - H[HMatrix::kZ];
  return ChangeFootPos(leg_index, dx, dy, dz);
}

// --------------------------------------------------------------SetAllAnglesTo0
void Quadruped::SetAllAnglesTo0() {
  for (int l = 0; l < kLegCount; ++l) {
    for (int i = 0; i < Leg::kPivotCount; ++i) {
      legs_[l]->SetPivotAngle(i, 0.0);
    }
  }
}

// -----------------------------------------------------------EqualizeFeetLevels
/** @brief set all feet Z, 0 frame */
bool Quadruped::EqualizeFeetLevels(double z) {
  const double* p[kLegCount];
  double original_z[kLegCount];
  int i = 0;
  for (i = 0; i < kLegCount; ++i) {
    // try to move leg i
    p[i] = GetRelativeHMatrixArray(i, Leg::kPivotCount);
    original_z[i] = p[i][HMatrix::kZ];
    if (!SetFootPos(i, p[i][HMatrix::kX], p[i][HMatrix::kY], z)) {
      // on fail undo leg i-1 ... 0
      printf("eq fail at i=%i\n", i);
      printf("eq target: %.2f, %.2f, %.2f\n", p[i][HMatrix::kX],
             p[i][HMatrix::kY], z);
      while (i > 0) {
        --i;
        printf("eq undo %i\n", i);
        SetFootPos(i, p[i][HMatrix::kX], p[i][HMatrix::kY], original_z[i]);
      }
      return false;
    }
  }
  return true;
}

// -----------------------------------------------------------ChangeBodyRotation
/** @brief rotate body with hmatrix, keep frame 0 feet pos */
bool Quadruped::ChangeBodyRotation(HMatrix R) {
  // grab xyz of each feet in frame 0. (for rollback among others)
  double deltas[kLegCount][3];
  int i;
  for (i = 0; i < kLegCount; ++i) {
    const double* p = GetRelativeHMatrixArray(i, Leg::kPivotCount);
    deltas[i][0] = -p[HMatrix::kX];
    deltas[i][1] = -p[HMatrix::kY];
    deltas[i][2] = -p[HMatrix::kZ];
  }
  // rotate cob
  H_cob_.SelfDot(R);
  // diff feet pos in frame 0. delta = new - old
  for (i = 0; i < kLegCount; ++i) {
    const double* p = GetRelativeHMatrixArray(i, Leg::kPivotCount);
    deltas[i][0] += p[HMatrix::kX];
    deltas[i][1] += p[HMatrix::kY];
    deltas[i][2] += p[HMatrix::kZ];
  }
  // set new feet pos to -delta
  for (i = 0; i < kLegCount; ++i) {
    if (!ChangeFootPos(i, -deltas[i][0], -deltas[i][1], -deltas[i][2])) {
      // on fail simply rotate cob back, and undo any changed legs
      while (i > 0) {
        --i;
        ChangeFootPos(i, deltas[i][0], deltas[i][1], deltas[i][2]);
      }
      H_cob_.SelfDot(R.Inverse());
      return false;
    }
  }
  return true;
}

// --------------------------------------------------------------SetBodyRotation
/** @brief rotate body with angles. updates feet pos*/
bool Quadruped::SetBodyRotation(HMatrix R) {
  // rotate back to I, then to R
  HMatrix R2 = H_cob_.Inverse();
  R2.SelfDot(R);
  return ChangeBodyRotation(R2);
}

// --------------------------------------------------------------------ResetBody
/** @brief restore H_cob_ to I(4) while keeping all feet pos
 *
 * This doesn't change the robot pose
 */
void Quadruped::ResetBody() {
  H_cob_.Clear();
}

// ------------------------------------------------------------SetFootRestVector
/** @brief set \ref Leg::rest_vector_ for a leg, 0 frame 
 * 
 * stored relative to cob
 */
void Quadruped::SetFootRestVector(int leg_index, double x, double y, double z) {
  HMatrix H_cob_foot = H_cob_.Inverse().Dot(HMatrix(x, y, z));
  rest_vectors_[leg_index][0] = H_cob_foot.GetX();
  rest_vectors_[leg_index][1] = H_cob_foot.GetY();
  rest_vectors_[leg_index][2] = H_cob_foot.GetZ();
}

// ----------------------------------------------------------------ConnectDevice
/** @brief connect to usb device, see \ref UsbCom::Connect for return values*/
int Quadruped::ConnectDevice(uint16_t vid, uint16_t pid) {
  return usb_.Connect(vid, pid);
}

// -----------------------------------------------------------------SyncToDevice
/** @brief update everything on the device to match the local data*/
bool Quadruped::SyncToDevice() {
  double pulsewidths[UsbCom::kDeviceServoCount];
  for (int l = 0; l < kLegCount; ++l) {
    for (int i = 0; i < Leg::kPivotCount; ++i) {
      pulsewidths[l * Leg::kPivotCount + i] = legs_[l]->GetPivotPulsewidth(i);
    }
  }
  if (usb_.WriteServoPulsewidths(pulsewidths)) {
    return false;
  }
  return true;
}

// ---------------------------------------------------------------SyncFromDevice
/** @brief update everything to match the currect device data*
 *
 * @return false on failure, true on success
 */
bool Quadruped::SyncFromDevice() {
  if (usb_.ReadServoPulsewidths()) {
    return false;
  }
  const double* pulsewidths = usb_.device_servo_pulsewidths();
  double pw;
  for (int l = 0; l < kLegCount; ++l) {
    for (int i = 0; i < Leg::kPivotCount; ++i) {
      pw = pulsewidths[l*Leg::kPivotCount+i];
      legs_[l]->SetPivotAngle(
        i, legs_[l]->GetAngleFromPulsewidth(i, pw));
    }
  }
  return true;
}

// --------------------------------------------------------GetMiscDataFromDevice
/** @brief calls UsbCom::ReadMiscData */
const uint8_t* Quadruped::GetMiscDataFromDevice() {
  return usb_.ReadMiscData();
}

// ---------------------------------------------------------GetAdcDataFromDevice
/** @brief calls UsbCom::ReadAdcData */
const uint8_t* Quadruped::GetAdcDataFromDevice() {
  return usb_.ReadAdcData();
}

// ---------------------------------------------------------------UpdateSpringGG
/** @brief update the spring gaitgenerator with the current feet, com, 
 * and rest positions
 */
void Quadruped::UpdateSpringGG() {
  sgg_.SetCoMPosition(H_com_.GetX(), H_com_.GetY());
  int i;
  for (i = 0; i < kLegCount; ++i) {
    const double* p = GetRelativeHMatrixArray(i, Leg::kPivotCount);
    sgg_.SetFootPosition(i, p[HMatrix::kX], p[HMatrix::kY]);
    const double* r = GetFootRestVector(i, HMatrix());
    sgg_.SetRestPosition(i, r[0], r[1]);
  }
  sgg_.CalculateForces();
  // sgg_.PrintForces();
}

// -----------------------------------------------------------------ZeroSpringGG
/** @brief set all spring forces to 0 for the current feet positions*/
void Quadruped::ZeroSpringGG() {
  sgg_.ZeroForces();
}

// -------------------------------------------------------GetLegWithHighestForce
/** @brief calls \ref SpringGG::GetLegWithHighestForce */
int Quadruped::GetLegWithHighestForce(double direction_angle) {
  return sgg_.GetLegWithHighestForce(direction_angle);
}

// -------------------------------------------------------------------CanLiftLeg
/** @brief check if a leg can be lifted without losing stability
 *
 * Make sure you have called \ref UpdateSpringGG first
 */
bool Quadruped::CanLiftLeg(int index, double margin) {
  return sgg_.CoMInside(index, margin);
}

// -----------------------------------------------------------CalcSpringGGTarget
/** @brief calls \ref SpringGG::GetDeltaVector and stores it in
 * \ref last_sgg_vector_
 */
bool Quadruped::CalcSpringGGTarget(int index, double angle, double F) {
  sgg_.GetDeltaVector(index, angle, F, last_sgg_vector_);
  if (last_sgg_vector_[2] != 0.0) {
    return false;
  } else {
    return true;
  }
}

// ----------------------------------------------------------get_last_sgg_vector
/** @brief return the last vector calculated by \ref sgg_ */
const double* Quadruped::get_last_sgg_vector() {
  return last_sgg_vector_;
}

// ---------------------------------------------------------FindVectorToDiagonal
/** @brief return a 2d vector to the diagonal between two feet + more values */
// TODO(michiel): this can be done without knowledge of diagonal indices
const double* Quadruped::FindVectorToDiagonal(int diagonal_index1,
                                              int diagonal_index2) {
  // grab hmatrices for diagonally opposed feet
  HMatrix footA  = H_cob_.Inverse().Dot(HMatrix(
                   GetRelativeHMatrixArray(diagonal_index1, Leg::kPivotCount)));
  HMatrix footB  = H_cob_.Inverse().Dot(HMatrix(
                   GetRelativeHMatrixArray(diagonal_index2, Leg::kPivotCount)));
  // find angle of line from A to B
  double angleAB = Get2DAngle(footA.GetX(), footB.GetX(),
                              footA.GetY(), footB.GetY());
  // create a new transform matrix to rotate footA and B so the line is
  // horizontal, and the distance to the line is the y coordinate of both
  HMatrix T(Z_AXIS, -angleAB);
  // apply transform
  footA = T.Dot(footA);
  // grab vector to line
  double v[3] = {0.0, footA.GetY(), 0.0};
  T.CounterRotateVector(v, last_sp_vector_);
  last_sp_vector_[2] = std::sqrt(std::pow(last_sp_vector_[0], 2) +
                                 std::pow(last_sp_vector_[1], 2));
  // determine leg index for the foot on the other side of the diagonal
  // which is likely the leg that makes the most similar angle to com
  // grab candidate indices
  int candidates[kLegCount - 2];
  int i;
  int j = 0;
  for (i = 0; i < kLegCount; ++i) {
    if (i == diagonal_index1 || i == diagonal_index2) continue;
    candidates[j] = i;
    ++j;
  }
  // compare angles to cob with the angle of our vector
  double vector_angle = Get2DAngle(0, last_sp_vector_[0],
                                   0, last_sp_vector_[1]);
  double difference_angle;
  double lowest_diff = 10.0;
  for (i = 0; i < kLegCount - 2; ++i) {
    // reusing footA
    footA = H_cob_.Inverse().Dot(HMatrix(
             GetRelativeHMatrixArray(candidates[i], Leg::kPivotCount)));
    difference_angle = std::abs(NormalizeAngle(
      Get2DAngle(0, footA.GetX(), 0, footA.GetY()) - vector_angle));
    if (difference_angle < lowest_diff) {
      lowest_diff = difference_angle;
      last_sp_vector_[3] = candidates[i];
    }
  }

  return last_sp_vector_;
}

}  // namespace Q1

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

// ------------------------------------------------------------ConfigurePivotRot
/** @brief modify R of a pivot while keeping Pivot::angle_ the same*/
void Quadruped::ConfigurePivotRot(int leg_index, int pivot_index, Axis axis,
                                  double angle) {
  legs_[leg_index]->ConfigurePivotRot(pivot_index, axis, angle);
}

// ------------------------------------------------------GetRelativeHMatrixArray
/** @brief return the hmatrix values for a hmatrix (pivot or foot) of a leg
 * relative to cob.*/
const double* Quadruped::GetRelativeHMatrixArray(int leg_index,
                                                 int pivot_index) {
  return legs_[leg_index]->GetRelativeHMatrixArray(pivot_index);
}

// ---------------------------------------------------------------SetPivotConfig
/** @brief set the axis, offset and max of a pivot*/
void Quadruped::SetPivotConfig(int leg_index, int pivot_index, double offset,
                               double abs_max) {
  legs_[leg_index]->SetPivotConfig(pivot_index, offset, abs_max);
}

// -------------------------------------------------------------ChangePivotAngle
/** @brief change a pivots angle, false on out of bounds*/
bool Quadruped::ChangePivotAngle(int leg_index, int pivot_index,
                                  double angle) {
  return legs_[leg_index]->ChangePivotAngle(pivot_index, angle);
}

// ----------------------------------------------------------------ChangeFootPos
/** @brief change the position of a foot, false if IK fails. COB frame*/
bool Quadruped::ChangeFootPos(int leg_index, double dx, double dy,
                              double dz) {
  return legs_[leg_index]->ChangeFootPos(dx, dy, dz);
}

// -------------------------------------------------------------------SetFootPos
/** @brief set x y  z for foot. false if IK fails. COB frame*/
bool Quadruped::SetFootPos(int leg_index, double x, double y, double z) {
  const double* H = legs_[leg_index]->GetRelativeHMatrixArray(
                                                              Leg::kPivotCount);
  const double dx = x - H[HMatrix::kX];
  const double dy = y - H[HMatrix::kY];
  const double dz = z - H[HMatrix::kZ];
  return ChangeFootPos(leg_index, dx, dy, dz);
}

// ----------------------------------------------------------------ConnectDevice
/** @brief connect to usb device, see \ref UsbCom::Connect for return values*/
int Quadruped::ConnectDevice(uint16_t vid, uint16_t pid) {
  return usb_.Connect(vid, pid);
}

// --------------------------------------------------------------GetDeviceAngles
/** @brief read device servo angles*/
const double* Quadruped::GetDeviceAngles() {
  if (usb_.ReadServoAngles() == 0) {
    return usb_.device_servo_angles();
  } else {
    return NULL;
  }
}

// ---------------------------------------------------------------SyncFromDevice
/** @brief update everything to match the currect device data*
 *
 * @return 1 on failure, 0 on success
 */
bool Quadruped::SyncFromDevice() {
  if (usb_.ReadServoAngles()) {
    return false;
  }
  const double* angles = usb_.device_servo_angles();
  for (int l = 0; l < kLegCount; ++l) {
    for (int i = 0; i < Leg::kPivotCount; ++i) {
      legs_[l]->SetPivotAngle(i, angles[l*Leg::kPivotCount + i]);
      printf("%.3f\n", angles[l*Leg::kPivotCount + i]);
    }
  }
  return true;
}

// -----------------------------------------------------------------SyncToDevice
/** @brief update everything on the device to match the local data*/
bool Quadruped::SyncToDevice() {
  double angles[UsbCom::kDeviceServoCount];
  for (int l = 0; l < kLegCount; ++l) {
    for (int i = 0; i < Leg::kPivotCount; ++i) {
      angles[l*Leg::kPivotCount + i] = legs_[l]->GetPivotAngle(i);
    }
  }
  if (usb_.WriteServoAngles(angles)) {
    return false;
  }
  return true;
}

// -----------------------------------------------------------------------GetCoM
/** @brief returns the center of mass position as HMatrix relative to world?
 *
 * The xy-plane (of the origin frame) is assumed to be normal to gravity
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

}  // namespace Q1

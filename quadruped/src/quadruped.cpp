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
  gaitgenerator_ = new ROGG::GaitGenerator();
  gaitgenerator_->set_use_enforce_wavegait(false);
  gaitgenerator_->set_H_cob_com(HMatrix().array());
}

// ---------------------------------------------------------Destructor Quadruped
/** @brief Destructor*/
Quadruped::~Quadruped() {
  for (int i = 0; i < kLegCount; ++i) {
    delete legs_[i];
  }
  delete gaitgenerator_;
}

// ----------------------------------------------------------------SetGGVelocity
/** @brief set the CoB velocity for the gaitgenerator*/
void Quadruped::SetGGVelocity(const double* velocity_vector) {
  gaitgenerator_->set_velocity(velocity_vector);
}

// -------------------------------------------------------------last_step_result
/** @brief accessor for \ref last_step_result_
 *
 * \ref last_step_result_ is filled when \ref GGStep is called
 */
ROGG::StepResults Quadruped::last_step_result() {
  return last_step_result_;
}

// ----------------------------------------------------------------set_gg_config
/** @brief mutator for \ref gg_config
 *
 *
 */
void Quadruped::set_gg_config(gaitgenerator_configuration config) {
  gg_config_ = config;
  SetGaitgeneratorHL(legs_[0]->GetPivotAbsMaxAngle(0));  // HL should update
  gaitgenerator_->set_transfer_speeds(gg_config_.transfer_speeds);
  gaitgenerator_->set_ASM_min(gg_config_.ASM_min);
  gaitgenerator_->set_ground_clearance(gg_config_.ground_clearance);
  gaitgenerator_->set_search_width(gg_config_.search_width);
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
  // if it's the foot, update the gaitgenerator as well
  if (pivot_index == Leg::kPivotCount) {
    SetGaitgeneratorFoot(leg_index);
  }
  // if it's pivot 0:
  if (pivot_index == 0) {
    HMatrix H_cob_pivot0 = H_cob_.Dot(
      HMatrix(legs_[leg_index]->GetRelativeHMatrixArray(0)));
    gaitgenerator_->SetLegPivot0(leg_index+1, H_cob_pivot0.array());
  }
}


// ---------------------------------------------------------SetGaitgeneratorFoot
/** @brief update the foot position in the gaitgenerator*/
void Quadruped::SetGaitgeneratorFoot(int index) {
  // gaitgenerator takes H_pivot0_P#
  // but we're using pivot0 as actual actuator so let's do some MAGIC!
  
  // copy H_cob_pivot0
  HMatrix magic(legs_[index]->GetHMatrixArray(0));
  // rotate to original 0 angle
  magic.SelfDot(HMatrix(Z_AXIS, -legs_[index]->GetPivotAngle(0)));
  // inverse magic dot H_0_P
  HMatrix gg_H_pivot0_P = magic.Inverse().Dot(
    HMatrix(legs_[index]->GetRelativeHMatrixArray(Leg::kPivotCount)));
    
  gaitgenerator_->SetLegP(index+1,  gg_H_pivot0_P.array());
  // HMatrix H_0_pivot0 = HMatrix(legs_[index]->GetRelativeHMatrixArray(0));
  // HMatrix H_0_P = HMatrix(legs_[index]->GetRelativeHMatrixArray(3));
  // HMatrix H_pivot0_P = H_0_pivot0.Inverse().Dot(H_0_P);
  // printf("LEG[%i]:\n", index);
  // printf("just sent relative x = %f to gg\n", gg_H_pivot0_P.array()[3]);
  // printf("H_0_P.x = %f\n", H_0_P.array()[3]);
  
  // inverse(H_0_pivot)*h_0_p
}

// ------------------------------------------------------------ConfigurePivotRot
/** @brief modify R of a pivot while keeping Pivot::angle_ the same*/
void Quadruped::ConfigurePivotRot(int leg_index, int pivot_index, Axis axis,
                                  double angle) {
  legs_[leg_index]->ConfigurePivotRot(pivot_index, axis, angle);
  if (pivot_index == 0) {
    // if it's pivot 0, the gaitgenerator also needs to be in on it
    HMatrix H_cob_pivot0 = H_cob_.Dot(
      HMatrix(legs_[leg_index]->GetRelativeHMatrixArray(0)));
    gaitgenerator_->SetLegPivot0(leg_index+1, H_cob_pivot0.array());
  }
  // TODO(michiel): technically if the foot changes, the gaitgenerator
  // also needs to know
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
  if (pivot_index == 0) {
    // TODO(michiel): update gaitgenerator HL on pivot 0
    // if it's pivot 0, then the gaitgenerator needs HL updated
    SetGaitgeneratorHL(abs_max);
  }
}

// -----------------------------------------------------------SetGaitgeneratorHL
/** @brief set HL in the gaitgenerator*/
void Quadruped::SetGaitgeneratorHL(double abs_max) {
  // HL1 is at angle -abs_max, HL2 with +abs_max
  const double HL[3] = {gg_config_.reachable_sector_radius, 0.0, 0.0};
  double rotated_HL[3];
  // a hmatrix with our needed R
  HMatrix R(Z_AXIS, abs_max);
  R.RotateVector(HL, rotated_HL);
  gaitgenerator_->set_HL1(
    HMatrix(rotated_HL[0], rotated_HL[1], rotated_HL[2]).array());
  R.CounterRotateVector(HL, rotated_HL);
  gaitgenerator_->set_HL2(
    HMatrix(rotated_HL[0], rotated_HL[1], rotated_HL[2]).array());
  printf("DEBUG: HL: %.3f, %.2f, %.3f\n",
    HL[0], HL[1], HL[2]);
}


// -------------------------------------------------------------ChangePivotAngle
/** @brief change a pivots angle, false on out of bounds*/
bool Quadruped::ChangePivotAngle(int leg_index, int pivot_index,
                                  double angle) {
  if (legs_[leg_index]->ChangePivotAngle(pivot_index, angle)) {
    SetGaitgeneratorFoot(leg_index);
    return true;
  } else {
    return false;
  }
}

// ----------------------------------------------------------------ChangeFootPos
/** @brief change the position of a foot, false if IK fails. COB frame*/
bool Quadruped::ChangeFootPos(int leg_index, double dx, double dy,
                              double dz) {
  if (legs_[leg_index]->ChangeFootPos(dx, dy, dz)) {
    SetGaitgeneratorFoot(leg_index);
    return true;
  } else {
    return false;
  }
}

// -------------------------------------------------------------------SetFootPos
/** @brief set x y  z for foot. false if IK fails. COB frame*/
bool Quadruped::SetFootPos(int leg_index, double x, double y, double z) {
  const double* H = legs_[leg_index]->GetRelativeHMatrixArray(
                                                              Leg::kPivotCount);
  const double dx = x - H[HMatrix::kX];
  const double dy = y - H[HMatrix::kY];
  const double dz = z - H[HMatrix::kZ];
  if (ChangeFootPos(leg_index, dx, dy, dz)) {
    SetGaitgeneratorFoot(leg_index);
    return true;
  } else {
    return false;
  }
}

// ----------------------------------------------------------------ConnectDevice
/** @brief connect to usb device, see \ref UsbCom::Connect for return values*/
int Quadruped::ConnectDevice(uint16_t vid, uint16_t pid) {
  return usb_.Connect(vid, pid);
}

// --------------------------------------------------------------GetDeviceAngles
/** @brief read device servo angles*/
const double* Quadruped::GetDeviceAngles() {
  if (usb_.ReadServoPulsewidths() == 0) {
    // return usb_.device_servo_angles();
    return NULL;
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
  if (usb_.ReadServoPulsewidths()) {
    return false;
  }
  // const double* angles = usb_.device_servo_angles();
  // for (int l = 0; l < kLegCount; ++l) {
    // for (int i = 0; i < Leg::kPivotCount; ++i) {
      // legs_[l]->SetPivotAngle(i, angles[l*Leg::kPivotCount + i]);
      // printf("%.3f\n", angles[l*Leg::kPivotCount + i]);
    // }
    // SetGaitgeneratorFoot(l);
  // }
  return true;
}

// -----------------------------------------------------------------SyncToDevice
/** @brief update everything on the device to match the local data*/
bool Quadruped::SyncToDevice() {
  double pulsewidths[UsbCom::kDeviceServoCount];
  for (int l = 0; l < kLegCount; ++l) {
    for (int i = 0; i < Leg::kPivotCount; ++i) {
    
      // angles[l*Leg::kPivotCount + i] = legs_[l]->GetPivotAngle(i);
    }
  }
  if (usb_.WriteServoPulsewidths(pulsewidths)) {
    return false;
  }
  return true;
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
  // gaitgenerator uses H_cob_com = H_cob_0 * H_0_com
  HMatrix H_cob_com = H_cob_.Inverse();
  H_cob_com.SelfDot(H_com_);
  gaitgenerator_->set_H_cob_com(H_cob_com.array());

  return H_com_.array();
}

// ------------------------------------------------------------------------GetKM
/** @brief returns the kinematic margin of a leg
 *
 * value is recalculated for each call
 */
double Quadruped::GetKM(int leg_index) {
  gaitgenerator_->ClearCacheFor(ROGG::TRANSFER);
  return gaitgenerator_->CalculateKMForLeg(leg_index+1);
}

// ---------------------------------------------------------------------GetLASMF
/** @brief returns the LASMF with the specified leg in the air
 *
 * The value is recalculated for each call
 */
double Quadruped::GetLASMF(int leg_index) {
  gaitgenerator_->ClearCacheFor(ROGG::TRANSFER);
  return gaitgenerator_->GetLASMF(leg_index+1);
}

// ---------------------------------------------------------------------GetLASMB
/** @brief returns the LASMB with the specified leg in the air
 *
 * The value is recalculated for each call
 */
double Quadruped::GetLASMB(int leg_index) {
  gaitgenerator_->ClearCacheFor(ROGG::TRANSFER);
  return gaitgenerator_->GetLASMB(leg_index+1);
}

// -----------------------------------------------------------------------GGStep
/** @brief perfrom a gaitgenerator step. store result*/
ROGG::StepResults Quadruped::GGStep() {
  // always not transferring
  gaitgenerator_->set_transferring(false);
  last_step_result_ = gaitgenerator_->Step();
  if (last_step_result_ == ROGG::NEWFOOTHOLD) {
    // H_0_PLT = H_0_cob * H_cob_PLT
    H_0_PLT_.Copy(H_cob_);
    double target_foothold_values[16];
    gaitgenerator_->CopyH_cob_PLT(target_foothold_values);
    H_0_PLT_.SelfDot(HMatrix(target_foothold_values));
  }
  return last_step_result_;
}

// ------------------------------------------------------------------------GetLT
/** @brief returns LT as 0-based leg index
 *
 * Only valid when the last GGStep result
 * was 2 (\ref ROGG::StepResults)
 */
int Quadruped::GetLT() {
  return gaitgenerator_->LT() - 1;
}

// -------------------------------------------------------------------GetH_0_PLT
/** returns the hmatrix values of the last calculated foothold relative to 0
 *
 * Only valid when the last GGStep result
 * was 2 (\ref ROGG::StepResults)
 */
const double* Quadruped::GetH_0_PLT() {
  return H_0_PLT_.array();
}

}  // namespace Q1

/**
 * @file wrapper.cpp
 * extern "C" functions
 */

#include "include/quadruped.h"

namespace Q1 {

// --------------------------------------------------------------QuadrupedAlloc
/** @brief create a quadruped instance an return its pointer
 *
 * Remeber to free the allocate memory again with \ref QuadrupedFree when
 * you're done.
 */
extern "C" Quadruped* QuadrupedAlloc() {
  return new Quadruped();
}

// ---------------------------------------------------------------QuadrupedFree
/** @brief free the memory allocated by \ref QuadrupedAlloc */
extern "C" void QuadrupedFree(Quadruped* q) {
  delete q;
}

// ----------------------------------------------------------QuadrupedGetHMatrix
/** @brief return a pointer to the values of a hmatrix (pivots and feet)
 *
 * @param q pointer to a quadruped.
 * @param index The index of the HMatrix. numbering: <br>
 * 0 = leg 0, pivot 0<br>
 * 1 = leg 0, pivot 1<br>
 * 3 = leg 0, foot/endpoint<br>
 * 14 = leg 3, pivot 2<br>
 * 15 = leg 3, foot/endpoint<br>
 */
extern "C" const double* QuadrupedGetHMatrix(Quadruped* q, int index) {
  int mod = Leg::kPivotCount + 1;  // i.e. 4
  int l = index / mod;  // leg index
  int p = index % mod;  // pivot index
  if (p >= Leg::kPivotCount) {
    // 3rd pivot is endpoint
    return q->GetEndpoint(l);
  } else {
    return q->GetHMatrixArrayByIndex(l * Leg::kPivotCount + p);
  }
}

// ---------------------------------------------------------QuadrupedSetPivotPos
/** @brief set x y and z coordinates of a pivot
 *
 * @param q pointer to a quadruped
 * @param leg_index index of the leg
 * @param pivot_index index of the pivot
 * @param x new x coordinate
 * @param y new y coordinate
 * @param z new z coordinate
 */
extern "C" void QuadrupedSetPivotPos(Quadruped* q, int leg_index,
                                     int pivot_index, double x, double y,
                                     double z) {
  q->SetPivotPos(leg_index, pivot_index, x, y, z);
}

// ---------------------------------------------------QuadrupedConfigurePivotRot
/** @brief configure the base rotation of a pivot. see
 * \ref Quadruped::ConfigurePivotRot*/
extern "C" void QuadrupedConfigurePivotRot(Quadruped* q, int leg_index,
                                        int pivot_index, Axis axis,
                                        double angle) {
  q->ConfigurePivotRot(leg_index, pivot_index, axis, angle);
}

// --------------------------------------------QuadrupedSetPivotPulsewidthConfig
/** @brief set \ref Pivot::pw_0_ and \ref Pivot::pw_60_
 *
 *
 */
extern "C" void QuadrupedSetPivotPulsewidthConfig(Quadruped* q, int leg_index,
                                                   int pivot_index,
                                                   double pw_0, double pw_60) {
  q->SetPivotPulsewidthConfig(leg_index, pivot_index, pw_0, pw_60);
}

// --------------------------------------------------QuadrupedGetRelativeHMatrix
/** @brief get the values of a hmatrix relative to the cob*/
extern "C" const double* QuadrupedGetRelativeHMatrix(Quadruped* q,
                                                     int leg_index,
                                                     int pivot_index) {
  return q->GetRelativeHMatrixArray(leg_index, pivot_index);
}

// ------------------------------------------------------QuadrupedSetPivotConfig
/** @brief set the offset and axis of a pivot
 *
 * @param q pointer to a quadruped
 * @param leg_index index for the leg (0..3)
 * @param pivot_index index of the pivot (0..3)
 * @param offset the offset angle (i.e. the neutral angle)
 * @param abs_max the absolute maximum angle relative to the offset for the
 * pivot
 */
extern "C" void QuadrupedSetPivotConfig(Quadruped* q, int leg_index,
                                         int pivot_index, double offset,
                                         double abs_max) {
  q->SetPivotConfig(leg_index, pivot_index, offset, abs_max);
}

// --------------------------------------------------- QuadrupedChangePivotAngle
/** @brief change the angle of a pivot, false on out of range*/
extern "C" bool QuadrupedChangePivotAngle(Quadruped* q, int leg_index,
                                            int pivot_index, double angle) {
  return q->ChangePivotAngle(leg_index, pivot_index, angle);
}

// -------------------------------------------------------QuadrupedChangeFootPos
/** @brief change the foot position of a leg. coords relative to cob
 *
 * @param q pointer to a quadruped
 * @param leg_index index of the leg
 * @param dx change in x direction
 * @param dy change in y direction
 * @param dz change in z direction
 * @param mode sets interpretation of dx,dy,dz:<br>
 * mode 0 = relative to Origin<br>
 * mode 1 = relative to COB (not implemented)
 */
extern "C" bool QuadrupedChangeFootPos(Quadruped* q, int leg_index,
                                       double dx, double dy, double dz,
                                       int mode) {
  if (mode == 0) {
    // relative to COB
    return q->ChangeFootPos(leg_index, dx, dy, dz);
  }
  return false;
}

// ----------------------------------------------------------QuadrupedSetFootPos
/** @brief change the foot position of a leg. coords relative to cob*
 *
 * @param q pointer to a quadruped
 * @param leg_index index of the leg
 * @param x new x coord
 * @param y new y coord
 * @param z new z coord
 */
extern "C" bool QuadrupedSetFootPos(Quadruped* q, int leg_index, double x,
                                     double y, double z) {
  return q->SetFootPos(leg_index, x, y, z);
}

// ----------------------------------------------------------QuadrupedConnectDev
/** Connect to the usb device with vid, pid*/
extern "C" int QuadrupedConnectDev(Quadruped* q, uint16_t vid, uint16_t pid) {
  return q->ConnectDevice(vid, pid);
}

// -----------------------------------------------------------QuadrupedSyncToDev
/** @brief send all local pivot angles to the connected device*/
extern "C" bool QuadrupedSyncToDev(Quadruped* q) {
  return q->SyncToDevice();
}

// ---------------------------------------------------------QuadrupedSyncFromDev
/** @brief send all local pivot angles to the connected device*/
extern "C" bool QuadrupedSyncFromDev(Quadruped* q) {
  return q->SyncFromDevice();
}

// --------------------------------------------------------------QuadrupedGetCoM
/** @brief calls \ref Quadruped::GetCoM */
extern "C" const double* QuadrupedGetCoM(Quadruped* q) {
  return q->GetCoM();
}

// -----------------------------------------------------QuadrupedSetAllAnglesTo0
/** @brief calls \ref Quadruped::SetAllAnglesTo0 */
extern "C" void QuadrupedSetAllAnglesTo0(Quadruped* q) {
  q->SetAllAnglesTo0();
}

// ------------------------------------------------------QuadrupedUpdateSpringGG
/** @brief calls \ref Quadruped::UpdateSpringGG */
extern "C" void QuadrupedUpdateSpringGG(Quadruped* q) {
  q->UpdateSpringGG();
}

// --------------------------------------------------------QuadrupedZeroSpringGG
/** @brief calls \ref Quadruped::ZeroSpringGG */
extern "C" void QuadrupedZeroSpringGG(Quadruped* q) {
  q->ZeroSpringGG();
}

// ----------------------------------------------QuadrupedGetLegWithHighestForce
/** @brief calls \ref Quadruped::GetLegWithHighestForce */
extern "C" int QuadrupedGetLegWithHighestForce(Quadruped* q, double angle) {
  return q->GetLegWithHighestForce(angle);
}

// ----------------------------------------------------------QuadrupedCanLiftLeg
/** @brief calls \ref Quadruped::CanLiftLeg */
extern "C" bool QuadrupedCanLiftLeg(Quadruped* q, int index, double margin) {
  return q->CanLiftLeg(index, margin);
}

// -----------------------------------------------QuadrupedGetLastSpringGGVector
/** @brief calls \ref Quadruped::CalcSpringGGTarget
 *
 * when z component of vector <> 0, the result is invalid
 */
extern "C" const double* QuadrupedGetLastSpringGGVector(Quadruped* q, int index,
                                                        double angle,
                                                        double F) {
  q->CalcSpringGGTarget(index, angle, F);
  return q->get_last_sgg_vector();
}

// --------------------------------------------------QuadrupedEqualizeFeetLevels
/** @brief calls \ref Quadruped::EqualizeFeetLevels */
extern "C" bool QuadrupedEqualizeFeetLevels(Quadruped* q, double z) {
  return q->EqualizeFeetLevels(z);
}

// ----------------------------------------------------------QuadrupedRotateBody
/** @brief calls \ref Quadruped::RotateBody */
extern "C" bool QuadrupedChangeBodyRotation(Quadruped* q, Axis axis,
                                            double angle) {
  return q->ChangeBodyRotation(HMatrix(axis, angle));
}

// -----------------------------------------------------QuadrupedSetBodyRotation
/** @brief calls \ref Quadruped::SetBodyRotation with angles */
extern "C" bool QuadrupedSetBodyRotation(Quadruped* q, double xrot,
                                            double yrot, double zrot) {
  HMatrix R(X_AXIS, xrot);
  R.SelfDot(HMatrix(Y_AXIS, yrot));
  R.SelfDot(HMatrix(Z_AXIS, zrot));
  return q->SetBodyRotation(R);
}

// ----------------------------------------------------QuadrupedChangeAllFeetPos
/** @brief calls \ref Quadruped::ChangeAllFeetPos */
extern "C" bool QuadrupedChangeAllFeetPos(Quadruped* q, double dx, double dy,
                                           double dz) {
  return q->ChangeAllFeetPos(dx, dy, dz);
}

// -----------------------------------------------------------QuadrupedResetBody
/** @brief calls \ref Quadruped::ResetBody */
extern "C" void QuadrupedResetBody(Quadruped* q) {
  q->ResetBody();
}

// ---------------------------------------------------------QuadrupedGetMiscData
/** @brief calls \ref UsbCom::ReadMiscData */
extern "C" const uint8_t* QuadrupedGetMiscData(Quadruped* q) {
  return q->GetMiscDataFromDevice();
}

// ----------------------------------------------------------QuadrupedGetAdcData
/** @brief calls \ref UsbCom::ReadAdcData */
extern "C" const uint8_t* QuadrupedGetAdcData(Quadruped* q) {
  return q->GetAdcDataFromDevice();
}

// ------------------------------------------------QuadrupedFindVectorToDiagonal
/** @brief calls \ref Quadruped::FindVectorToDiagonal */
extern "C" const double* QuadrupedFindVectorToDiagonal(Quadruped* q,
                                                       int diagonal_index1,
                                                       int diagonal_index2) {
  return q->FindVectorToDiagonal(diagonal_index1, diagonal_index2);
}

// ---------------------------------------------------QuadrupedSetFootRestVector
/** @brief calls \ref Quadruped::SetFootRestVector */
extern "C" void QuadrupedSetFootRestVector(Quadruped* q, int leg_index,
                                             double x, double y, double z) {
  q->SetFootRestVector(leg_index, x, y, z);
}

// ---------------------------------------------------QuadrupedGetFootRestVector
/** @brief calls \ref Quadruped::GetFootRestVector with specified R */
extern "C" const double* QuadrupedGetFootRestVector(Quadruped* q, int leg_index,
                                                    Axis axis, double angle) {
  return q->GetFootRestVector(leg_index, HMatrix(axis, angle));
}

}  // namespace Q1

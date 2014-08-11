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

// ---------------------------------------------------------QuadrupedSetGGConfig
/** @brief calls \ref Quadruped::set_gg_config with the supplied values*/
extern "C" void QuadrupedSetGGConfig(Quadruped* q, 
                                                 double reachable_sector_radius,
                                                 double transfer_speed1,
                                                 double transfer_speed2,
                                                 double transfer_speed3,
                                                 double ASM_min,
                                                 double ground_clearance,
                                                 double search_width) {
  Quadruped::gaitgenerator_configuration config;
  config.reachable_sector_radius = reachable_sector_radius;
  config.transfer_speeds[0] = transfer_speed1;
  config.transfer_speeds[1] = transfer_speed2;
  config.transfer_speeds[2] = transfer_speed3;
  config.ASM_min = ASM_min;
  config.ground_clearance = ground_clearance;
  config.search_width = search_width;
  q->set_gg_config(config);
}

// -------------------------------------------------------QuadrupedSetGGVelocity
/** @brief calls \ref Quadruped::SetGGVelocity */
extern "C" void QuadrupedSetGGVelocity(Quadruped* q, double x, double y, 
                                       double z) {
  const double vector[3] = {x, y, z};
  q->SetGGVelocity(vector);
}

// ---------------------------------------------------------------QuadrupedGetKM
/** @brief calls \ref Quadruped::GetKM */
extern "C" double QuadrupedGetKM(Quadruped* q, int leg_index) {
  return q->GetKM(leg_index);
}

}  // namespace Q1

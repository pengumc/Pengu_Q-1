/**
 * @file quadruped.h
 */

#ifndef QUADRUPED_INCLUDE_QUADRUPED_H_
#define QUADRUPED_INCLUDE_QUADRUPED_H_

#include <stdint.h>

#include <cstddef>

#include "include/rogg/gaitgenerator.h"
#include "include/leg.h"
#include "include/hmatrix.h"
#include "include/usbcom.h"

namespace Q1 {

/** @brief the main quadruped class
 *
 */
class Quadruped {
 public:
  // constants
  static const int kLegCount = 4;
  // typedefs
  typedef struct {
    double reachable_sector_radius;  ///< length of HL in gaitgenerator.
    double transfer_speeds[3];
    double ASM_min;
    double ground_clearance;
    double search_width;
    double L_min;
  } gaitgenerator_configuration;
  // con/destructors
  Quadruped();
  ~Quadruped();
  // functions
  void SetPivotPulsewidthConfig(int leg_index, int pivot_index, double pw_0, 
                                double pw_60);
  void set_reachable_sector_radius(double value);
  void set_gg_config(gaitgenerator_configuration config);
  ROGG::StepResults last_step_result();
  void SetGGVelocity(const double* velocity_vector);
  const double* GetH_0_PLT();
  const double* GetHMatrixArrayByIndex(int index);
  const double* GetRelativeHMatrixArray(int leg_index, int pivot_index);
  const double* GetEndpoint(int index);
  const double* GetCoM();
  void SetPivotPos(int leg_index, int pivot_index, double x, double y,
                   double z);
  void SetPivotConfig(int leg_index, int pivot_index, double offset,
                       double abs_max);
  void ConfigurePivotRot(int leg_index, int pivot_index, Axis axis,
                         double angle);
  bool ChangePivotAngle(int leg_index, int pivot_index, double angle);
  bool ChangeFootPos(int leg_index, double dx, double dy, double dz);
  bool SetFootPos(int leg_index, double x, double y, double z);
  int ConnectDevice(uint16_t vid, uint16_t pid);
  const double* GetDeviceAngles();
  bool SyncToDevice();
  bool SyncFromDevice();
  double GetKM(int leg_index);
  double GetLASMF(int leg_index);
  double GetLASMB(int leg_index);
  ROGG::StepResults GGStep();
  int GetLT();

 private:
  Leg* legs_[kLegCount]; /**< @brief leg pointers*/
  HMatrix H_cob_;/**< @brief HMatrix for Center of body*/
  HMatrix H_com_;/**<@brief Hmatrix for center of mass*/
  UsbCom usb_;/**< @brief usb communications*/
  ROGG::GaitGenerator* gaitgenerator_;  ///< the gaitgenerator (rose)
  gaitgenerator_configuration gg_config_;  ///< gaitgenerator parameters
  ROGG::StepResults last_step_result_;  ///< result of last gaitgenerator step
  HMatrix H_0_PLT_;  ///< last target foothold
  void SetGaitgeneratorHL(double abs_max);
  void SetGaitgeneratorFoot(int index);
};

}  // namespace Q1

#endif  // QUADRUPED_INCLUDE_QUADRUPED_H_

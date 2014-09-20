/**
 * @file quadruped.h
 */

#ifndef QUADRUPED_INCLUDE_QUADRUPED_H_
#define QUADRUPED_INCLUDE_QUADRUPED_H_

#include <stdint.h>

#include <cstddef>

#include "include/springgg.h"
#include "include/leg.h"
#include "include/hmatrix.h"
#include "include/usbcom.h"

namespace Q1 {

/** @brief the main quadruped class
 *
 * lots of configuration to do before use:<br>
 * - set all pivot positions with \ref SetPivotPos
 * - set al pivot configurations with \ref ConfigurePivotRot
 * - set offset and max angles with \ref SetPivotConfig
 * - set servo configuration with \ref SetPivotPulsewidthConfig
 */
class Quadruped {
 public:
  // constants
  static const int kLegCount = 4;
  // typedefs
  // con/destructors
  Quadruped();
  ~Quadruped();
  // functions for configuration
  void SetPivotPulsewidthConfig(int leg_index, int pivot_index, double pw_0,
                                double pw_60);
  void SetPivotPos(int leg_index, int pivot_index, double x, double y,
                   double z);
  void SetPivotConfig(int leg_index, int pivot_index, double offset,
                       double abs_max);
  void ConfigurePivotRot(int leg_index, int pivot_index, Axis axis,
                         double angle);
  // functions for data retrieval
  const double* GetHMatrixArrayByIndex(int index);
  const double* GetRelativeHMatrixArray(int leg_index, int pivot_index);
  const double* GetEndpoint(int index);
  const double* GetCoM();
  // functions to change pivot states
  bool ChangePivotAngle(int leg_index, int pivot_index, double angle);
  bool ChangeFootPos(int leg_index, double dx, double dy, double dz);
  bool SetFootPos(int leg_index, double x, double y, double z);
  void SetAllAnglesTo0();
  void EqualizeFeetLevels(int index);
  // functions handling usb communication
  int ConnectDevice(uint16_t vid, uint16_t pid);
  bool SyncToDevice();
  bool SyncFromDevice();
  // functions to do gaitgeneration
  void UpdateSpringGG();
  void ZeroSpringGG();
  int GetLegWithHighestForce(double direction_angle);
  bool CanLiftLeg(int index, double margin);
  bool CalcSpringGGTarget(int index, double angle, double F);
  const double* get_last_sgg_vector();

 private:
  Leg* legs_[kLegCount]; /**< @brief leg pointers*/
  HMatrix H_cob_;/**< @brief HMatrix for Center of body*/
  HMatrix H_com_;/**<@brief Hmatrix for center of mass*/
  UsbCom usb_;/**< @brief usb communications*/
  SpringGG sgg_;/**< @brief spring based gaitgenerator*/
  double last_sgg_vector_[3];/**< @brief last calculated vector from sgg*/
};

}  // namespace Q1

#endif  // QUADRUPED_INCLUDE_QUADRUPED_H_

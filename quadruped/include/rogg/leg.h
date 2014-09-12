/** 
 * @file leg.h
 */

#ifndef ROSEGAITGENERATOR_INCLUDE_LEG_H_
#define ROSEGAITGENERATOR_INCLUDE_LEG_H_

#include <cmath>
#include "include/rogg/hmatrix.h"
#include "include/rogg/log.h"

namespace ROGG {

/** @brief holds a leg and all its associated properties.*/
class Leg {
 public:
  // constants
  static const double kOutsideReach = -1.0;
  /**< @brief KM value if P is out of bounds.*/
  static const double kCalculationFailed = -2.0;
  /**< @brief something bad happened during calculation of KM.*/
  // constructor
  Leg(int id, Logger* logger);
  // methods
  void set_H_pivot0_P(const double* array);
  void set_L_min(double min_value);
  void SetLegParams(const double* HL1, const double* HL2, const double* pivot0);
  HMatrix H_pivot0_P();
  HMatrix H_cob_pivot0();
  double KM();
  void UpdateKM(const double* velocity);
  double CalculateKMForP(const double* velocity, HMatrix* H_pivot0_Pnew);
  void ClearCache();
  int id();

 private:
  // members
  HMatrix H_pivot0_P_;/**< @brief H-matrix for P relative to pivot0.*/
  HMatrix* active_H_pivot0_P_;/**< @brief pointer to the H_pivot0_P for calc*/
  HMatrix H_pivot0_HL1_;/**< @brief an offset HL1 relative to pivot0*/
  HMatrix H_pivot0_HL2_;/**< @brief an offset HL1 relative to pivot0*/
  HMatrix H_cob_pivot0_;/**< @brief pivot0 frame relative to COB*/
  double KM_;/**< @brief KM value for this leg with its foot at frame P*/
  double L_max_; /**< @brief HL1 length*/
  Logger* logger_;/**<@brief for writing to the logfile*/
  int id_;/**< @brief index in the leg array (0-based)*/
  // methods
  double _CalculateKM(const double* velocity);
};

}  // namespace ROGG

#include "include/rogg/gaitgenerator.h"

#endif  // ROSEGAITGENERATOR_INCLUDE_LEG_H_

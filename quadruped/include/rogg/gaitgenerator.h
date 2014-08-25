/**
 * @file gaitgenerator.h
 */

#ifndef ROSEGAITGENERATOR_INCLUDE_GAITGENERATOR_H_
#define ROSEGAITGENERATOR_INCLUDE_GAITGENERATOR_H_

#include <string.h>
#include <stdint.h>
#include <cmath>
#include <map>

#include "include/rogg/hmatrix.h"
#include "include/rogg/leg.h"
#include "include/rogg/log.h"
#include "include/rogg/lasmbfstorage.h"
#include "include/rogg/areas.h"
#include "include/rogg/stopwatch.h"

#ifndef M_PI
  #define M_PI   3.14159265358979323846264338327950288
#endif

namespace ROGG {

  enum StepResults {
    TRANSFERRING = 0,/**< @brief 0: did nothing*/
    NO_LIFTABLE_LEGS = 1,/**< @brief 1: legliftingCheck failed*/
    NEWFOOTHOLD = 2,/**< @brief 2: a new foothold was calculated*/
    CALCULATE_FAILED = 3,/**< @brief 3: no valid footholds found*/
    END_OF_STATEMACHINE = 4/**< @brief 4: ERROR end of statemachine*/
  };
  enum StateMachineStates {
    WAIT = 2,  ///< 2
    TRANSFER = 1,  ///< 1
    CALCULATE = 0  ///< 0
  };/**< @brief States available for the statemachine*/


/** @brief The main gait-generator class.
 *
 * useful notes:<br>
 * - all indices for the public functions are 1 based (for ease of use with
 * 20-sim)
 * - all indices for the private functions are 0 based
 * - \ref LT_ is (and should) only be set in \ref LegLiftingCheck
 * <br><br>
 * usage:<br>
 * 1. create an instance
 * \code GaitGenerator gait_generator = GaitGenerator(); \endcode
 * 2. set all the parameters (order doesn't matter)
 * \code
 * // a simple 16 valued array we'll use in this example:
 * double example_array[16] = {
 *   1, 0, 0, 0,
 *   0, 1, 0, 0,
 *   0, 0, 1, 0,
 *   0, 0, 0, 1 };
 * gait_generator.set_HL1(example_array);
 * gait_generator.set_HL2(example_array);
 * gait_generator.setLegPivot0(1, example_array);
 * gait_generator.setLegPivot0(2, example_array);
 * gait_generator.setLegPivot0(3, example_array);
 * gait_generator.setLegPivot0(4, example_array);
 * // example array for the transfer speeds V1, V2, and V3
 * double example_transfer_speeds[3] = {1, 2, 3};
 * gait_generator.set_transfer_speeds(example_transfer_speeds);
 * gait_generator.set_ASM_min(0.04);
 * gait_generator.set_ground_clearance(0.05);
 * \endcode
 */
class GaitGenerator {
 public:
  // typedefs
  typedef std::map<double, int> SortedKM;
  typedef union {
    struct {
      Leg* left_front;/**< @brief pointer to left front leg*/
      Leg* left_rear;/**< @brief pointer to left rear leg*/
      Leg* right_rear;/**< @brief pointer to right rear leg*/
      Leg* right_front;/**< @brief pointer to right front leg*/
    } direction;
    Leg* by_index[4];/**< @brief array of leg pointers ordered LF LR RR RF*/
  } LegDirections;/**< @brief data strucure to keep track of leg directions*/
  typedef struct {
    Leg* U;
    Leg* V;
    Leg* W;
  } LegUVW;
  typedef struct {
    double B;
    double F;
  } LASM_struct;/**< @brief struct for transferring both F and B values*/
  // constants
  static const int kNumLegs = 4;
  /**< @brief Number of legs on a quadruped.*/
  // de/con-structors
  GaitGenerator();
  ~GaitGenerator();
  // methods
  HMatrix HL1();
  HMatrix HL2();
  HMatrix H_cob_com();
  Leg leg(int leg_no);  // TODO(michiel): remove this. it's not used
  int LT();
  int state();
  Logger* logger();
  void set_HL1(const double *array);
  void set_HL2(const double *array);
  void set_H_cob_com(const double *array);
  void set_velocity(const double *array);
  void set_ASM_min(const double value);
  void set_transferring(const bool is_transferring);
  void set_external_time(double value);
  void set_transfer_speeds(const double* array);
  void set_ground_clearance(const double value);
  void set_search_width(double value);
  void set_use_enforce_wavegait(bool onoff);
  void set_force_wavegait(bool onoff);
  void set_select_with_margin(bool onoff);
  void SetLegPivot0(int leg_no, const double* array);
  void SetLegP(int leg_no, const double* array);
  static double NormalizeAngle(const double angle);
  double CalculateKMForLeg(int leg_no);
  StepResults Step();
  void PlainLog(const char* text);
  double GetLASMB(int leg_no);
  double GetALASMB();
  double GetLASMF(int leg_no);
  double GetALASMF();
  void CopyH_cob_PLT(double* array);
  double GetTDEstimation();
  void Copy4SearchAreaCorners(double* array64);
  void CopyFullTDArea(double* area_array);
  void CopyFullKMArea(double* area_array);
  double GetLastStepTime();
  void DisableLog();
  void ClearCacheFor(StateMachineStates state);

 private:
  // members
  Logger* logger_;/**< @brief for writing to the logfile*/
  HMatrix HL1_; /**< @brief the HL1 parameter (H-matrix).*/
  HMatrix HL2_; /**< @brief the HL2 parameter (H-matrix).*/
  HMatrix H_cob_com_; /**< @brief frame of COM relative to COB.*/
  Leg* legs_[kNumLegs];/**< @brief the legs.*/
  double velocity_[3];/**< @brief velocity vector (in COB fram).*/
  double LASMB_[kNumLegs];/**< @brief storage for calculated LASMB.*/
  double LASMF_[kNumLegs];/**< @brief storage for calculated LASMF.*/
  double theta_v_;/**< @brief angle between x-axis and velocity.*/
  double speed_;/**< @brief speed of the COM*/
  HMatrix H_com_T_;/**< @brief H-matrix that contains an R based on theta_v*/
  HMatrix H_T_com_;/**< @brief inverse of H_com_T_*/
  HMatrix H_com_P_[4];/**< @brief H-matrices for feet relative to the COM.*/
  bool cached_H_com_P_;/**< @brief indicates if H_com_P is already calculated */
  bool velocity_changed_;/**< @brief indicates velocity has changed.*/
  Leg* leg_RR_during_last_calc_;/**< @brief leg that was right rear last calc*/
  double ASM_min_;/**< @brief ASM_min radius*/
  StateMachineStates state_;/**< @brief state of the statemachine.*/
  bool transferring_;/**< @brief indicates a leg is currently transferring.*/
  int LT_;/**< @brief The index of the leg to calculate for.*/
  int NLT_;/**< @brief The index of the leg that is likely to transfer next.*/
  SortedKM sorted_KM_;/**< @brief KM sorted low to high.*/
  int KM_min_index_;/**< @brief leg index that has the lowest KM (excl LT)*/
  double external_time_;/**< @brief time as received in Step.*/
  double search_width_;/**< @brief width (and height) of search area*/
  int cells_per_row_;/**< cells per row of search area*/
  int middle_index_;/**< center index in search_array_ */
  double search_array_[675];/**< @brief search points around the foot of LT*/
  double TD_array_[225];/**< @brief transfer distance for search area*/
  double SFFA_array_[225];/**< @brief SFFA values for search area*/
  double KM_array_[225];/**< @brief KM values of leg LT for searh area*/
  double ESD_arrays_[4][225];/** @brief ESD_LT_NLT(P) values*/
  double ELD_arrays_[4][225];/** @brief ELD_LT_LJ(P) values*/
  LASMBFStorage LASMBF_storage;/**< @brief storage for LASMBF_LT_#_P values*/
  double TD_min_;/**< @brief minimum transfer distance*/
  double transfer_speeds_[3];/**< @brief V1 V2 and V3*/
  double ground_clearance_;/**< @brief height of the apex of a leg-trajectory.*/
  LegDirections leg_directions_;/**< @brief provide leg access by LF/LR/RR/RF*/
  LegUVW leg_UVW_;/**< @brief provide leg access by U/V/W*/
  BaseArea* area_D_;/**< @brief area D*/
  BaseArea* area_O_;/**< @brief area O*/
  BaseArea* area_C_;/**< @brief area C*/
  BaseArea* areas_A_[kNumLegs];/**< @brief area A for each NLT*/
  BaseArea* areas_B_[kNumLegs];/**< @brief area B for each possible rearleg*/
  BaseArea* area_combined_;/**< @brief combined area*/
  int selected_foothold_;/**< @brief the search array index for the foothold*/
  HMatrix last_target_;/**< @brief last target H_cob_PLT*/
  Stopwatch step_timer_;/**< @brief stopwatch to track step calculation time*/
  bool enforce_wavegait_;/**< @brief only check wave-gait legs, starts true*/
  // TODO(michiel): maybe collect config flags in a single variable
  bool use_enforce_wavegait_;/**<@brief use \ref enforce_wavegait_ or not*/
  bool force_wavegait_;/**<@brief force the wavegait [false]*/
  bool select_with_margin_;
  /**<@brief toggle \ref SelectFoothold or \ref SelectFootholdWithMargin*/

  // methods
  LASM_struct _CalculateLASMBF(int leg_index, HMatrix* H_com_Pnew);
  LASM_struct CalculateLASMBFForP(int leg_index, int search_array_entry);
  double CalculateSFFA_LT(int search_array_entry);
  double CalculateKM_LT(int search_array_entry);
  double CalculateESD_LT(int leg_index, int search_array_entry);
  double CalculateELD_LT(int leg_index, int search_array_entry);
  void CalculateH_com_P();
  double CalculateTD(int search_array_entry);
  bool CalculateNewFoothold();
  bool LegLiftingCheck();
  void GenerateSortedKM();
  void GenerateSearchArray();
  HMatrix GetH_com_Pnew(int search_array_entry);
  Leg* GetClosestRearLeg(int leg_index);
  void UpdateLASMBF(int leg_index);
  void UpdateVelocityValues();
  void UpdateLegDirections();
  void UpdateUVW();
  void UpdateKM_min();  // call this when LT changes
  bool GenerateCombined(int leg_index, bool leg_index_was_NLT);
  void LogKM_array();
  void LogTD_array();
  void LogELD_array(int leg_index);
  void LogAreas();
  void LogSearchArray();
  bool CheckCriteriaD(int search_array_entry);
  bool CheckCriteriaO(int search_array_entry);
  bool CheckCriteriaC(int search_array_entry);
  bool CheckCriteriaA(int leg_index, int search_array_entry);
  bool CheckCriteriaB(int leg_index, int search_array_entry);
  void SelectFoothold();
  void SelectFootholdWithMargin();
};

}  // namespace ROGG

#endif  // ROSEGAITGENERATOR_INCLUDE_GAITGENERATOR_H_

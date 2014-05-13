/**
 * @file pivot.h
 */

#ifndef QUADRUPED_INCLUDE_PIVOT_H_
#define QUADRUPED_INCLUDE_PIVOT_H_

#include <stdint.h>

#include <cmath>

#include "include/hmatrix.h"
#include "include/utility.h"

#ifndef M_PI
  #define M_PI 3.14159265358979323846264338327950288
#endif

namespace Q1 {

/** @brief represents a pivot of a leg
 *
 * Pivots rotate around their Z-axis.<br>
 * -abs_max_angle_ &gt; + angle_ &gt; + abs_max_angle_<br>
 * offset_angle_ is meant to be the middle of the angle range. so
 * the range is (offset - abs_max, offset + abs_max)<br>
 * <br>
 * Pivots should be configured once, after that only angle should be changed<br>
 * Usage:<br>
 * 1. Create instance
 * \code
 * Pivot pivotA = Pivot(NULL); // no parent
 * Pivot pivotB = Pivot(pivotA.framep()); \\ with pivotA as parent
 \endcode
 * 2. configure the orientation of the pivot.
 * \code pivotA.ConfigureRot(X_AXIS, 1.57); \endcode
 * 3. set the abs_max_angle. It's been reset during set_axis and you can't
 * do much until it's been set again
 * \code pivotA.set_abs_max_angle(0.3); // 0.3 rad \endcode
 * 4. set the offset angle
 * \code pivotA.set_offset_angle(1.57); \endcode
 * - you can now change or set the angle all you want
 * \code bool success = pivotA.set_angle(1.4); \endcode
 * - you can retrieve the local frame \ref H_frame_ or the frame relative to
 * the origin \ref H_rel_
 * \code
 * HMatrix local = pivotA.H_frame();
 * HMatrix relative = pivotA.H_frame();
 * // PivotA doesn't have a parent, no parent = origin
 * // so relative is the same as local in this case
 * \endcode
 * <br>
 * we now have a pivot that's been rotated around the x-axis by a 90 degrees.
 * it's actuated rotation is around its own z-axis and in neutral it makes an angle
 * of 1.57 rad. it can have any angle between 1.27 and 1.87 rad
 *
 */
class Pivot {
 public:
  // constructors
  Pivot();
  explicit Pivot(HMatrix* parent);
  // accessors mutators
  void set_abs_max_angle(double angle);
  void set_offset_angle(double angle);
  void set_parent(HMatrix* parent);
  bool set_angle(double angle);
  double angle();
  HMatrix H_frame();
  HMatrix* H_framep();
  // functions
  void UpdateH_rel();
  bool ChangeAngle(double delta_angle);
  void SetPosition(double x, double y, double z);
  bool IsInRange(double angle);
  const double* GetHMatrixArray();
  const double* GetRelativeHMatrixArray();
  void ConfigureRot(Axis axis, double angle);
  HMatrix GetSpecificH(HMatrix* target);

 private:
  double abs_max_angle_;/**< @brief the absolute maximum angle*/
  double offset_angle_;/**< @brief the offset angle*/
  double angle_;/**< @brief the current angle*/
  HMatrix H_frame_;
  /**< @brief HMatrix for this pivot relative to the previous (or COB)*/
  HMatrix H_rel_;/**< @brief HMatrix relative to 0*/
};

}  // namespace Q1

#endif  // QUADRUPED_INCLUDE_PIVOT_H_

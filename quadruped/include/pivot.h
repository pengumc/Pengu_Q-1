/**
 * @file pivot.h
 */

#ifndef QUADRUPED_INCLUDE_PIVOT_H_
#define QUADRUPED_INCLUDE_PIVOT_H_

#include <stdint.h>

#include <cmath>

#include "include/hmatrix.h"

#ifndef M_PI
  #define M_PI 3.14159265358979323846264338327950288
#endif

namespace Q1 {

/** @brief represents a pivot of a leg
 *
 * internal - abs_max_angle_ &gt; + angle_ &gt; + abs_max_angle_<br>
 * offset_angle_ is meant to be the middle of the angle range. so
 * the range is (offset - abs_max, offset + abs_max)
 */
class Pivot {
 public:
  // types
  typedef enum {
    X_AXIS = 0,
    Y_AXIS = 1,
    Z_AXIS = 2
  } Axis;
  // constructors
  Pivot();
  // accessors mutators
  void set_abs_max_angle(double angle);
  void set_offset_angle(double angle);
  bool set_angle(double angle);
  void set_axis(Axis axis);
  // functions
  bool ChangeAngle(double delta_angle);
  void SetPosition(double x, double y, double z);

 private:
  double abs_max_angle_;/**< @brief the absolute maximum angle*/
  double offset_angle_;/**< @brief the offset angle*/
  double angle_;/**< @brief the current angle*/
  Axis axis_;/**< @brief the axis along which this pivot rotates*/
  HMatrix H_frame_;
  /**< @brief HMatrix for this pivot relative to the previous (or COB)*/
};

}  // namespace Q1

#endif  // QUADRUPED_INCLUDE_PIVOT_H_

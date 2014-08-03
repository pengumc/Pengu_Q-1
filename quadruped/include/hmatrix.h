/** 
 * @file hmatrix.h
 */

#ifndef QUADRUPED_INCLUDE_HMATRIX_H_
#define QUADRUPED_INCLUDE_HMATRIX_H_

#include <string.h>
#include <cmath>
#include "include/log.h"


namespace Q1 {

/** @brief This is used across classes so it can't be in one :(*/
typedef enum {
  X_AXIS = 0, /**< 0*/
  Y_AXIS = 1, /**< 1*/
  Z_AXIS = 2  /**< 2*/
} Axis;


/** @brief Holds a 4x4 Homogeneous matrix.
 *
 * indices of \ref array_ <br>
 * <SPAN class="extra_red">00, 01, 02,</SPAN> <span class="extra_blue">03,
 * </span> <br>
 * <SPAN class="extra_red">04, 05, 06,</SPAN> <span class="extra_blue">07,
 * </span> <br>
 * <SPAN class="extra_red">08, 09, 10,</SPAN> <span class="extra_blue">11,
 * </span> <br>
 * 12, 13, 14, 15 <br>
 * <br>
 * Row 4 is always assumed 0, 0, 0, 1 <br>
 * The <span class="extra_red">
 * red</span> entries are referred to as the rotation matrix <b>R</b><br>
 * The <span class="extra_blue">blue</span> entries are referred to as 
 * translation vecot <b>T</b>
 */
class HMatrix {
 public:
  // types
  // constants
  static const int kMagic16 = 16;/**<@brief Number of elements in a H-matrix.*/
  static const int kX = 3;/**< @brief index in HMatrix for the x coordinate*/
  static const int kY = 7;/**< @brief index in HMatrix for the y coordinate*/
  static const int kZ = 11;/**< @brief index in HMatrix for the z coordinate*/
  // de/con-structors
  HMatrix();
  explicit HMatrix(const double* array);
  HMatrix(double x, double y, double z);
  HMatrix(Axis axis, double angle);
  // methods
  void set_parent(HMatrix* parent);
  HMatrix* parent();
  const double* array();
  double GetX();
  void SetX(double value);
  double GetY();
  void SetY(double value);
  double GetZ();
  void SetZ(double value);
  double Distance();
  void set_array(const double* array);
  void Copy(HMatrix src);
  HMatrix Dot(HMatrix right);
  void SelfDot(HMatrix right);
  void SelfDotScaled(HMatrix right, double factor);
  HMatrix Inverse();
  void CounterRotateVector(const double* vector, double* vector_out);
  void RotateVector(const double* vector, double* vector_out);
  void LogMatrix(Logger* logger);
  void Clear();

 private:
  // members
  double array_[kMagic16];/**<@brief The array that holds the H-matrix values.*/
  HMatrix* parent_;/**< @brief parent in the transform tree, NULL = origin*/
};

}  // namespace Q1

#endif  // QUADRUPED_INCLUDE_HMATRIX_H_

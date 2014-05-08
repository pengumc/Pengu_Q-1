/**
 * @file hmatrix.cpp
 * Hmatrices and stuff
 */
#include "include/hmatrix.h"

namespace Q1 {
// ------------------------------------------------------------------Constructor
/** @brief Constructor for HMatrix
 * The matrix is initialized to the identity matrix. <b>I</b>(4)
 */
HMatrix::HMatrix() {
  // initialize to identity matrix
  array_[0] = 1.0;
  array_[1] = 0.0;
  array_[2] = 0.0;
  array_[3] = 0.0;
  array_[4] = 0.0;
  array_[5] = 1.0;
  array_[6] = 0.0;
  array_[7] = 0.0;
  array_[8] = 0.0;
  array_[9] = 0.0;
  array_[10] = 1.0;
  array_[11] = 0.0;
  array_[12] = 0.0;
  array_[13] = 0.0;
  array_[14] = 0.0;
  array_[15] = 1.0;
}

// -----------------------------------------------------------------Constructor2
/** @brief Constructor with intial matrix
 * @param array The array with which to initialize. Make sure at least 16 values
 * are accessible.
 */
HMatrix::HMatrix(const double* array) {
  for (int i = 0; i < kMagic16; ++i) {
    array_[i] = array[i];
  }
}

// -----------------------------------------------------------------Constructor3
/** @brief Constructor with initial coordinates.
 *
 * The HMatrix is initialized with <b>R</b> = <b>I</b>(3)
 *  and <b>T</b> = [x; y; z]
 * @param x the x coordinate
 * @param y the y coordinate
 * @param z the z coordinate
 */
HMatrix::HMatrix(double x, double y, double z) {
  array_[0] = 1.0;
  array_[1] = 0.0;
  array_[2] = 0.0;
  array_[3] = x;
  array_[4] = 0.0;
  array_[5] = 1.0;
  array_[6] = 0.0;
  array_[7] = y;
  array_[8] = 0.0;
  array_[9] = 0.0;
  array_[10] = 1.0;
  array_[11] = z;
  array_[12] = 0.0;
  array_[13] = 0.0;
  array_[14] = 0.0;
  array_[15] = 1.0;
}

// -----------------------------------------------------------------Constructor4
/** @brief create a HMatrix with only <b>R</b> for a rotation along 1 axis*/
HMatrix::HMatrix(Axis axis, double angle) {
  array_[3] = 0.0;
  array_[7] = 0.0;
  array_[11] = 0.0;
  array_[12] = 0.0;
  array_[13] = 0.0;
  array_[14] = 0.0;
  array_[15] = 0.0;
  switch (axis) {
    case Z_AXIS: {
      array_[0] = std::cos(angle);
      array_[1] = -std::sin(angle);
      array_[2] = 0.0;
      array_[4] = std::sin(angle);
      array_[5] = std::cos(angle);
      array_[6] = 0.0;
      array_[8] = 0.0;
      array_[9] = 0.0;
      array_[10] = 1.0;
      break;
    }
    case Y_AXIS: {
      array_[0] = std::cos(angle);
      array_[1] = 0.0;
      array_[2] = std::sin(angle);
      array_[4] = 0.0;
      array_[5] = 1.0;
      array_[6] = 0.0;
      array_[8] = -std::sin(angle);
      array_[9] = 0.0;
      array_[10] = std::cos(angle);
      break;
    }
    case X_AXIS: {
      array_[0] = 1.0;
      array_[1] = 0.0;
      array_[2] = 0.0;
      array_[4] = 0.0;
      array_[5] = std::cos(angle);
      array_[6] = -std::sin(angle);
      array_[8] = 0.0;
      array_[9] = std::sin(angle);
      array_[10] = std::cos(angle);
      break;
    }
  }
}

// -------------------------------------------------------------------------Copy
/** @brief Copy data from another HMatrix*/
void HMatrix::Copy(const HMatrix src) {
  for (int i = 0; i < kMagic16; ++i) {
    array_[i] = src.array_[i];
  }
}

// ---------------------------------------------------------------array accessor
/** @brief Accessor function for \ref array_
 * @returns A read only pointer to the values of the HMatrix
 */
inline const double* HMatrix::array() {
  return array_;
}

// ----------------------------------------------------------------array mutator
/** @brief Mutator function for the Hmatrix values
 * @param array Values to write to the array. Make sure at least 16 values are
 * accessible
 */
void HMatrix::set_array(const double* array) {
  for (int i = 0; i < kMagic16; ++i) {
    array_[i] = array[i];
  }
}

// --------------------------------------------------------------------LogToFile
/** @brief Logs the values of the H-matrix to a textfile
  * @param logger a Logger instance to use for logging.
  */
void HMatrix::LogMatrix(Logger* logger) {
  logger->LogToFile("\n[ %+.2f, %+.2f, %+.2f, %+f,\n", array_[0], array_[1],
                     array_[2], array_[3]);
  logger->LogToFile("  %+.2f, %+.2f, %+.2f, %+f,\n", array_[4], array_[5],
                     array_[6], array_[7]);
  logger->LogToFile("  %+.2f, %+.2f, %+.2f, %+f,\n", array_[8], array_[9],
                     array_[10], array_[11]);
  logger->LogToFile("  %+.2f, %+.2f, %+.2f, %+f]\n\n", array_[12], array_[13],
                     array_[14], array_[15]);
}

// --------------------------------------------------------------------------Dot
/** @brief Calculates the dot product of two matrices
 * @param right The right hand of the dot operation.
 * @return A HMatrix contiaining the result of this * right
 */
HMatrix HMatrix::Dot(HMatrix right) {
  double result[kMagic16];
  // rv          * lv
  // 00 01 02 03   00 01 02 03   00*00 + 01*04 + 02*08 + 03*12 etc.
  // 04 05 06 07 * 04 05 06 07 =
  // 08 09 10 11   08 09 10 11
  // 12 13 14 15   12 13 14 15
  const double* rv = right.array();  // values of right H-matrix
  const double* lv = array_;  // values of left H-matrix
  result[0] = lv[0]*rv[0] + lv[1]*rv[4] + lv[2]*rv[8] + lv[3]*rv[12];
  result[1] = lv[0]*rv[1] + lv[1]*rv[5] + lv[2]*rv[9] + lv[3]*rv[13];
  result[2] = lv[0]*rv[2] + lv[1]*rv[6] + lv[2]*rv[10] + lv[3]*rv[14];
  result[3] = lv[0]*rv[3] + lv[1]*rv[7] + lv[2]*rv[11] + lv[3]*rv[15];

  result[4] = lv[4]*rv[0] + lv[5]*rv[4] + lv[6]*rv[8] + lv[7]*rv[12];
  result[5] = lv[4]*rv[1] + lv[5]*rv[5] + lv[6]*rv[9] + lv[7]*rv[13];
  result[6] = lv[4]*rv[2] + lv[5]*rv[6] + lv[6]*rv[10] + lv[7]*rv[14];
  result[7] = lv[4]*rv[3] + lv[5]*rv[7] + lv[6]*rv[11] + lv[7]*rv[15];

  result[8] = lv[8]*rv[0] + lv[9]*rv[4] + lv[10]*rv[8] + lv[11]*rv[12];
  result[9] = lv[8]*rv[1] + lv[9]*rv[5] + lv[10]*rv[9] + lv[11]*rv[13];
  result[10] = lv[8]*rv[2] + lv[9]*rv[6] + lv[10]*rv[10] + lv[11] *rv[14];
  result[11] = lv[8]*rv[3] + lv[9]*rv[7] + lv[10]*rv[11] + lv[11] *rv[15];

  result[12] = lv[12]*rv[0] + lv[13]*rv[4] + lv[14]*rv[8] + lv[15]*rv[12];
  result[13] = lv[12]*rv[1] + lv[13]*rv[5] + lv[14]*rv[9] + lv[15]*rv[13];
  result[14] = lv[12]*rv[2] + lv[13]*rv[6] + lv[14]*rv[10] + lv[15]*rv[14];
  result[15] = lv[12]*rv[3] + lv[13]*rv[7] + lv[14]*rv[11] + lv[15]*rv[15];

  return HMatrix(result);
}

// ----------------------------------------------------------CounterRotateVector
/** @brief write <b>R<SUP>-1</SUP></b> * <b>vector_in</b> to <b>vector_out</b>.
  * 
  * @param vector_in a 3 element vector as [x; y; z]
  * @param vector_out writeable 3 element vector. <br> 
  */
void HMatrix::CounterRotateVector(const double* vector_in, double* vector_out) {
  // transposed R matrix * vector_in
  // 00 04 08              0   00*0 + 04*1 + 08*2
  // 01 05 09            * 1 =
  // 02 06 10              2
  vector_out[0] = array_[0]*vector_in[0] + array_[4]*vector_in[1]
                  + array_[8]*vector_in[2];
  vector_out[1] = array_[1]*vector_in[0] + array_[5]*vector_in[1]
                  + array_[9]*vector_in[2];
  vector_out[2] = array_[2]*vector_in[0] + array_[6]*vector_in[1]
                  + array_[10]*vector_in[2];
}

// -----------------------------------------------------------------RotateVector
/** @brief write <b>R</b> * <b>vector_in</b> to <b>vector_out</b>.
  * 
  * @param vector_in a 3 element vector as [x; y; z]
  * @param vector_out writeable 3 element vector.
  */
void HMatrix::RotateVector(const double* vector_in, double* vector_out) {
  // transposed R matrix * vector_in
  // 00 01 02              0   00*0 + 01*1 + 02*2
  // 04 05 06            * 1 = 04*0 + 05*1 + 06*2 etc
  // 08 09 10              2
  vector_out[0] = array_[0]*vector_in[0] + array_[1]*vector_in[1]
                  + array_[2]*vector_in[2];
  vector_out[1] = array_[4]*vector_in[0] + array_[5]*vector_in[1]
                  + array_[6]*vector_in[2];
  vector_out[2] = array_[8]*vector_in[0] + array_[9]*vector_in[1]
                  + array_[10]*vector_in[2];
}


// --------------------------------------------------------------------Inverse()
/** @brief Invert a HMatrix
 * 
 * @returns An inverted HMatrix.
 */
HMatrix HMatrix::Inverse() {
  // This is assuming the bottom row is 0 0 0 1, otherwise it gets complicated
  // 00 01 02 03      00 04 08           -03
  // 04 05 06 07 ==>  01 05 09 & inv R * -07
  // 08 09 10 11      02 06 10           -11
  // 12 13 14 15      00 00 00

  // setup the inverse R and -T
  double inverse_array[16] = {
    array_[0], array_[4], array_[8], -array_[3],
    array_[1], array_[5], array_[9], -array_[07],
    array_[2], array_[6], array_[10], -array_[11],
    0.0, 0.0, 0.0, 1.0
  };
  // inv R * new T
  inverse_array[3] = -(inverse_array[0] * array_[3] +
                     inverse_array[1] * array_[7] +
                     inverse_array[2] * array_[11]);
  inverse_array[7] = -(inverse_array[4] * array_[3] +
                     inverse_array[5] * array_[7] +
                     inverse_array[6] * array_[11]);
  inverse_array[11] =-(inverse_array[8] * array_[3] +
                      inverse_array[9] * array_[07] +
                      inverse_array[10] * array_[11]);

  HMatrix H(inverse_array);
  return H;
}

// -------------------------------------------------------------------------GetX
/** @brief return the x coordinate value of the HMatrix
 */
double HMatrix::GetX() {
  return array_[kX];
}

// -------------------------------------------------------------------------GetY
/** @brief return the y coordinate value of the HMatrix
 */
double HMatrix::GetY() {
  return array_[kY];
}

// -------------------------------------------------------------------------GetZ
/** @brief return the z coordinate value of the HMatrix
 */
double HMatrix::GetZ() {
  return array_[kZ];
}

// -------------------------------------------------------------------------SetX
/** @brief set the value for the x coordinate in the h-matrix
 *
 * @param value the new x coordinate
 */
void HMatrix::SetX(double value) {
  array_[kX] = value;
}

// -------------------------------------------------------------------------SetY
/** @brief set the value for the y coordinate in the h-matrix
 *
 * @param value the new y coordinate
 */
void HMatrix::SetY(double value) {
  array_[kY] = value;
}

// -------------------------------------------------------------------------SetZ
/** @brief set the value for the z coordinate in the h-matrix
 *
 * @param value the new z coordinate
 */
void HMatrix::SetZ(double value) {
  array_[kZ] = value;
}

// ---------------------------------------------------------------------Distance
/** @brief get the distance of the transformation*/
double HMatrix::Distance() {
  return std::sqrt(std::pow(array_[kX], 2) +
                   std::pow(array_[kY], 2) +
                   std::pow(array_[kZ], 2));
}

}  // namespace Q1

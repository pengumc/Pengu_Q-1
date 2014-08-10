/**
 * @file storageLASMBF
 */

#ifndef ROSEGAITGENERATOR_INCLUDE_LASMBFSTORAGE_H_
#define ROSEGAITGENERATOR_INCLUDE_LASMBFSTORAGE_H_

#include <string.h>

namespace ROGG {

/** @brief For storing the LASMB and F values for each entry in the
 * search_array, for each leg.
 *
 * This is essentially a 3D array of 2 x 4 x 225 values<br>
 * Usage:<br>
 * 1. Create an instance
 * \code LASMBFStorage lasmbf = LASMBFStorage(); \endcode
 * - store the LASMB and LASMF values for a leg at an index with \ref Store
 * \code lasmbf.Store(0, 15, -5.3, 4.2);
 * // Stores the value for the LASMB (5.3) and the LASMF (4.2) at index 15 for 
 * // the first leg (0-based index).
 * \endcode
 * -  retrieve values for the LASMB or LASMF with \ref GetLASMB or GetLASMF
 * \code 
 * // the LASMF at index 57 for the 3rd leg:
 * double LASMF = lasmbf.GetLASMF(2, 57);
 * \endcode
 * - Clear all stored values with \ref Clear
 * - Check if an index has been filled (i.e. not cleared) with \ref IsFilled
 */
class LASMBFStorage {
 public:
  // constants
  static const int kSize = 225;
  // methods
  void Store(int leg_index, int search_array_entry, double LASMB, double LASMF);
  double GetLASMB(int leg_index, int search_array_entry);
  double GetLASMF(int leg_index, int search_array_entry);
  bool IsFilled(int leg_index, int search_array_entry);
  void Clear();
 private:
  // members
  double LASMB_values_[4*kSize];
  double LASMF_values_[4*kSize];
};

}  // namespace ROGG

#endif  // ROSEGAITGENERATOR_INCLUDE_LASMBFSTORAGE_H_

/**
 * @file stopwatch.h
 */

#ifndef ROSEGAITGENERATOR_INCLUDE_STOPWATCH_H_
#define ROSEGAITGENERATOR_INCLUDE_STOPWATCH_H_

#include <ctime>

#include "include/rogg/log.h"

namespace ROGG {

/** @brief keeps track of time.
 *
 * usage: <br>
 * 1. create an instance \code Stopwatch S = Stopwatch(); \endcode
 * 2. call Start \code S.Start(); \endcode
 * 3. call Stop \code double milliseconds_since_start = S.Stop() \endcode
 * <br>
 * call \ref last to retrieve the last stored duration (same value that was 
 * returned from Stop). This value is reset when \ref Start is called again.<br>
 * <br>
 * The resolution of the stopwatch is dependent on the OS. (windows seems to 
 * have a 5 ms resolution).<br>
 * <br>
 * NOTE: time storage will wrap at 2147483648 ms (about 596 hours)
 */
class Stopwatch {
 public:
  Stopwatch();
  void Start();
  double Stop();
  double last();

 private:
  std::clock_t c_mark1_; /**< @brief current process running time at Start*/
  std::clock_t c_mark2_;/**< @brief current process running time at Stop*/
  double last_;/**< @brief difference in start and stop times in milliseconds*/
};

}  // namespace ROGG

#endif  // ROSEGAITGENERATOR_INCLUDE_STOPWATCH_H_

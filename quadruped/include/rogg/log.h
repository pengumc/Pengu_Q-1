/** 
 * @file log.h
 */

#ifndef ROSEGAITGENERATOR_INCLUDE_LOG_H_
#define ROSEGAITGENERATOR_INCLUDE_LOG_H_

#include <stdio.h>
#include <stdarg.h>

namespace ROGG {

/** @brief For writing to the logfile
 * 
 * The logfile is opened once, and kept open until the \ref Logger instance
 * is destroyed, disabled or \ref CloseLog is called.
 * Usage:
 * 1. Create an instance
 * \code Logger logger = Logger(); \endcode
 * 2. Open a logfile
 * \code logger.OpenLog("logfile.txt"); \endcode
 * This immediately clears the contents of logfile.txt
 * - write anything to the logfile as if using printf:
 * \code logger.LogToFile("some integer = %d\n", some_integer); \endcode
 * - use \ref set_enabled to enable or disable the logfile.
 * \code logger.set_enabled(false); 
 * // Any calls to LogToFile are now ignored.
 * \endcode
 * WARNING: re-enabling the logfile is not supported.
*/
class Logger {
 public:
  Logger();
  void LogToFile(const char* format, ...);
  void OpenLog(const char* filename);
  void CloseLog();
  void set_enabled(bool b);

 private:
  FILE* f_; /**< @brief filehande for the logfile*/
  bool enabled_;
};

}  // namespace ROGG

#endif  // ROSEGAITGENERATOR_INCLUDE_LOG_H_

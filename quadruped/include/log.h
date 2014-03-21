/**
 * @file log.h
 */

#ifndef QUADRUPED_INCLUDE_LOG_H_
#define QUADRUPED_INCLUDE_LOG_H_
#include <stdarg.h>

namespace Q1 {

/** @brief For writing to the logfile*/
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
  /**< @brief will prevent log entries from being written
   * when set to false.*/
};

}  // namespace Q1

#endif  // QUADRUPED_INCLUDE_LOG_H_

/**
 * @file log.cpp
 * some utility logging functions
 */
#include "include/log.h"

namespace Q1 {

// -------------------------------------------------------------------Contructor
/** @brief contructor 
 *
 * enabled = true
 */ 
Logger::Logger() {
  f_ = NULL;
  enabled_ = true;
}

// --------------------------------------------------------------enabled mutator
/** @brief Mutator function for enabled_
 *
 * @param b value to set \ref enabled_ to.
 * If the logfile was open and loging is disabled, then the file is also closed.
 * and \ref CloseLog is called
 */
void Logger::set_enabled(bool b) {
  enabled_ = b;
  if (!b && f_ != NULL) {
    CloseLog();
  }
}

// ----------------------------------------------------------------------OpenLog
/** @brief open the logfile for writing (clears contents)
 *
 * This also enables the logger if it was turned off
 * @param filename The filename of_ the logfile
 */
void Logger::OpenLog(const char* filename) {
  f_ = fopen(filename, "w");
  enabled_ = true;
  fprintf(f_, "Logger::OpenLog - Log opened\n");
}

// --------------------------------------------------------------------LogToFile
/** @brief take printf style input and write it to the logfile
 *
 * The input works the same as a printf function
 */
void Logger::LogToFile(const char* format, ...) {
  if (!enabled_) return;
  va_list arguments;
  va_start(arguments, format);
  vfprintf(f_, format, arguments);
  va_end(arguments);
  fflush(f_);
}

// ---------------------------------------------------------------------CloseLog
/** @brief Close the logfile */
void Logger::CloseLog() {
  fprintf(f_, "Logger::CloseLog - Closing log\n");
  fclose(f_);
}

}  // namespace Q1

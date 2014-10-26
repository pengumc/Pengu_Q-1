/**
 * @file usbcom.h
 */

#ifndef QUADRUPED_INCLUDE_USBCOM_H_
#define QUADRUPED_INCLUDE_USBCOM_H_

#include <stdint.h>
#include <wchar.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "include/hidapi.h"

namespace Q1 {

/** @brief usb communcations using hidapi*/
class UsbCom {
 public:
  // constants
  static const double kMaxPulsewidth = 2107e-6;
  static const double kTimeConstant = 12e6/22.0;
  static const int kDeviceServoCount = 12;
  /**< number of servos. Should match \ref Quadruped::kLegCount * \ref
   * Leg::kPivotCount */
  static const int kUsbWriteBufferSize = kDeviceServoCount*2 + 2;
  /**< should correspond to the <b>input</b> report size (in bytes) +1 of the
   *  usbmaster https://github.com/pengumc/usbtoi2c <br>
   * the assumption is made that the write buffer size = 2 + number of pivots*2
   * in the robot
   */
  static const int kUsbReadBufferSize = 8;
  /**< should correspond to the <b>output</b> report size (in bytes) of the
   *  usbmaster href"https://github.com/pengumc/usbtoi2c">UsbToI2C*/
  static const uint8_t kUsbCustomGetPos0To3 = 0x05;
  static const uint8_t kUsbCustomGetPos4To7 = 0x06;
  static const uint8_t kUsbCustomGetPos8To11 = 0x07;
  static const uint8_t kUsbCustomSetData = 0x03;
  static const uint8_t kUsbSuccess[kUsbReadBufferSize];
  // constructor, destructor
  UsbCom();
  ~UsbCom();
  // accessors, mutators
  const wchar_t* last_error();
  const double* device_servo_pulsewidths();
  hid_device* handle();
  // functions
  int Connect(uint16_t vid, uint16_t pid);
  int ReadServoPulsewidths();
  int WriteServoPulsewidths(const double* pulsewidths);
  const uint8_t* ReadMiscData();

 private:
  hid_device* handle_;/**< @brief usb device handle*/
  uint16_t vid_;/**< @brief vendor id*/
  uint16_t pid_;/**< @brief product id*/
  uint8_t wbuf_[kUsbWriteBufferSize];/**< @brief write buffer*/
  uint8_t rbuf_[kUsbReadBufferSize];/**< @brief read buffer*/
  uint8_t misc_buf_[kUsbReadBufferSize];  ///< last read data
  const wchar_t* last_error_;
  double device_servo_pulsewidths_[kDeviceServoCount];  ///< last read pw
  /**< @brief K factor
   *
   * old: angle = K * (PW - PW<sub>mid</sub>)<br>
   * new:<br>
   * pw = 2107e-6 - x*22/12e6<br>
   * x = 1500e-6 - (angle/full_range * 1000e-6)<br>
   */
};

}  // namespace Q1

#endif  // QUADRUPED_INCLUDE_USBCOM_H_

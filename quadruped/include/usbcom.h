/**
 * @file usbcom.h
 */

#ifndef QUADRUPED_INCLUDE_USBCOM_H_
#define QUADRUPED_INCLUDE_USBCOM_H_

#include <stdint.h>
#include <wchar.h>
#include <stdio.h>
#include <string.h>

#include "include/hidapi.h"

namespace Q1 {

/** @brief usb communcations using hidapi*/
class UsbCom {
 public:
  // constants
  static const double kDefaultK = -0.034;/**< default value for \ref K_*/
  static const double kMidPW = 72;
  static const int kDeviceServoCount =12;
  /**< number of servos. Should match \ref Quadruped::kLegCount * \ref 
   * Leg::kPivotCount */
  static const int kUsbWriteBufferSize = kDeviceServoCount + 2;
  /**< should correspond to the <b>input</b> report size (in bytes) +1 of the
   *  usbmaster https://github.com/pengumc/usbtoi2c <br>
   * the assumption is made that the write buffer size = 2 + number of pivots 
   * in the robot
   */
  static const int kUsbReadBufferSize = 8;
  /**< should correspond to the <b>output</b> report size (in bytes) of the
   *  usbmaster href"https://github.com/pengumc/usbtoi2c">UsbToI2C*/
  static const int kHighPosCount = kDeviceServoCount - kUsbReadBufferSize;
  /**< number of bytes to use from the result of reading pos high*/
  static const uint8_t kUsbCustomGetPosL = 0x05;
  static const uint8_t kUsbCustomGetPosH = 0x06;
  static const uint8_t kUsbCustomSetData = 0x03;
  static const uint8_t kUsbSuccess[kUsbReadBufferSize];
  // constructor, destructor
  UsbCom();
  ~UsbCom();
  // accessors, mutators
  void set_K(double new_K);
  const wchar_t* last_error();
  const double* device_servo_angles();
  // functions
  int Connect(uint16_t vid, uint16_t pid);
  int ReadServoAngles();
  int WriteServoAngles(const double* servo_angles);


 private:
  hid_device* handle_;/**< @brief usb device handle*/
  uint16_t vid_;/**< @brief vendor id*/
  uint16_t pid_;/**< @brief product id*/
  uint8_t wbuf_[kUsbWriteBufferSize];/**< @brief write buffer*/
  uint8_t rbuf_[kUsbReadBufferSize];/**< @brief read buffer*/
  const wchar_t* last_error_;
  double device_servo_angles_[kDeviceServoCount];/**< last read angles*/
  double K_;
  /**< @brief K factor
   *
   * angle = K * (PW - PW<sub>mid</sub>)
   */
};

}  // namespace Q1

#endif  // QUADRUPED_INCLUDE_USBCOM_H_

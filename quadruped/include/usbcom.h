/**
 * @file usbcom.h
 */

#ifndef QUADRUPED_INCLUDE_USBCOM_H_
#define QUADRUPED_INCLUDE_USBCOM_H_

#include <stdint.h>

#include "include/hidapi.h"

namespace Q1 {

/** @brief usb communcations using hidapi*/
class UsbCom {
 public:
  // constants
  static const int kUsbWriteBufferSize = 14;
  /**< should correspond to the <b>input</b> report size (in bytes) +1 of the
   *  usbmaster https://github.com/pengumc/usbtoi2c*/
  static const int kUsbReadBufferSize = 8;
  /**< should correspond to the <b>output</b> report size (in bytes) of the
   *  usbmaster href"https://github.com/pengumc/usbtoi2c">UsbToI2C*/
  // constructor, destructor
  UsbCom(uint16_t vid, uint16_t pid);
  ~UsbCom();
  // functions
  int Connect();

 private:
  hid_device* handle_;/**< @brief usb device handle*/
  uint16_t vid_;/**< @brief vendor id*/
  uint16_t pid_;/**< @brief product id*/
  uint8_t wbuf_[kUsbWriteBufferSize];/**< @brief write buffer*/
  uint8_t rbuf_[kUsbReadBufferSize];/**< @brief read buffer*/
};

}  // namespace Q1

#endif  // QUADRUPED_INCLUDE_USBCOM_H_

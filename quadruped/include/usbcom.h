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
   *  usbmaster <a href"https://github/com/pengumc/usbtoi2c">UsbToI2C</a>*/
  static const int kUsbReadBufferSize = 8;
  /**< should correspond to the <b>output</b> report size (in bytes) of the
   *  usbmaster <a href"https://github/com/pengumc/usbtoi2c">UsbToI2C</a>*/
  // constructor, destructor
  UsbCom(uint16_t vid, uint16_t pid);
  ~UsbCom();
  // functions
  int Connect();

 private:
  hid_device* handle_;/**< usb device handle*/
  uint16_t vid_;/**< vendor id*/
  uint16_t pid_;/**< product id*/
  uint8_t wbuf_[kUsbWriteBufferSize];/**< buffer for writing to the device*/
  uint8_t rbuf_[kUsbReadBufferSize];/**< buffer for reading from the device*/
};

}  // namespace Q1

#endif  // QUADRUPED_INCLUDE_USBCOM_H_

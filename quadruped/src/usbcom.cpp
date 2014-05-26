/**
 * @file usbcom.cpp
 */
#include "include/usbcom.h"

namespace Q1 {

// ------------------------------------------------------------------Constructor
/** @brief constructor
 * 
 * pid and vid are stored, no connection is made at this time.
 * @param vid the vendor id of the usb device to use
 * @param pid the product id
 */
UsbCom::UsbCom(uint16_t vid, uint16_t pid) {
  vid_ = vid;
  pid_ = pid;
  handle_ = NULL;
}

// -------------------------------------------------------------------Destructor
/** @brief destructor, also disconnects device if one was connected*/
UsbCom::~UsbCom() {
  if (handle_) {
    hid_close(handle_);
  }
  hid_exit();
}

// ----------------------------------------------------------------------Connect
/** @brief connect to the first device with \ref vid_ \ref pid_
 * 
 * @retval 0 on success
 * @retval 1 if hid_init failed
 * @retval 2 if device couldn't be opened
 */
int UsbCom::Connect() {
  int init_result = hid_init();
  if (init_result) {
    handle_ = NULL;
    return 1;
  }
  handle_ = hid_open(vid_, pid_, NULL);
  if (!handle_) {
    return 2;
  }
  return 0;
}

}  // namespace Q1

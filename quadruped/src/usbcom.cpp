/**
 * @file usbcom.cpp
 */
#include "include/usbcom.h"

namespace Q1 {

const uint8_t UsbCom::kUsbSuccess[UsbCom::kUsbReadBufferSize] =
  {'s', 'u', 'c', 'c', 'e', 's', 's', ' '};

// ------------------------------------------------------------------Constructor
/** @brief constructor
 *
 * pid and vid are stored, no connection is made at this time.
 */
UsbCom::UsbCom() {
  handle_ = NULL;
  last_error_ = NULL;
}

// -------------------------------------------------------------------Destructor
/** @brief destructor, also disconnects device if one was connected*/
UsbCom::~UsbCom() {
  if (handle_) {
    hid_close(handle_);
  }
  hid_exit();
}

// -------------------------------------------------------------------last_error
/** @brief get the last usb error or NULL
 *
 * \code printf(last error: %ls", usbcominstance.last_error()); \endcode
 */
const wchar_t* UsbCom::last_error() {
  return last_error_;
}

// -----------------------------------------------------device_servo_pulsewidths
/** @brief accessor for \ref device_servo_pulsewidths_ */
const double* UsbCom::device_servo_pulsewidths() {
  return device_servo_pulsewidths_;
}

// ----------------------------------------------------------------------Connect
/** @brief connect to the first device with \ref vid_ \ref pid_
 *
 * @param vid the vendor id of the usb device to use
 * @param pid the product id
 * @retval 0 on success
 * @retval 1 if hid_init failed
 * @retval 2 if device couldn't be opened
 */
int UsbCom::Connect(uint16_t vid, uint16_t pid) {
  vid_ = vid;
  pid_ = pid;
  int init_result = hid_init();
  if (init_result) {
    handle_ = NULL;
    return 1;
  }
  handle_ = hid_open(vid_, pid_, NULL);
  if (!handle_) {
    return 2;
  }
  last_error_ = NULL;
  return 0;
}

// -----------------------------------------------------------------------handle
/** @brief accessor for \ref handle_ */
hid_device* UsbCom::handle() {
  return handle_;
}


// ---------------------------------------------------------ReadServoPulsewidths
/** @brief read the servo pulsewidths from the connected device
 *
 * On failure the last error is available in \ref last_error <br>
 * @retval 0 read was succesfull, values are stored in
 * \ref device_servo_pulsewidths_
 * @retval 1 read was not successful, device handle is still valid
 * @retval 2 no device connected
 */
int UsbCom::ReadServoPulsewidths() {
  if (!handle_) return 2;
  union {
    uint16_t value;
    uint8_t bytes[2];
  } pw;  // temp storage because we'll receive high byte of pw first

  // request pos 0..3
  wbuf_[0] = 0x00;
  wbuf_[1] = kUsbCustomGetPos0To3;
  int res = hid_write(handle_, wbuf_, sizeof(wbuf_));
  if (res < 0) {
    last_error_ = hid_error(handle_);
    return 1;
  }
  // read pos 0..3
  res = hid_read(handle_, rbuf_, sizeof(rbuf_));
  if (res < kUsbReadBufferSize) {
    last_error_ = hid_error(handle_);
    return 1;
  }
  // store them in device_servo_pulsewidths_
  for (int i = 0; i < kUsbReadBufferSize; i += 2) {
    pw.bytes[1] = rbuf_[i];
    pw.bytes[0] = rbuf_[i+1];
    device_servo_pulsewidths_[i>>1] =
      -1.0 * static_cast<double>(pw.value) / kTimeConstant + kMaxPulsewidth;
  }

  // request pos 4..7
  wbuf_[0] = 0x00;
  wbuf_[1] = kUsbCustomGetPos4To7;
  res = hid_write(handle_, wbuf_, sizeof(wbuf_));
  if (res < 0) {
    last_error_ = hid_error(handle_);
    return 1;
  }
  // read pos 4..7
  res = hid_read(handle_, rbuf_, sizeof(rbuf_));
  if (res < kUsbReadBufferSize) {
    last_error_ = hid_error(handle_);
    return 1;
  }
  for (int i = 0; i < kUsbReadBufferSize; i += 2) {
    pw.bytes[1] = rbuf_[i];
    pw.bytes[0] = rbuf_[i+1];
    device_servo_pulsewidths_[(i>>1)+4] =
      -1.0 * static_cast<double>(pw.value) / kTimeConstant + kMaxPulsewidth;
  }

  // request pos 8..11
  wbuf_[0] = 0x00;
  wbuf_[1] = kUsbCustomGetPos8To11;
  res = hid_write(handle_, wbuf_, sizeof(wbuf_));
  if (res < 0) {
    last_error_ = hid_error(handle_);
    return 1;
  }
  // read pos 8..11
  res = hid_read(handle_, rbuf_, sizeof(rbuf_));
  if (res < kUsbReadBufferSize) {
    last_error_ = hid_error(handle_);
    return 1;
  }
  for (int i = 0; i < kUsbReadBufferSize; i += 2) {
    pw.bytes[1] = rbuf_[i];
    pw.bytes[0] = rbuf_[i+1];
    device_servo_pulsewidths_[(i>>1)+8] =
      -1.0 * static_cast<double>(pw.value) / kTimeConstant + kMaxPulsewidth;
  }
  return 0;
}


// --------------------------------------------------------WriteServoPulsewidths
/** @brief write a set of servo pulsewidths to the device
 *
 * On failure the last error can be retrieved with \ref last_error
 * @param pulsewidths pointer to 12 (\ref kDeviceServoCount) values to send
 * @retval 0 success
 * @retval 1 write failure
 * @retval 2 no device connected
 * @retval 3 write succeeded but subsequential read failed
 * @retval 4 write succeeded but subsequential read returned something
 * unexpected (nothing in \ref last_error_)
 * */
int UsbCom::WriteServoPulsewidths(const double* pulsewidths) {
  if (!handle_) return 2;
  // send write command
  wbuf_[0] = 0x00;
  wbuf_[1] = 0x03;
  double sc_pw;  // servocontroller pulsewidth value
  union {
    uint16_t value;
    uint8_t buffer_values[2];
  } pw;
  for (int i = 0; i < kDeviceServoCount; ++i) {
    // wbuf_[i+2] = static_cast<uint8_t>(servo_angles[i] / K_ + kMidPW);
    sc_pw = (kMaxPulsewidth - pulsewidths[i]) * kTimeConstant;
    pw.value = static_cast<uint16_t>(round(sc_pw));
    wbuf_[i*2+2] = pw.buffer_values[1];
    wbuf_[i*2+3] = pw.buffer_values[0];
  }
  int res = hid_write(handle_, wbuf_, sizeof(wbuf_));
  if (res < 0) {
    last_error_ = hid_error(handle_);
    return 1;
  }
  // read the result
  res = hid_read(handle_, rbuf_, sizeof(rbuf_));
  if (res < 0) {
    last_error_ = hid_error(handle_);
    return 3;
  }
  if (memcmp(kUsbSuccess, rbuf_, kUsbReadBufferSize / sizeof(uint8_t))) {
    // readbuffer should containt  "success "
    return 4;
  }
  return 0;
}

// ------------------------------------------------------------------readWithCmd
/** @brief send cmd and read the returned data from the device.
 *
 * Data is stored in \ref misc_buf_
 * @return True on success
 */
bool UsbCom::ReadWithCmd(uint8_t cmd) {
  if (!handle_) return false;
  // send getdata command
  wbuf_[0] = 0x00;
  wbuf_[1] = cmd;
  int res = hid_write(handle_, wbuf_, sizeof(wbuf_));
  if (res < 0) {
    last_error_ = hid_error(handle_);
    return NULL;
  }
  // read the result
  res = hid_read(handle_, rbuf_, sizeof(rbuf_));
  if (res < 0) {
    last_error_ = hid_error(handle_);
    return NULL;
  }
  // store in misc_buf_
  if (memcmp(kUsbSuccess, rbuf_, kUsbReadBufferSize / sizeof(uint8_t))) {
    memcpy(misc_buf_, rbuf_, sizeof(rbuf_));
    return true;
  } else {
    printf("usbcom: received 'success' after request instead of data\n");
    return false;
  }
}

// -----------------------------------------------------------------ReadMiscData
/** @brief read misc data from device, returns pointer to \ref misc_buf_ 
 * or NULL on error
 */
const uint8_t* UsbCom::ReadMiscData() {
  if (ReadWithCmd(kUsbCustomGetMiscData)) {
    return misc_buf_;
  } else {
    return NULL;
  }
}

// ------------------------------------------------------------------ReadAdcData
/** @brief read adc data from device, returns pointer to \ref misc_buf_ 
 * or NULL on error
 */
const uint8_t* UsbCom::ReadAdcData() {
  if (ReadWithCmd(kUsbCustomGetAdcData)) {
    return misc_buf_;
  } else {
    return NULL;
  }
}

}  // namespace Q1

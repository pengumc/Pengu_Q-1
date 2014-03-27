/**
 * @file Usbdevice.h
 */
/* Copyright (c) 2012 Michiel van der Coelen

    This file is part of Crobot

    Crobot is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Crobot is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Crobot.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef __USBDEVICE__
#define __USBDEVICE__
#define USBCOMMANDLINE


//needed for broken usb.h
//#include <sys/types.h>
//#include <usb.h>

#include <string.h>
#include <unistd.h> //for usleep
#ifdef __WINDOWSCRAP__
	#include <windows.h>
#endif
#include "opendevice.h"
#include "usbconfig.h"
#include "i2c_header.h"
#include "requests.h"
#include "Report.h"
#include "Pscontroller.h"
#include "Accelerometer.h"
#include "Leg.h"

/** Number of times to allow a failed comminication.*/
#define USBDEV_RETRY 3

/** Timeout for communication.*/
#define USBDEV_TIMEOUT_MS 500

/** Number of legs.*/
#define USBDEV_LEGNO 4

/** Number of servos.*/
#define USBDEV_SERVOCOUNT BUFLEN_SERVO_DATA

/** Usbdevice data.*/
typedef struct{
    usb_dev_handle* handle; /**<libusb device handle.*/
    int vid; /**<vendor id.*/
    int pid; /**<product id.*/
//    char servoDataBuffer[BUFLEN_SERVO_DATA];
    char* vendor; /**< vendor string. */
    char* product; /**< product string. */
    int connected; /**<connection state.*/
	pscontroller_t pscon; /**<Playstation controller data.*/
    accelerometer_t* acc; /**<Accelerometer data.*/
    leg_t* legs[USBDEV_LEGNO]; /**<Legs with servos.*/
} usbdevice_t;

/** control message request types*/
typedef enum{
    USBDEV_READ = USB_ENDPOINT_IN,
    USBDEV_WRITE = USB_ENDPOINT_OUT
} usbdevice_reqType;

usbdevice_t* Usbdevice_alloc();
void Usbdevice_free(usbdevice_t* usbdevice);

void Usbdevice_init(usbdevice_t* usbdevice);
int Usbdevice_connect(usbdevice_t* usbdevice);
int Usbdevice_getData(usbdevice_t* usbdevice, char* buffer);
int Usbdevice_sendServoData(usbdevice_t* usbdevice);
int Usbdevice_getServoData(usbdevice_t* usbdevice, char* buffer);
int Usbdevice_loadPositions(usbdevice_t* usbdevice);
void printBuffer(char* buffer);
void nsleep(long nanoseconds);
#endif


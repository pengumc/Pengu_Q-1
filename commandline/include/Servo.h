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
/**
 * @addtogroup Servo
 * A module to to hold servo data and to handle conversions from
 * the angles used in calculations to the pulswidths that are used in the
 * quadruped.
 * 
 * notes:
 * @li Angles are in radians.
 * @li The pulsewidth should always correspond to the angle and vice versa.
 * The correct functions should be used when changing either of them
 * in stead of writing them directly. 
 * 
 * The top-view of a servo corresponds with the unit circle ( @em t is the angle here
 for some weird reason):
 * \image html "unit_circle.png"
 * 
 * An example:
 * @anchor servo_example
 * @code
 *  // Create a new servo.
 *  servo_t* myServo = Servo_alloc();
 * 
 *  // Print something about the new servo.
 *  Servo_printDetails(myServo, "myServo");
 * 
 *  //Try to change the servo angle by 1 Pi
 *  int result = Servo_changeAngle(myServo, 1 * M_PI);
 *  if(result){
     * printf("Angle successfully changed.\n");
     * printf(" Angle is now: %.2f\n", myServo->_angle);
     * printf(" Pulsewidth was automatically changed as well to: %d\n", myServo->_pw);
 *  }else{
     * printf("The pulsewidth that's needed to achieve this angle is out of bounds. Nothing was changed.\n");
 *  }
 * 
 *  //Setting the offset:
 *  Servo_setOffset(myServo, 15.4);
 *  //This action sets the angle of the servo to the new offset.
 *  //the offset value is wound down till it's within (-Pi .. Pi) by \ref Angle_normalize.
 *  
 *  //set the turning direction to backwards.
 *  //Any positive value will set the direction to forwards, anything else means backwards.
 *  Servo_setDirection(myServo, -1);
 *  //Increasing the angle will now reduce the pulsewidth.
 *  //The pulsewidth is reset to \ref SERVO_DEF_MID_PULSE
 * 
 *  //reset the servo to default values.
 *  Servo_reset(myServo);
 * 
 *  // Destroy the servo.
 *  Servo_free(myServo);
 * @endcode
 * @{
 * @file Servo.h
 */


#ifndef __SERVO__
#define __SERVO__

#include <stdlib.h>
#include <stdio.h>
#include "Angle.h"
#include "Report.h"

/** default maximum pulsewidth*/
#define SERVO_DEF_MAX_PULSE 96

/** default minimum pulsewidth*/
#define SERVO_DEF_MIN_PULSE 48

/** (min+max)/2 */
#define SERVO_DEF_MID_PULSE 72

/** default rotation direction for a servo*/
#define SERVO_DEF_DIR 1.0f

/** default servo offset*/
#define SERVO_DEF_OFFSET 0

/** default conversion factor*/
#define SERVO_DEF_K 0.034

/** Struct to hold the servo data.*/
typedef struct{
    uint8_t _pw; /**< Current pulsewidth of servo.*/
    uint8_t midPulse; /**< Neutral pulse for this servo.*/
    uint8_t minPulse; /**< Minimum pulse for this servo.*/
    uint8_t maxPulse; /**< Maximum pulse for this servo.*/
    angle_t offset; /**< physical rotation of the servo.*/
    double K; /**< pulsewidth to angle conversion factor.*/
    double direction; /**< Direction of servo rotation.*/
    angle_t _angle; /**< Current angle of servo.*/
} servo_t;


void Servo_reset(servo_t* servo);
int Servo_setAngle(servo_t* servo, angle_t value);
int Servo_setPw(servo_t* servo, uint8_t value);
angle_t Servo_convertToAngle(servo_t* servo, uint8_t pulsewidth);
uint8_t Servo_convertToPulsewidth(servo_t* servo, angle_t value);
int Servo_checkAngle(servo_t* servo, angle_t value);
void Servo_printDetails(servo_t* servo, const char* name);
void Servo_setOffset(servo_t* servo, angle_t offset);
void Servo_setDirection(servo_t* servo, int8_t direction);
int Servo_changeAngle(servo_t* servo, angle_t value);

servo_t* Servo_alloc();
void Servo_free(servo_t* servo);

#endif
/**@}*/

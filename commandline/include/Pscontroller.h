/** 
 * @file Pscontroller.h
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

#ifndef __PSCONTROLLER__
#define __PSCONTROLLER__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/** Default mid value for an analog stick.*/
#define PS_DEFAULT_ANALOG 128

/** Bit checking macro.
 * @param x The variable to check.
 * @param y The bit to check
 * 
 * Example:
 * @code
 *  CHK(0xF0, 4); 
 *  //will check the fifth bit of 0b11110000.
 *  //---------------------------------^
 * @endcode
*/
#define CHK(x,y) (x&(1<<y)) 

/** Buttons on a Playstation 1 controller.
 * And their bit position in our data.
 */
typedef enum{
	PS_SELECT = 0, /**< Select.*/
	PS_START = 3,/**<Start. */
	PS_UP = 4,/**<Up. */
	PS_RIGHT = 5,/**<Down. */
	PS_DOWN = 6,/**<Righ. */
	PS_LEFT = 7,/**<Left. */
	//substract 8 from these to get the bit numbers
	PS_L2 = 8,/**<L2. */
	PS_R2 = 9,/**<R2. */
	PS_L1 = 10,/**<L1. */
	PS_R1 = 11,/**<R1. */
	PS_TRIANGLE = 12,/**<Triangle. */
	PS_CIRCLE = 13,/**<Circle. */
	PS_CROSS = 14,/**<Cross. */
	PS_SQUARE = 15,/**<Square. */
} pscontroller_button;

/** Axis on a Playstation 1 controller.
 */
typedef enum{
	PS_RX = 0,/**<Right x-axis.*/
	PS_RY = 1,/**<Right y-axis.*/
	PS_LX = 2,/**<Left x-axis.*/
	PS_LY = 3 /**<Left y-axis.*/
} pscontroller_axis;


/** Playstation controller data.*/
typedef struct {
  uint8_t ss_dpad; /**< Start, select and dpad buttons.*/
  uint8_t shoulder_shapes; /**< Shoulder buttons and circle,
                                triangle, square, cross.*/
  uint8_t previous_ss_dpad;/**< Previous state of ss_dpad.*/
  uint8_t previous_shoulder_shapes;/**< Previous state of shoulder_shapes.*/
  uint8_t rx; /**< Right x-axis value.*/
  uint8_t ry; /**< Right y-axis value.*/
  uint8_t lx; /**< Left x-axis value.*/
  uint8_t ly; /**< Lefy y-axis value.*/
  uint8_t rx_mid; /**< Right x-axis mid value.*/
  uint8_t ry_mid; /**< Right y-axis mid value.*/
  uint8_t lx_mid; /**< Left x-axis mid value.*/
  uint8_t ly_mid; /**< Left y-axis mid value.*/
}pscontroller_t ;

pscontroller_t* Pscontroller_alloc();
void Pscontroller_init(pscontroller_t* pscon);
void Pscontroller_free(pscontroller_t* pscontroller);

void Pscontroller_saveMid(pscontroller_t* pscontroller);

int Pscontroller_isAnalogOn(pscontroller_t* pscontroller);

int Pscontroller_getButton(pscontroller_t* pscontroller,
	pscontroller_button button);

int8_t Pscontroller_getAxis(pscontroller_t* pscontroller,
	pscontroller_axis axis);

int Pscontroller_getButtonEdge(pscontroller_t* pscontroller,
	pscontroller_button button);
	
void Pscontroller_updateData(pscontroller_t* pscontroller, 
    uint8_t ss_dpad, uint8_t shoulder_shapes,
    uint8_t rx, uint8_t ry, uint8_t lx, uint8_t ly);



#endif


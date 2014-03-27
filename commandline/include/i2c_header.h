/**
 * @file i2c_header.h
 * @brief Some I2C values. Some of these are shared between firmware and 
 PC software
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



#ifndef I2C_HEADER_H
#define I2C_HEADER_H

#ifndef USBCOMMANDLINE
  #include <util/twi.h>
#endif

/** I2C address of the servocontroller.*/
#define I2C_SLAVE_ADDRESS 0x01

/** register value for writing I2C (atmega).*/
#define SLA_W ((I2C_SLAVE_ADDRESS<<1) | TW_WRITE)

/** register value for reading I2C (atmega).*/
#define SLA_R ((I2C_SLAVE_ADDRESS<<1) | TW_READ)

/** Length of the buffer that holds servo positions.*/
#define BUFLEN_SERVO_DATA 12

/** Default servo positions.*/
	#define SERVO_DATA_EMPTY {72,72,72,72,72,72,72,72,72,72,72,72}

/** ack*/
#define TWACK (TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA))

/** nack*/
#define TWNACK (TWCR=(1<<TWINT)|(1<<TWEN))

/** start*/
#define TWSTART (TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTA))

/** reset*/
#define TWRESET (TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO)|(1<<TWEA))

/** stop start*/
#define TWSTOSTA (TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTA)|(1<<TWSTO))
#endif


/**will make the servocontroller load the array stored in eeprom.*/
#define I2C_LOAD_STARTPOS 2

/**Servocontroller saves the current servopositions in EEPROM.*/
#define I2C_SAVE_STARTPOS 3


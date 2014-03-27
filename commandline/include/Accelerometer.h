/**
 * @addtogroup Accelerometer
 * @{
 * @file Accelerometer.h
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

#ifndef __ACCELEROMETER__
#define __ACCELEROMETER__

/** Number of filters to use.*/
#define ACC_FILTER_COUNT 3

/** Adc values to keep track of.*/
#define ACC_ADC_COUNT 3

/** Default mid value for an adc.*/
#define ACC_MID 128

#include <stdint.h>
#include <time.h>
#include <math.h>
#include "rotation.h"
#include "Filter.h"


/** Accelerometer data.*/
typedef struct {
    rot_vector_t* tilt_angles; /**<Holds the calculated tilt angles.*/
    filter_t* filters[ACC_FILTER_COUNT];/**<The filters.*/
    time_t lastUpdateTime; /**<Timestamp of last update. (not implemented)*/
    uint8_t adc_values[ACC_ADC_COUNT]; /**<Holds the latest adc values.*/
} accelerometer_t;

accelerometer_t* Accelerometer_alloc();
void Accelerometer_free(accelerometer_t* acc);

void Accelerometer_updateValues(accelerometer_t* acc,
    uint8_t x, uint8_t y, uint8_t z);
void _Accelerometer_calcTilt(accelerometer_t* acc);
void _Accelerometer_updateFilters(accelerometer_t* acc);
//time_t _Accelerometer_getTimeDelta(accelerometer_t* acc);

#endif
/** @}*/

/**
 * @file Filter.h
 * @brief Kalman filter. We'll keep track the last X input and output values,
 So we have those available for some nice plots.<br>
 The filter is a <a href=http://en.wikipedia.org/wiki/Kalman_filter>
 kalman filter </a>
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

#ifndef __FILTER__
#define __FILTER__

#include <stdlib.h>
#include <stdint.h>
#include <string.h> //for memset
#include "Report.h"

/** Number of data points to remember.*/
#define FILTER_GRAPH_LENGTH 300

/** Filter data.*/
typedef struct FILTER_T {
    double Sz; /**<Sz (or R cov of observation noise) parameter of the filter.*/
    double Sw; /**<Sw (or Q cov of process noise) parameter of the filter.*/
    
    double* inputData; /**<Remembered input points.*/
    double* outputData;/**<Remembered output points.*/
    uint16_t dataIndex; /**<Index of input/output Data.*/
    uint8_t dataset_changed;/**<Indication we should not free Data.*/

    double x; /**<Filter output.*/
    double _x_last; /**<Previous filter output.*/
    double _P; /**<Internal P.*/
    double _P_last; /**<Internal previous P.*/
} filter_t;

filter_t* Filter_alloc();
void Filter_free(filter_t* f);
void Filter_init(filter_t* f);

void Filter_step(filter_t* f, double input);

uint16_t Filter_addPoints(filter_t* f, double in, double out);
void Filter_addInputPoint(filter_t* f, double value);
void Filter_addOutputPoint(filter_t* f, double value);

void Filter_changeGraphPointers(filter_t* f, double* in, double* out);

/* do not want
void Filter_setSz(filter_t* f, double sz);
void Filter_setSw(filter_t* f, double sw);

double Filter_getOuput(filter_t* f);
double Filter_getSz(filter_t* f);
double Filter_getSw(filter_t* f);
*/


#endif


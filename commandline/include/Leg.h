/** 
 * @file Leg.h
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


#ifndef __LEG__
#define __LEG__

#include "rotation.h"
#include "Servo.h"
#include "Solver.h"
#include "Report.h"

/** Number of servos per leg*/
#define LEG_DOF 3

/** Leg data.*/
typedef struct{
	servo_t* servos[LEG_DOF]; /**< Servo data for the servo in this leg.*/
	rot_vector_t* servoLocations[LEG_DOF+1];/**< Location relative to servo0.*/
	rot_vector_t* offsetFromCOB; /**< The vector from COB to servo 0*/
    rot_vector_t* endpointRollback; /** Rollback coords for the last action.*/
	solver_t* legSolver; /**< The solver that will be used by this leg */
} leg_t;

leg_t* Leg_alloc();
void Leg_commitEndpointChange(leg_t* leg);
void Leg_free(leg_t* leg);
void Leg_initDefaults(leg_t* leg);
double Leg_getServoAngle(leg_t* leg, uint8_t servoNo);
uint8_t Leg_getServoPw(leg_t* leg, uint8_t servoNo);
void Leg_printDetails(leg_t* leg);
int Leg_setServoPw(leg_t* leg, uint8_t servoNo, uint8_t pw);
int Leg_tryEndpointChange(leg_t* leg, rot_vector_t* delta);
void Leg_updateServoLocations(leg_t* leg);
int Leg_changeServoAngle(leg_t* leg, uint8_t s, double value);
void Leg_setLengths(leg_t * leg, double A, double B, double C);
void Leg_resyncSolverParams(leg_t* leg);
void Leg_performRollback(leg_t* leg);
void Leg_setupRollback(leg_t* leg);

#endif


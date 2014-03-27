/**
 * @file Solver.h
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

 
#ifndef __SOLVER__
#define __SOLVER__
#include <stdlib.h>
#include <stdint.h>

#include <math.h>
#ifndef M_PI
/** defenition of PI if it's not defined in math.h*/
#define M_PI		3.14159265358979323846
/** defenition of PI/2 if it's not defined in math.h*/
#define M_PI_2		1.57079632679489661923	
#endif
#include "rotation.h"
#include "Angle.h"

/** Leg parameters to be used in the solver*/
typedef struct PARAMS{
    double A;/**< Distance between servo 0 and 1 in cm.*/
    double B;/**< Distance between servo 1 and 2 in cm.*/
    double C;/**< Distance between servo 2 and the endpoint in cm.*/
    double X;/**< Target x coordinate of the endpoint.*/
    double _X;/**< temporary x coordinate of the endpoint Only internal.*/
    double Y;/**< Target Y coordinate of the endpoint.*/
    double Z;/**< Target Z coordinate of the endpoint.*/
} solverParams_t;

typedef struct{
    solverParams_t* params; /**< Parameters to use in solving stuff*/
    rot_vector_t* lastResult; /**< Holds the last result of a solve call*/
    int8_t validLastResult; /**<indicates the last solve was succesful.*/   
} solver_t;

solver_t* Solver_alloc();
void Solver_free(solver_t* solver);

angle_t Solver_calculateAlpha(solver_t* solver);
int Solver_solve(solver_t* solver);
void Solver_changeXYZ(solver_t* solver, double X, double Y, double Z);
void Solver_changeABC(solver_t* solver, double A, double B, double C);
void Solver_setXYZ(solver_t* solver, double X, double Y, double Z);
void Solver_setABC(solver_t* solver, double A, double B, double C);
solverParams_t Solver_getParams(solver_t* solver);

#endif


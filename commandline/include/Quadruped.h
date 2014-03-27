/**
 * @file quadruped.h
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

#ifndef __QUADRUPED__
#define __QUADRUPED__


#include "Usbdevice.h"
#include "Filter.h"
#include "Pscontroller.h"

//COM-------------------------------------------
/**Servo positions.*/
typedef struct{
    double x[USBDEV_LEGNO*LEG_DOF];
    double y[USBDEV_LEGNO*LEG_DOF];
    double z[USBDEV_LEGNO*LEG_DOF];
} com_servopos_t;


/**Endpoint positions.*/
typedef struct{
    double x[USBDEV_LEGNO];
    double y[USBDEV_LEGNO];
    double z[USBDEV_LEGNO];
} com_endpoints_t;


/**Main communication struct.*/
typedef struct DATATRANSFER{
    uint8_t pulsewidths[USBDEV_LEGNO*LEG_DOF];
    double angles[USBDEV_LEGNO*LEG_DOF];
    com_servopos_t servopos;
    com_endpoints_t endpoints;
} communication_t;


typedef struct SERVOINFO{
    uint8_t pulsewidths[BUFLEN_SERVO_DATA];
    double angles[BUFLEN_SERVO_DATA];
} servoinfo_t;

//QPED----------------------------------------------------------
/**Quadruped data.*/
typedef struct QPED{
    usbdevice_t* dev; /**<The usb helper.*/
    char buffer[BUFLEN_SERVO_DATA]; /**<The usb buffer.*/
    rot_matrix_t* R;/**<rotation matrix of the main body.*/
    rot_matrix_t* invR;/**<Inverse of R.*/
    rot_vector_t* angles;/**<vector with main body angles.*/
    servoinfo_t* si;/**<servoinfo structure for communication.*/
    communication_t* com;
} quadruped_t;


quadruped_t* Quadruped_alloc();
void Quadruped_free(quadruped_t* qped);

int Quadruped_update(quadruped_t* qped);
int Quadruped_startup(quadruped_t* qped);
void Quadruped_setGraphPointers(quadruped_t* qped,
    double* inX, 
    double* inY, 
    double* inZ, 
    double* outX, 
    double* outY, 
    double* outZ);
void Quadruped_configureServoOffset(quadruped_t* qped,
    uint8_t legNo, uint8_t servoNo, angle_t offset);
void Quadruped_updateMatricesFromAngles(quadruped_t* qped, rot_vector_t* a);
int Quadruped_rotate(quadruped_t* qped, 
    double xaxis, double yaxis, double zaxis);
int Quadruped_changeLegEndpoint(quadruped_t* qped, uint8_t legNo,
    double X, double Y, double Z);
int Quadruped_changeAllEndpoints(quadruped_t* qped,
    double X, double Y, double Z);
int Quadruped_setAllEndpointZs(quadruped_t* qped, double Z);
int Quadruped_getPsAxis(quadruped_t* qped, pscontroller_axis axis);
int Quadruped_getServoData(quadruped_t* qp);
void Quadruped_updateServoinfo(quadruped_t* qp);
int Quadruped_commit(quadruped_t* qped);
servoinfo_t* Quadruped_getServoinfoPointer(quadruped_t* qp);
int Quadruped_changeSingleServo(
    quadruped_t* qp, uint8_t l, uint8_t s, double angle);
void Quadruped_configureLegLengths(quadruped_t* qped,
    uint8_t legNo, double A, double B, double C);
void Quadruped_configureLegLengths(quadruped_t* qped,
    uint8_t legNo, double A, double B, double C);
void Quadruped_debugLegs(quadruped_t* qp);
void Quadruped_enableCommunication(quadruped_t* qp, communication_t* com);
int Quadruped_loadStartPos(quadruped_t* qped);
int Quadruped_getPsButton(quadruped_t* qped, pscontroller_button button);
int Quadruped_getPsButtonEdge(quadruped_t* qped, pscontroller_button button);
int Quadruped_getPsAxis(quadruped_t* qped, pscontroller_axis axis);

//COM------------------------------------------------------
communication_t* Communication_alloc();
void Communication_free(communication_t* com);
void Communication_updatePWA(communication_t* com, quadruped_t* qp);
void Communication_updatePos(communication_t* com, quadruped_t* qp);
void Communication_updateEndpoints(communication_t* com, quadruped_t* qp);


#endif



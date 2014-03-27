/**
 * @file rotation.h
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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h> 
#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

//no changes in these 2 are supported
#define ROT_MATRIX_DIMENSION 3
#define ROT_MATRIX_SIZE ROT_MATRIX_DIMENSION * ROT_MATRIX_DIMENSION

typedef double rot_matrix_t; //either float or double
typedef double rot_vector_t;

///allocate vector
rot_vector_t *rot_vector_alloc();
///set vector value
void rot_vector_set(rot_vector_t *v, uint8_t n, rot_vector_t a);
void rot_vector_change(rot_vector_t *v, uint8_t n, rot_vector_t a);
void rot_vector_setAll(rot_vector_t *v, rot_vector_t x, rot_vector_t y, rot_vector_t z);
void rot_vector_changeAll(rot_vector_t *v, rot_vector_t a, rot_vector_t b, rot_vector_t c);
///copy
void rot_vector_copy(rot_vector_t *source, rot_vector_t *target);
///get value
rot_vector_t rot_vector_get(rot_vector_t *v, uint8_t n);
///print vector
void rot_vector_print(rot_vector_t *v);
///return vector length in xz plane;
double rot_vector_get_length_xz(rot_vector_t *v);
///return vector length in xz plane;
double rot_vector_get_length_yz(rot_vector_t *v);
///return vector length
double rot_vector_get_length(rot_vector_t *v);
///add vectors v1 = v1 + v2
void rot_vector_add(rot_vector_t *v1, rot_vector_t *v2);
///minus vectors v1=v1-v2
void rot_vector_minus(rot_vector_t *v1, rot_vector_t *v2);
///allocate matrix
rot_matrix_t *rot_matrix_alloc();
///free a vector or matrix
void rot_free(void *x);
///fill matrix with an array
int rot_matrix_fill(rot_matrix_t *M, rot_matrix_t *array, size_t size);
///print matrix
void rot_matrix_print(rot_matrix_t *M);
///scale the entire array with factor
void rot_matrix_scale(rot_matrix_t* M, double factor);
///set all values in an array to a
void rot_matrix_set_all(rot_matrix_t *M, rot_matrix_t a);
///set a value in a matrix to a
void rot_matrix_set(rot_matrix_t *M, uint8_t row, uint8_t col, rot_matrix_t a);

///do a matrix*vector = vector
void rot_matrix_dot_vector(rot_matrix_t *M, rot_vector_t *x, rot_vector_t *y);

///create a rotation matrix from the euler angles in v
void rot_matrix_build_from_angles(rot_matrix_t *M, rot_vector_t *v );

//invert a 3d matrix
void rot_matrix_invert(rot_matrix_t *M, rot_matrix_t *result);



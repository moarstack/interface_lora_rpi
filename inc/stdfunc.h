/*
 * stdfunc.h
 *
 *  Created on: 22 марта 2016 г.
 *      Author: kryvashek
 */

#ifndef STDFUNC_H_
#define STDFUNC_H_

#include <stdarg.h>
#include <stddef.h>
#include <limits.h>

#define RAND_BYTES	4

unsigned char brand( unsigned short * seed_place );

void srand( unsigned int seed );

int rand( void );

#endif // STDFUNC_H_

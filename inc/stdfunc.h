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

void * memset( void * ptr, int value, size_t num );

void * memcpy( void * destination, const void * source, size_t num );

int memcmp( const void * ptr1, const void * ptr2, size_t num );

//int DEBUGOUT( const char * format, ... );

#endif // STDFUNC_H_

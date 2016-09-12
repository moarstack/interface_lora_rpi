/*
 * stdfunc.с
 *
 *  Created on: 22 марта 2016 г.
 *     Authors: svalov, kryvashek
 */

#include "stdfunc.h"
#include "stdint.h"
#include "stdbool.h"

unsigned short reg[2];
unsigned char ibrand( unsigned short * seed_place )
{
	register unsigned short	reg;
	register unsigned char	trc = 0, res, i;

	reg = ( NULL == seed_place ? 0 : *( seed_place ) );

	trc |= ( 1 & ( ( reg >> 10 ) ^ ( reg >> 13 ) ) ) << 0;
	trc |= ( 1 & ( ( reg >> 11 ) ^ ( reg >> 12 ) ) ) << 1;
	trc |= ( 1 & ( ( reg >> 14 ) ^ ( reg >> 15 ) ) ) << 2;
	trc |= ( 1 & ( ( reg >>  0 ) ^ ( reg >>  9 ) ) ) << 3;
	trc |= ( 1 & ( ( reg >>  3 ) ^ ( reg >> 15 ) ) ) << 4;
	trc |= ( 1 & ( ( reg >>  2 ) ^ ( reg >> 12 ) ) ) << 5;
	trc |= ( 1 & ( ( reg >>  4 ) ^ ( reg >> 13 ) ) ) << 6;
	trc |= ( 1 & ( ( reg >>  1 ) ^ ( reg >> 14 ) ) ) << 7;
	res = trc;

	for( i = 0; i < 7; i++ )
		trc = ( trc >> 1 ) ^ ( trc & 1 );

	reg = ( reg >> 1 ) | ( ( unsigned short )trc << 15 );

	*( seed_place ) = reg;
	return res;
}

void isrand( unsigned int seed )
{
	reg[ 0 ] = ( unsigned short )( seed & 0xFFFF );
	reg[ 1 ] = ( unsigned short )( (seed>>16) & 0xFFFF );
}

int irand( void )
{

	register int	res = 0;
	res |= ibrand( reg ) << 24;
	res |= ibrand( reg + 1 ) << 16;
	res |= ibrand( reg ) << 8;
	res |= ibrand( reg + 1 );
	return res;
}

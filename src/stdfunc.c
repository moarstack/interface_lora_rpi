/*
 * stdfunc.с
 *
 *  Created on: 22 марта 2016 г.
 *     Authors: svalov, kryvashek
 */

#include "stdfunc.h"
#include "stdint.h"
#include "stdbool.h"
#include "board.h"
#include "board_api.h"

unsigned short reg[2];
unsigned char brand( unsigned short * seed_place )
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

void srand( unsigned int seed )
{
	reg[ 0 ] = ( unsigned short )( seed & 0xFFFF );
	reg[ 1 ] = ( unsigned short )( (seed>>16) & 0xFFFF );
}

int rand( void )
{

	register int	res = 0;
	res |= brand( reg ) << 24;
	res |= brand( reg + 1 ) << 16;
	res |= brand( reg ) << 8;
	res |= brand( reg + 1 );
	return res;
}

void * memset( void * ptr, int value, size_t num )
{
	if( NULL == ptr )
		return ptr;

	unsigned char	* _destination = ( unsigned char * )ptr,
			* _source = ( unsigned char * )&value;

	while( num-- > 0 )
		*_destination++ = *_source;

	return ptr;
}

void * memcpy( void * destination, const void * source, size_t num )
{
	if( NULL == destination || NULL == source )
		return destination;

	unsigned char	* _destination = ( unsigned char * )destination,
			* _source = ( unsigned char * )source;

	while( num-- > 0 )
		*_destination++ = *_source++;

	return destination;
}

int memcmp( const void * ptr1, const void * ptr2, size_t num )
{
	if( NULL == ptr1 && NULL == ptr2 )
		return 0;

	else if( NULL != ptr1 && NULL == ptr2 )
		return 1;

	else if( NULL == ptr1 && NULL != ptr2 )
		return -1;

	size_t			index;
	unsigned char	* _first = ( unsigned char * )ptr1,
			* _second = ( unsigned char * )ptr2;

	for( index = 1; index <= num; index++, _first++, _second++ )
		if( *_first > *_second )
			return index;
		else if( *_first < *_second )
			return -( int )index;

	return 0;
}


inline void printInt(int value){
	if(value == 0){
		Board_UARTPutChar('0');
		return;
	}
	if(value<0)
		Board_UARTPutChar('-');
	else
		value = -value; //need some magic for negatives
	int divider = -1000000000; // need moar? write function for long long
	bool first = true;
	while(divider<0){
		int val = value/divider;
		value%=divider;
		divider/=10;
		if(val!=0 || !first){
			Board_UARTPutChar('0'+val);
			first = false;
		}
	}
}
inline void printHex(int value, int8_t size, bool capital, bool zero){
	if(value==0 && !zero){
		Board_UARTPutChar('0');
		return;
	}
	char offset = (capital?'A':'a') - 10;
	size= (size - 1) * 4;
	bool first = true;
	while(size>=0){
		uint32_t val = (value>>size)&0xf;
		if((zero && val==0) || val>0 || !first){
			Board_UARTPutChar(val + (val>9?offset:'0') );
			first = false;
		}
		size-=4;
	}
}

int DEBUGOUT(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	while(*format){
		if(*format == '%'){
			//int x = va_arg (ap, int);
			format++;

			char* start = format;
			while(*format>='0' && *format<='9')
				format++;

			//check for line end
			if(*format=='d'){
				int x = va_arg (ap, int);
				printInt(x);
				format++;
				continue;
			}
			if(*format=='p'){
				int x = va_arg (ap, int);
				DEBUGSTR("0x");
				printHex(x, 8, false, true);
				format++;
				continue;
			}
			if(*format=='c'){
				int x = va_arg (ap, int);
				Board_UARTPutChar((char)x);
				format++;
				continue;
			}
			if(*format=='s'){
				char* x = va_arg (ap, char*);
				DEBUGSTR(x);
				format++;
				continue;
			}

			//possible hex
			bool hex = *format=='x';
			bool HEX = *format=='X';

			if(hex || HEX){
				int x = va_arg (ap, int);
				bool zero = (*start =='0');
				if(zero)
					start++;
				uint8_t size  = 0;
				for(;start<format;start++){
					size*=10;
					size+=*start-'0';
				}
				printHex(x, size, HEX, zero);
				format++;
				continue;
			}
		}
		Board_UARTPutChar(*format);
		format++;
	}
	va_end (ap);
	return 0;
}

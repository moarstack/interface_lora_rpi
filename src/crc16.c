/*
 * crc16.c
 *
 *  Created on: 03 февр. 2016 г.
 *      Author: svalov
 */

#include "crc16.h"

uint16_t crc16(const uint8_t* data_p, uint16_t length){
	uint8_t x;
	uint16_t crc = 0xFFFF;

    while (length--){
        x = crc >> 8 ^ *data_p++;
        x ^= x>>4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x <<5)) ^ ((uint16_t)x);
    }
    return crc;
}

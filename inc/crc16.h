/*
 * crc16.h
 *
 *  Created on: 03 февр. 2016 г.
 *      Author: svalov
 */

#ifndef CRC16_H_
#define CRC16_H_

#include "stdint.h"

uint16_t crc16(const uint8_t* data_p, uint16_t length);

#endif /* CRC16_H_ */

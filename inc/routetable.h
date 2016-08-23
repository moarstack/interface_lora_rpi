/*
 * routetable.h
 *
 *  Created on: Apr 16, 2016
 *      Author: spiralis
 */

#ifndef ROUTETABLE_H_
#define ROUTETABLE_H_

#include <stdint.h>
//#include "routing.h"

#define SzRouteDataRecord	sizeof(RouteDataRecord_T)

typedef uint32_t	RouteAddr_T;	//type for routing addresses
typedef uint16_t	RouteTableSize_T;
typedef char		RouteChance_T;

#pragma pack(push, 1)
typedef struct {
	RouteAddr_T		Relay,
					Dest;
	RouteChance_T	P;
} RouteDataRecord_T;

typedef struct{
	uint8_t TableSize;

	uint8_t FinderMarkerRenewRate;  //0 exp, 1..255 linear
	uint8_t FinderMarkerDefaultMetric;
	uint8_t RouteRenewRate; //0 exp, 1..255 linear
	uint8_t RouteDefaultMetric;
	uint8_t RouteQualityThreshold; // send finder if route less or equal
} RouteTableSettings_T;

typedef struct {
	RouteDataRecord_T	* Table;	// table itself
	RouteTableSize_T	Capacity,	// limit of records count in table
						Count;	// count of records in table
	moarTime_T			LastTimeUpdated;
	RouteTableSettings_T*  Settings;
} RouteDataTable_T;
#pragma pack(pop)

bool RouteTableInit( RouteDataTable_T * table, RouteTableSettings_T* settings );
inline RouteDataTable_T * RouteTableCreate( RouteTableSettings_T* settings );
bool RouteTableClear( RouteDataTable_T * table );
bool RouteTableDestroy( RouteDataTable_T * table );
bool RouteTableRenew( RouteDataTable_T * table, moarTime_T tick );
bool RouteTableAdd( RouteDataTable_T * table, RouteAddr_T relay, RouteAddr_T dest );
bool RouteTableDelAll( RouteDataTable_T * table, RouteAddr_T relay, RouteAddr_T dest ); // add RouteTableDelOne() TODO
inline void RouteTableUpdate( RouteDataRecord_T * row );
RouteDataRecord_T * RouteTableGetDest( RouteDataTable_T * table, RouteAddr_T relay );
RouteDataRecord_T * RouteTableGetRelayFirst( RouteDataTable_T * table, RouteAddr_T dest );
RouteDataRecord_T * RouteTableGetRelayNext( RouteDataTable_T * table, RouteDataRecord_T * row );
RouteDataRecord_T * RouteTableGetRelayBest( RouteDataTable_T * table, RouteAddr_T dest );
RouteDataRecord_T * RouteTableGetRecord( RouteDataTable_T * table, RouteAddr_T relay, RouteAddr_T dest );
bool Bump( RouteDataTable_T * table, RouteAddr_T relay, RouteChance_T newP );
inline RouteDataRecord_T * RouteTableRowFirst( RouteDataTable_T * table );
inline RouteDataRecord_T * RouteTableRowNext( RouteDataTable_T * table, RouteDataRecord_T * prevRow );
inline RouteDataRecord_T * RouteTableRowIndexed( RouteDataTable_T * table, RouteTableSize_T index );
inline RouteTableSize_T RouteTableCount( RouteDataTable_T * table );

#endif /* ROUTETABLE_H_ */

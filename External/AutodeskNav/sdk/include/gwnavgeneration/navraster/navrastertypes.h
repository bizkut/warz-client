/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavRasterTypes_H
#define Navigation_NavRasterTypes_H

#include "gwnavruntime/basesystem/logger.h"


namespace Kaim
{

typedef KyUInt16 NavRasterConnectionType;
static const NavRasterConnectionType NavRasterConnectionType_NO_TAG = 0;
static const NavRasterConnectionType NavRasterConnectionType_HOLE   = 1;
static const NavRasterConnectionType NavRasterConnectionType_WALL   = 2;
static const NavRasterConnectionType NavRasterConnectionType_STEP   = 3;
static const NavRasterConnectionType NavRasterConnectionType_CellBorder = 4;

typedef KyUInt32 PixelColor;
static const PixelColor PixelColor_Unset           = KyUInt32MAXVAL;
static const PixelColor PixelColor_IrrelevantColor = KyUInt32MAXVAL - 7; //to be compliant with boundary colors

typedef KyUInt32 NavRasterFloorIdx;
static const NavRasterFloorIdx NavRasterFloorIdx_Invalid = KyUInt32MAXVAL;

typedef KyUInt32 NavRasterCardinalDir;
static const NavRasterCardinalDir NavRasterCardinalDir_EAST       = CardinalDir_EAST /*0*/;
static const NavRasterCardinalDir NavRasterCardinalDir_NORTH      = CardinalDir_NORTH/*1*/;
static const NavRasterCardinalDir NavRasterCardinalDir_WEST       = CardinalDir_WEST /*2*/;
static const NavRasterCardinalDir NavRasterCardinalDir_SOUTH      = CardinalDir_SOUTH/*3*/;
static const NavRasterCardinalDir NavRasterCardinalDir_NORTH_EAST = 4;
static const NavRasterCardinalDir NavRasterCardinalDir_NORTH_WEST = 5;
static const NavRasterCardinalDir NavRasterCardinalDir_SOUTH_WEST = 6;
static const NavRasterCardinalDir NavRasterCardinalDir_SOUTH_EAST = 7;
KY_INLINE bool IsNavRasterCardinalDirOnDiagonal(const NavRasterCardinalDir dir) { return dir >= 4; }


// After the erosion pass, we tag the pixels that are directly adjacent to
// certain features. 
enum NavRasterFeatureMask
{
	UNDEFINED_NAVRASTER_FEATURE = 0,
	NAVRASTER_HOLE_PIXEL = 1,
	NAVRASTER_WALL_PIXEL = 2
	// next = 4/8/16 etc... 
};

}

#endif //Navigation_NavRasterTypes_H


/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_CardinalDir_H
#define Navigation_CardinalDir_H

#include "gwnavruntime/base/types.h"

namespace Kaim
{

/// Defines a type that refers to one of the cardinal points on the compass:
/// - #CardinalDir_EAST
/// - #CardinalDir_NORTH
/// - #CardinalDir_WEST
/// - #CardinalDir_SOUTH
/// - #CardinalDir_INVALID
typedef KyUInt32 CardinalDir;

static const CardinalDir CardinalDir_EAST = 0;  ///< Identifies East, defined as the positive direction of the X axis.  
static const CardinalDir CardinalDir_NORTH = 1; ///< Identifies North, defined as the positive direction of the Y axis.  
static const CardinalDir CardinalDir_WEST = 2;  ///< Identifies West, defined as the negative direction of the X axis.  
static const CardinalDir CardinalDir_SOUTH = 3; ///< Identifies South, defined as the negative direction of the Y axis.  
static const CardinalDir CardinalDir_INVALID = KyUInt32MAXVAL; ///< Identifies an invalid cardinal direction.  

/// Returns the CardinalDir that lies in the opposite direction from the specified CardinalDir.
/// For example, specifying #CardinalDir_EAST returns #CardinalDir_WEST.
/// \pre must be one of the four valid CardinalDir.
KY_INLINE CardinalDir GetOppositeCardinalDir(const CardinalDir dir) { return (dir + 2) & 3 /*% 4*/; }
KY_INLINE CardinalDir GetNextCardinalDir_CCW(const CardinalDir dir) { return (dir + 1) & 3 /*% 4*/; }
KY_INLINE CardinalDir GetNextCardinalDir_CW(const CardinalDir dir)  { return (dir + 3) & 3 /*% 4*/; }
}

#endif


/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_AbstractGraphUtils_H
#define Navigation_AbstractGraphUtils_H

#include "gwnavruntime/math/cardinaldir.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/base/types.h"

namespace Kaim
{

class ActiveData;
class NavCell;
class NavCellBlob;


class AbstractGraphUtils
{
public:
	static bool HasCellBoundaries(const NavCellBlob* navCellBlob, KyUInt8 wantedCellBoundaries);
	static void GetCellBoundaries(const NavCellBlob* navCellBlob, KyUInt8& cellBoundariesMask);
	static void GetCellBoundariesInFloor(const NavCellBlob* navCellBlob, KyUInt32 floorIdx, KyUInt8& cellBoundaries);
	static NavCell* GetNavCellFromCellPos(const CellPos& cellPos, ActiveData* activeData);

	// Determine along which axis the position will be compared :
	// CardinalDir_NORTH or CardinalDir_SOUTH are done along X axis (index = 0)
	// CardinalDir_EAST or CardinalDir_WEST are done along Y axis (index = 1)
	static KyUInt32 GetNavVertexAxisIndexFromCellBoundary(CardinalDir cellBoundaryDir);
};

}


#endif

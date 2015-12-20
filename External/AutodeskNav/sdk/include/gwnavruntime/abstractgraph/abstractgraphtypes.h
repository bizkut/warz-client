/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_AbstractGraphTypes_H
#define Navigation_AbstractGraphTypes_H

#include "gwnavruntime/containers/collection.h"
#include "gwnavruntime/navmesh/blobs/navvertex.h"
#include "gwnavruntime/base/types.h"

namespace Kaim
{

	typedef KyUInt16 CompactAbstractGraphNodeIdx;
	typedef KyUInt32 AbstractGraphNodeIdx;
	static const CompactAbstractGraphNodeIdx CompactAbstractGraphNodeIdx_Invalid = KyUInt16MAXVAL;
	static const AbstractGraphNodeIdx AbstractGraphNodeIdx_Invalid = KyUInt16MAXVAL;

	typedef KyUInt16 CompactAbstractGraphFloorIdx;
	typedef KyUInt32 AbstractGraphFloorIdx;
	static const CompactAbstractGraphFloorIdx CompactAbstractGraphFloorIdx_Invalid = KyUInt16MAXVAL;
	static const AbstractGraphFloorIdx AbstractGraphFloorIdx_Invalid = KyUInt16MAXVAL;

	typedef KyUInt16 CompactAbstractGraphCellIdx;
	typedef KyUInt32 AbstractGraphCellIdx;
	static const CompactAbstractGraphCellIdx CompactAbstractGraphCellIdx_Invalid = KyUInt16MAXVAL;
	static const AbstractGraphCellIdx AbstractGraphCellIdx_Invalid = KyUInt16MAXVAL;

	typedef KyUInt32 AbstractGraphIdx;
	static const AbstractGraphIdx AbstractGraphIdx_Invalid = CollectionInvalidIndex;

	typedef NavVertex AbstractGraphVertex;
}

#endif

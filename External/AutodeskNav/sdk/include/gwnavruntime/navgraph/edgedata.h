/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_EdgeData_H
#define Navigation_EdgeData_H

// primary contact: MAMU - secondary contact: NOBODY
#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/basesystem/logger.h"

namespace Kaim
{

/* For internal use. */
class GraphEdgeData
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	GraphEdgeData() { Clear(); }
	KY_INLINE void Clear() { }

};

} // namespace Kaim

#endif

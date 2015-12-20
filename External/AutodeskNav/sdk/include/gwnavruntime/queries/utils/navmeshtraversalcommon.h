/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavmeshTraversalCommon_H
#define Navigation_NavmeshTraversalCommon_H

#include "gwnavruntime/querysystem/workingmemcontainers/workingmemcontainerbase.h"

namespace Kaim
{

enum TraversalResult
{
	TraversalResult_DONE,
	TraversalResult_LACK_OF_MEMORY_FOR_CLOSED_NODES,
	TraversalResult_LACK_OF_MEMORY_FOR_OPEN_NODES,
	TraversalResult_LACK_OF_MEMORY_FOR_VISITED_NODES,
	TraversalResult_ERROR
};


}

#endif // Navigation_NavmeshTraversalCommon_H


/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_VertexData_H
#define Navigation_VertexData_H

#include "gwnavruntime/navgraph/navgraphtypes.h"
#include "gwnavruntime/navmesh/identifiers/navtrianglerawptr.h"

namespace Kaim
{
class NavGraphLink;

/* For internal use. */
class GraphVertexData
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)

public:
	GraphVertexData() : m_startIdxInEdgeDataArray(0), m_navGraphLink(KY_NULL) {}

public:
	KyUInt32 m_startIdxInEdgeDataArray; /*< index of the first index of the outgoingEdge in  */
	CellPos m_cellPos; // the CellPos in which the vertex has been spatialized
	NavGraphLink* m_navGraphLink;
};

}

#endif //Navigation_VertexData_H


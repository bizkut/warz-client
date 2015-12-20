/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_SpatializedPointCollectorContext_H
#define Navigation_SpatializedPointCollectorContext_H

#include "gwnavruntime/querysystem/workingmemcontainers/navfloorandnavgraphedgestatusingrid.h"
#include "gwnavruntime/querysystem/workingmemcontainers/workingmemdeque.h"
#include "gwnavruntime/querysystem/workingmemcontainers/workingmemarray.h"

namespace Kaim
{

class CollectorTraversalNodeIdx
{
public:
	CollectorTraversalNodeIdx() : m_data(KyUInt32MAXVAL) {}
	CollectorTraversalNodeIdx(const NavFloorRawPtr& /*navFloorRawPtr*/, KyUInt32 nodeIndex) : m_data(0x80000000 | nodeIndex) {}
	CollectorTraversalNodeIdx(const NavGraphEdgeRawPtr& /*navGraphEdgeRawPtr*/, KyUInt32 nodeIndex) : m_data(0x7FFFFFFF & nodeIndex) {}

	KY_INLINE bool IsNavFloorNode()     const { return (m_data & 0x80000000) != 0; }
	KY_INLINE bool IsNavGraphEdgeNode() const { return (m_data & 0x80000000) == 0; }
	KY_INLINE KyUInt32 GetNodeIndex()   const { return (m_data & 0x7FFFFFFF);      }

	KyUInt32 m_data; // 1 bit for type, 31 bit for index
};

class SpatializedPointCollectorContext
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_QueryWorkingMem)
public:
	SpatializedPointCollectorContext() {}
	~SpatializedPointCollectorContext() { ReleaseWorkingMemory(); }

	void ReleaseWorkingMemory()
	{
		m_traversalNodeStatus.ReleaseWorkingMemoryBuffer();
		m_openNodes.ReleaseWorkingMemoryBuffer();
		m_edgeRawPtrNodes.ReleaseWorkingMemoryBuffer();
		m_navFloorRawPtrNodes.ReleaseWorkingMemoryBuffer();
		m_resultCollection.ReleaseWorkingMemoryBuffer();
	}
public:
	NavFloorAndNavGraphEdgeStatusInGrid m_traversalNodeStatus;
	WorkingMemDeque<CollectorTraversalNodeIdx> m_openNodes; //< open nodes = nodes that are about to be analysed
	WorkingMemArray<NavGraphEdgeRawPtr> m_edgeRawPtrNodes;
	WorkingMemArray<NavFloorRawPtr> m_navFloorRawPtrNodes;
	WorkingMemArray<SpatializedPoint*> m_resultCollection;
};




}


#endif //Navigation_SpatializedPointCollectorContext_H


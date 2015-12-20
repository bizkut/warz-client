/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_GeneratorConnectComponentTraversal_H
#define Navigation_GeneratorConnectComponentTraversal_H

#include "gwnavruntime/base/memory.h"

#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/navmesh/identifiers/navhalfedgerawptr.h"

#include "gwnavgeneration/generator/generatornavdatafilter.h"
#include "gwnavruntime/containers/sharedpoollist.h"
#include "gwnavruntime/database/navtagptr.h"

namespace Kaim
{

class NavMeshElementBlob;

// Implementation detail of PdgNavDataFilter
class GeneratorConnectComponentTraversal
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public :
	GeneratorConnectComponentTraversal() :
		m_openNodesPool(Memory::pGlobalHeap, MemStat_NavDataGen, PoolChunkSize::SlotCount, 64),
		m_openNodes(&m_openNodesPool),
		m_currentComponentId(KyUInt32MAXVAL)
	{}

	typedef NavTagPtr ConnexPtr;
public :
	void Clear() { m_openNodes.Clear(); }

	void Search();

	KY_INLINE void AddStartNavConnex(const ConnexPtr& navConnextPtr)
	{
		SetConnectedComponentId(navConnextPtr, m_currentComponentId);
		m_openNodes.PushBack(navConnextPtr);
	}

	KY_INLINE void SetCurrentComponentId(ConnectedComponentId currentComponentId)
	{
		m_currentComponentId = currentComponentId;
	}

	ConnectedComponentId GetConnectedComponentId(const ConnexPtr& navConnextPtr)
	{
		return navConnextPtr.m_navFloorPtr->GetConnexConnectedComponentId(navConnextPtr.m_navTagIdx);
	}

	void SetConnectedComponentId(const ConnexPtr& navConnextPtr, ConnectedComponentId currentComponentId)
	{
		navConnextPtr.m_navFloorPtr->SetConnexConnectedComponentId(navConnextPtr.m_navTagIdx, currentComponentId);
	}
private:

	KY_INLINE void OpenNodeIfNew(const ConnexPtr& navConnextPtr)
	{
		ConnectedComponentId connectedComponentId = GetConnectedComponentId(navConnextPtr);

		if (connectedComponentId == ConnectedComponent_UNDEFINED)
		{
			SetConnectedComponentId(navConnextPtr, m_currentComponentId);
			m_openNodes.PushBack(navConnextPtr);
		}
	}

protected:
	SharedPoolList<ConnexPtr>::NodePool m_openNodesPool;
	SharedPoolList<ConnexPtr> m_openNodes;
	ConnectedComponentId m_currentComponentId;
};


inline void GeneratorConnectComponentTraversal::Search()
{
	ConnexPtr currentConnexPtr;
	SharedPoolList<ConnexPtr>::Iterator firstIterator = m_openNodes.GetFirst();
	while (m_openNodes.IsEmpty() == false)
	{
		currentConnexPtr = *firstIterator;
		m_openNodes.Erase(firstIterator);

		NavFloor* navFloor = currentConnexPtr.m_navFloorPtr;
		const NavFloorBlob* navFloorBlob = navFloor->GetNavFloorBlob();
		const KyUInt32 triangleCount = navFloorBlob->GetNavTriangleCount();
		const CompactNavConnexIdx* connexIdxBuffer = navFloorBlob->m_triangleConnexIndices.GetValues();
		for(NavTriangleIdx triangleIdx = 0; triangleIdx < triangleCount; ++triangleIdx)
		{
			if (connexIdxBuffer[triangleIdx] != currentConnexPtr.m_navTagIdx)
				continue;

			NavHalfEdgeIdx firstEdgeIdx = 3*triangleIdx;

			for(KyUInt32 i = 0; i < 3; ++i)
			{
				NavHalfEdgeRawPtr edge(navFloor, firstEdgeIdx + i);
				NavHalfEdgeRawPtr pairEdge;
				if (edge.IsHalfEdgeCrossable(pairEdge) == false)
					continue;
				NavTrianglePtr pairTriangle(pairEdge.GetNavFloor(), NavFloorBlob::NavHalfEdgeIdxToTriangleIdx(pairEdge.GetHalfEdgeIdx()));

				OpenNodeIfNew(ConnexPtr(pairTriangle));
			}
		}

		firstIterator = m_openNodes.GetFirst();
	}
}



}

#endif // Navigation_GeneratorConnectComponentTraversal_H


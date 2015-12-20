/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/




// ---------- Primary contact: JUBA - secondary contact: LAPA
#ifndef Navigation_NavGraph_H
#define Navigation_NavGraph_H

#include "gwnavruntime/navgraph/blobs/navgraphblob.h"
#include "gwnavruntime/navgraph/vertexdata.h"
#include "gwnavruntime/navgraph/edgedata.h"

namespace Kaim
{

class ConnectedNavGraph;
class NavGraphMemoryManager;
class NavData;

/// This class is a runtime wrapper of a NavGraphBlob, it gathers all the runtime information associated to a NavFloor
/// such links to other NavFloors, SpatializedPoint spatialized in this NavFloor...
class NavGraph
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
	KY_CLASS_WITHOUT_COPY(NavGraph)
	KY_REFCOUNT_MALLOC_FREE(NavGraph)

public:

	void OnRemove();
	bool IsStillLoaded() const;
	bool IsActive()      const;

public:
	const NavGraphEdgeInfo& GetNavGraphEdgeInfo(const NavGraphEdgeSmartIdx& edgeIdInGraph) const;
	const NavGraphBlob* GetNavGraphBlob() const;

	void SetIdxInTheActiveDataBuffer(NavGraphIdxInActiveData idxInTheBufferOfStitchedGraph);
	NavGraphIdxInActiveData GetIdxInTheActiveDataBuffer() const;

	GraphVertexData& GetGraphVertexData(NavGraphVertexIdx vertexIdx) const;
	GraphEdgeData& GetEdgeData(NavGraphEdgeSmartIdx navGraphEdgeSmartIdx) const;

	KyUInt32 GetIdxInTheBufferOfNavGraph() const { return GetIndexInCollection(); }

	KyUInt32 GetIndexInCollection() const { return m_indexInCollection; }
	void SetIndexInCollection(KyUInt32 indexInCollection) { m_indexInCollection = indexInCollection; }

	void ComputeAllCellPosOfVerticesAndCellBox();

	static KyUInt32 ComputeTotalSizeForNavGraph(const NavGraphBlob* navGraphBlob);
	static Ptr<NavGraph> Create(const NavGraphBlob* navGraphBlob, Database* database, MemoryHeap* pheap = 0);
private:
	void DebugCheckIsStillLoaded() const;
public:
	Database* m_database;
	KyUInt32 m_indexInCollection;
	CellBox m_cellBox; // the cellBox of all the vertices in the Database
	NavGraphIdxInActiveData m_idxInTheActiveDataBuffer;
	const NavGraphBlob* m_navGraphBlob;
	NavTag* m_navTags; // the navTag presents in the NavFloorBlob are copied in the navGraph.

	GraphVertexData* m_vertexDatas;
	GraphEdgeData* m_edgeDatas;

	NavData* m_navData; // For visual debug purpose
};

KY_INLINE void NavGraph::DebugCheckIsStillLoaded() const
{
	KY_LOG_ERROR_IF(IsStillLoaded() == false,("You Cannot call this function if graph has been unloaded"));
}

KY_INLINE const NavGraphBlob* NavGraph::GetNavGraphBlob() const
{
	DebugCheckIsStillLoaded();
	return m_navGraphBlob;
}

KY_INLINE void NavGraph::SetIdxInTheActiveDataBuffer(NavGraphIdxInActiveData idxInTheBufferOfStitchedGraph)
{
	DebugCheckIsStillLoaded();
	m_idxInTheActiveDataBuffer = idxInTheBufferOfStitchedGraph;
}
KY_INLINE NavGraphIdxInActiveData NavGraph::GetIdxInTheActiveDataBuffer() const
{
	DebugCheckIsStillLoaded();
	return m_idxInTheActiveDataBuffer;
}

KY_INLINE GraphVertexData& NavGraph::GetGraphVertexData(NavGraphVertexIdx vertexIdx) const
{
	DebugCheckIsStillLoaded();
	return m_vertexDatas[vertexIdx];
}
KY_INLINE GraphEdgeData& NavGraph::GetEdgeData(NavGraphEdgeSmartIdx navGraphEdgeSmartIdx) const
{
	DebugCheckIsStillLoaded();
	return m_edgeDatas[m_vertexDatas[navGraphEdgeSmartIdx.GetStartVertexIdx()].m_startIdxInEdgeDataArray + navGraphEdgeSmartIdx.GetNeighborVertexIdx()];
}

KY_INLINE bool NavGraph::IsActive()      const { return GetIdxInTheActiveDataBuffer() != NavGraphIdxInActiveData_Invalid; }
KY_INLINE bool NavGraph::IsStillLoaded() const { return m_navGraphBlob != KY_NULL; }
KY_INLINE void NavGraph::OnRemove()            { m_navGraphBlob = KY_NULL; }


}


#endif //Navigation_NavGraph_H


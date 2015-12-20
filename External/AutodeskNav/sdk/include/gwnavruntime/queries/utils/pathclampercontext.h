/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_PathClamperContext_H
#define Navigation_PathClamperContext_H

#include "gwnavruntime/querysystem/workingmemcontainers/navdatachangeindexingrid.h"
#include "gwnavruntime/queries/raycangoquery.h"
#include "gwnavruntime/navgraph/identifiers/navgraphedgeptr.h"

namespace Kaim
{

typedef KyUInt16 ClampNodeIndex;
static const ClampNodeIndex ClampNodeIndex_Invalid = KyUInt16MAXVAL;

class ClampNode
{
public:
	ClampNode();
	ClampNode(const Vec3f& pos, NodeTypeAndRawPtrDataIdx nodeTypeAndRawPtrDataIdx, ClampNodeIndex predecessorIdx, ClampNodeIndex nextNodeIdx);
	ClampNode(const Vec3f& pos, const WorldIntegerPos& integerPos, NodeTypeAndRawPtrDataIdx nodeTypeAndRawPtrDataIdx, ClampNodeIndex predecessorIdx, ClampNodeIndex nextNodeIdx);

	PathNodeType GetNodeType()    const;
	KyUInt32 GetIdxOfRawPtrData() const;

	void SetNodeType(PathNodeType nodeType);
	void SetIdxOfRawPtrData(KyUInt32 indexOfRawPtrData);

	Vec3f m_nodePosition;
	WorldIntegerPos m_nodeIntegerPos;
	NodeTypeAndRawPtrDataIdx m_nodeTypeAndRawPtrDataIdx; // 32 bits
	ClampNodeIndex m_predecessorNodeIdx; // 16 bits
	ClampNodeIndex m_nextNodeIdx; // 16bits
};

class PathRefinerContext;
class BaseRayCanGoQuery;
class Channel;

enum ClampResult
{
	ClampResult_SUCCESS,
	ClampResult_FAIL_MEMORYLIMIT,
	ClampResult_FAIL_CANGOHIT
};

class PathClamperContext
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_QueryWorkingMem)
public:

	enum PathClamperStatus
	{
		Initialisation,
		NeedToComputeIntersections,
		ProcessingIntersections
	};

	PathClamperContext() : m_clamperStatus(Initialisation), m_currentClampNodeIdx(ClampNodeIndex_Invalid), m_currentIntersectionLastIndex(KyUInt32MAXVAL) {}
	~PathClamperContext() { ReleaseWorkingMemory(); }

	void ReleaseWorkingMemory()
	{
		m_triangleRawPtrNodes.ReleaseWorkingMemoryBuffer();
		m_vertexRawPtrNodes.ReleaseWorkingMemoryBuffer();
		m_clampNodes.ReleaseWorkingMemoryBuffer();
		m_currentDestNavTrianglePtr.Invalidate();
		m_clamperStatus = Initialisation;
		m_currentClampNodeIdx = ClampNodeIndex_Invalid;
		m_currentIntersectionLastIndex = KyUInt32MAXVAL;
		m_startNavGraphEdgePtr = NavGraphEdgePtr();
		m_destNavGraphEdgePtr = NavGraphEdgePtr();
	}

	KyResult InitFromRefinerContext(WorkingMemory* workingMemory, PathRefinerContext* pathRefinerContext);
	KyResult InitFromCanGo(WorkingMemory* workingMemory, BaseRayCanGoQuery& baseRayCanGoQuery);
	KyResult InitFromChannel(Database* database, WorkingMemory* workingMemory, const Channel& channel);

	bool IsClampingDone() const { return m_currentClampNodeIdx == 0; } // node of index 0 is the destination node

	WorkingMemArray<ClampNode>            m_clampNodes;
	WorkingMemArray<NavTriangleRawPtr>    m_triangleRawPtrNodes;
	WorkingMemArray<NavGraphVertexRawPtr> m_vertexRawPtrNodes;

	PathClamperStatus m_clamperStatus;
	ClampNodeIndex m_currentClampNodeIdx;
	KyUInt32 m_currentIntersectionLastIndex;
	NavTrianglePtr m_currentDestNavTrianglePtr;
	Ptr<QueryDynamicOutput> m_queryDynamicOutput;
	NavGraphEdgePtr   m_startNavGraphEdgePtr;
	NavGraphEdgePtr   m_destNavGraphEdgePtr;

public : // Internal
	Vec3f m_currentStartPos;
	NavTriangleRawPtr m_currentStartTriangleRawPtr;
	WorldIntegerPos m_currentStartIntegerPos;
	KyUInt16 m_currentVertexIdx;
	KyUInt16 m_currentIdxInTriangleBuffer;
};




KY_INLINE ClampNode::ClampNode() :
m_predecessorNodeIdx(ClampNodeIndex_Invalid),
	m_nextNodeIdx(ClampNodeIndex_Invalid)
{}

KY_INLINE ClampNode::ClampNode(const Vec3f& pos, NodeTypeAndRawPtrDataIdx nodeTypeAndRawPtrDataIdx, ClampNodeIndex predecessorIdx, ClampNodeIndex nextNodeIdx) :
	m_nodePosition(pos),
	m_nodeTypeAndRawPtrDataIdx(nodeTypeAndRawPtrDataIdx),
	m_predecessorNodeIdx(predecessorIdx),
	m_nextNodeIdx(nextNodeIdx)
{}
KY_INLINE ClampNode::ClampNode(const Vec3f& pos, const WorldIntegerPos& integerPos, NodeTypeAndRawPtrDataIdx nodeTypeAndRawPtrDataIdx, ClampNodeIndex predecessorIdx, ClampNodeIndex nextNodeIdx) :
	m_nodePosition(pos),
	m_nodeIntegerPos(integerPos),
	m_nodeTypeAndRawPtrDataIdx(nodeTypeAndRawPtrDataIdx),
	m_predecessorNodeIdx(predecessorIdx),
	m_nextNodeIdx(nextNodeIdx)
{}

KY_INLINE PathNodeType ClampNode::GetNodeType()    const { return m_nodeTypeAndRawPtrDataIdx.GetNodeType(); }
KY_INLINE KyUInt32 ClampNode::GetIdxOfRawPtrData() const { return m_nodeTypeAndRawPtrDataIdx.GetIdxOfRawPtrData(); }

KY_INLINE void ClampNode::SetNodeType(PathNodeType nodeType) { m_nodeTypeAndRawPtrDataIdx.SetNodeType(nodeType); }
KY_INLINE void ClampNode::SetIdxOfRawPtrData(KyUInt32 indexOfRawPtrData) { m_nodeTypeAndRawPtrDataIdx.SetIdxOfRawPtrData(indexOfRawPtrData); }

}


#endif //Navigation_PathClamperContext_H


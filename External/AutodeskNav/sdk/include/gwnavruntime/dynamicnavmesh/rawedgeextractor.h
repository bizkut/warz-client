/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_RawEdgeExtractor_H
#define Navigation_RawEdgeExtractor_H

// primary contact: LASI - secondary contact: NONE

#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/database/navtag.h"
#include "gwnavruntime/dynamicnavmesh/dynamicnavmeshquerytypes.h"
#include "gwnavruntime/kernel/SF_RefCount.h"

namespace Kaim
{

class WorkingMemory;
class DynamicNavMeshQuery;
template <class T>
class WorkingMemArray;
class WorkingMemBitField;
class NavFloorBlob;
class NavVertex;

class RawEdgeExtractor
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public: 
	RawEdgeExtractor(DynamicNavMeshQuery* query) : 
	m_query(query) {}

public:
	KyResult AssignTagVolumesToCurrentFloor(WorkingMemory* workingMemory);
	KyResult ExtractSortedNavtags(WorkingMemory* workingMemory);
	KyResult ExtractCurrentTagVolumeContour(WorkingMemory* workingMemory);
	KyResult ExtractNavFloorContoursAndSortAllInputEdge(WorkingMemory* workingMemory);

private:
	KyResult FindHoleContour(WorkingMemory* workingMemory, NavHalfEdgeIdx firstHalfEdgeIdxOfContour,
		WorkingMemArray<NavHalfEdgeIdx>& edgeIndiceOfContour, WorkingMemBitField& relevantClippingEdges, WorkingMemBitField& obstacleEdgesInHoles);

	KyResult FindConnexContour(WorkingMemory* workingMemory, NavHalfEdgeIdx firstHalfEdgeIdxOfContour,
		WorkingMemArray<NavHalfEdgeIdx>& edgeIndiceOfContour, WorkingMemBitField& relevantClippingEdges);

	KyResult FindFullyInsideConnexContour(WorkingMemory* workingMemory, NavHalfEdgeIdx firstHalfEdgeIdxOfContour
		, WorkingMemArray<NavHalfEdgeIdx>& edgeIndiceOfContour, WorkingMemBitField& relevantClippingEdges);

	PolygonWinding ComputeWinding(const WorkingMemArray<NavHalfEdgeIdx>& edgeIndiceOfContour, KyUInt32 lowerLeftStarEdgeIdxInContour,
		const KyUInt32 contourEdgeCount, const NavVertex& lowerLeftPos, const NavFloorBlob* staticFloorBlob);

	void SetInputEdge(WorkingMemArray<InputEdge>& rawInputEdges, KyUInt32 inputEdgeIdx, KyUInt32 ownerIdx, KyUInt32 edgeOrigin,
		KyUInt32 navTagIdx, const Vec2i& coordStart, const Vec2i& coordEnd, KyUInt32 stitch1To1EdgeIdx = KyUInt32MAXVAL);

private: 
	// input
	DynamicNavMeshQuery* m_query;

	KyUInt32 m_currentConnexContourIdx;
	KyUInt32 m_currentHoleContourIdx;
};

}

#endif //Navigation_RawEdgeExtractor_H

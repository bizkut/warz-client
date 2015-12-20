/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef KAIM_EDGE_INTERSECTOR_H
#define KAIM_EDGE_INTERSECTOR_H

// primary contact: LASI - secondary contact: NONE

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/math/vec2i.h"
#include "gwnavruntime/math/vec3i.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/visualsystem/visualcolor.h"
#include "gwnavruntime/dynamicnavmesh/dynamicnavmeshquerytypes.h"



namespace Kaim
{
// For Debug render
class ScopedDisplayList;
class VisualColor;
class WorkingMemory;
class WorkingMemBitField;
template <class T>
class WorkingMemArray;

//----------------------------------------------------------------------
// Compute all the intersections between all the edges passed as input.
// The intersection points will be snapped to an integer grid and injected as "Cuts" on the original edges supplied.
// The snapping is done in a way that prevents spurious intersections that normally result from such process.
// This approach is similar to:
//----------------------------------------------------------------
// Hobby, J. - 1993
// Practical segment intersection with finite precision output.
// Technical Report 93/2-27, Bell Laboratories (Lucent Technologies)
//------------------------------------------------------------------

class EdgeIntersector
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	EdgeIntersector() : m_debugDisplayListBeforeSnap(KY_NULL), m_debugDisplayListAfterSnap(KY_NULL){}
public:
	KyResult ComputeIntersections(WorkingMemory* workingMemory);

	static bool Intersects_grid_rounded(const Vec2i& hotPoint, const Vec2i& start, const Vec2i& end);
private:
	KyResult ComputeIntersections_Pass1(WorkingMemory* workingMemory, const WorkingMemArray<InputEdge>& rawEdges,
		const WorkingMemArray<Box2i>& edgeBoundingBoxes, WorkingMemArray<Vec2i>& points);

	void ComputeIntersections_Pass2(const WorkingMemArray<InputEdge>& rawEdges,
		const WorkingMemArray<Box2i>& edgeBoundingBoxes, const WorkingMemArray<Vec2i>& points);

	void ComputeIntersectionsOuputEdgePieces(const WorkingMemArray<InputEdge>& rawEdges,
		const WorkingMemArray<Vec2i>& points, KyArray<InputEdgePiece>& intersectedEdges);

	void InitEdges(const KyArray<InputEdge>& rawEdges);

	void ComputeBoundingBoxesOfInputEdges(const WorkingMemArray<InputEdge>& rawEdges, WorkingMemArray<Box2i>& edgeBoundingBoxes);

	void ComputeBoundingBoxesIntersection(const WorkingMemArray<InputEdge>& rawEdges, const WorkingMemArray<Box2i>& edgeBoundingBoxes,
		WorkingMemBitField& boxIntersect);

	KyResult SortHotPointAndRemoveDuplicate(WorkingMemory* workingMemory, const WorkingMemArray<InputEdge>& rawEdges,
		WorkingMemArray<IndexedPos>& hotPoints, WorkingMemArray<Vec2i>& uniqueHotPoints);

	void SortCutList(const WorkingMemArray<InputEdge>& rawEdges, const WorkingMemArray<Vec2i>& points);

	KyArray<KyArray<KyUInt32> > m_cuts; // an array of hotPoint Idx

public: // debug check and render
	void IntersectionsToDisplayList(const WorkingMemArray<InputEdge>& rawEdges, const WorkingMemArray<Vec2i>& hotPoints,
		ScopedDisplayList& displayList, VisualColor color = VisualColor::White);

	// DEBUG Render using ScopedDisplayList
	KyResult CheckResult(const WorkingMemArray<InputEdge>& rawEdges, const WorkingMemArray<Vec2i>& hotPoints, ScopedDisplayList* displayList);
	// DEBUG display settings
	// displayListBeforeSnap and displayListAfterSnap pointers are use to distinguish what we want to display
	void SetDebugDisplayLists(KyFloat32 rasterPrecision,const Vec2i& cellOffset, ScopedDisplayList* displayListBeforeSnap, ScopedDisplayList* displayListAfterSnap);

	ScopedDisplayList* m_debugDisplayListBeforeSnap;
	ScopedDisplayList* m_debugDisplayListAfterSnap;
	KyFloat32 m_rasterPrecision;
	Vec2i m_cellOffset;
	
};

} // namespace Kaim



#endif // KAIM_EDGE_INTERSECTOR_H

/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_NavFloorSweepline_H
#define Navigation_NavFloorSweepline_H

#include "gwnavruntime/math/vec2i.h"
#include "gwnavruntime/dynamicnavmesh/navfloorsweeplinetypes.h"

#include "gwnavruntime/math/integersegmentutils.h"

namespace Kaim
{
class DynamicNavTag;
class ScopedDisplayList;

// This object is used to sweep a plane containing edges belonging to polygons and determine
// what's left and right of each edge in terms of polygons.
// This algorithm is based on boost polygon approach (http://www.boost.org/doc/libs/1_47_0/libs/polygon/doc/index.htm)
// and vatti clipping algorithm (http://en.wikipedia.org/wiki/Vatti_clipping_algorithm)
// In order to work properly this algorithm requires that:
// 1) Input edges DO NOT intersect except at their endpoints

class NavFloorSweepline
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	typedef SharedPoolList<ScanDataElement>::Iterator ScanDataIterator;
	typedef SharedPoolList<SweeplineProperty> PropertyList;
	typedef Pool<PropertyList>::Key                   PropertyListKey;

public:
	NavFloorSweepline(KyArray<InputEdgePiece>& input, SweepLineOutput& result);
	~NavFloorSweepline();

	KyResult Scan();
	void SetDebugDisplayList(KyFloat32 rasterPrecision, const Vec2i& cellOffset, ScopedDisplayList* displayList)
	{
		m_rasterPrecision = rasterPrecision;
		m_cellOriginOffset = Vec3i(cellOffset.x, cellOffset.y, 0);
		m_displayList = displayList;
	}

	void MergePropertyMaps(PropertyList& mp1, const PropertyList& mp2);
	void UpdatePropertyList(PropertyList& propertyList, SweeplineProperty& prop_data);
	KyUInt32 ComputeNavTagIndexFromPropertyList(const PropertyList& propertySet);
	bool AreListsEquals(const PropertyList& list1, const PropertyList& list2);
	void TransferContent(PropertyList& list1, PropertyList& list2, bool clearList2 = true);

private:
	void EraseEndEvent(SharedPoolList<Vec2i>::Iterator epqi);
	void RemoveRetiredEdges();
	
	void InsertSortedInScandata(const ScanDataElement& elem);
	void InsertSortedIfUniqueInEndPointQueue(const Vec2i& elem);
	ScanDataIterator LowerBoundInScandata(const EdgePoints& refElem);

	void HandleInputEvents(KyUInt32& currentInputIdx);
	void InsertAllTheEdgesOfInsertionSetInScanDataListAndEndPointQueue();
	void WriteOutEdge(const InputEdgePiece& he, const PropertyList& pm_left, const PropertyList& pm_right);

	void VisualDebugWriteOutEdges(const InputEdgePiece &he, const PropertyList &pm_left, const PropertyList &pm_right);

	void FilterOutlineEdge(const InputEdgePiece& edge, const PropertyList& left, const PropertyList& right);
	bool IsEdgeFullyInsideHole(const PropertyList& leftSet, const PropertyList& rightSet);
	bool IsEdgeInsideFloor(const PropertyList& propertySet);
	bool IsHalfEdgeInHole(const PropertyList& propertySet);
	void DebugRender_Scan();


private:
	KyArray<InputEdgePiece>* m_input;
	SweepLineOutput* m_result;

	Pool<PropertyList> m_propertyListPool;         /// Pool To allocate PropertyList
	KyArray<PropertyListKey> m_propertyListKeys;   /// temp for fast clear() of m_propertyPool
	PropertyList::NodePool m_propertyListNodePool; /// One pool unsed by for all the sharedPoolListed allocated in m_propertyListPool

	SharedPoolList<ScanDataElement> m_scanData;
	SharedPoolList<ScanDataElement>::NodePool m_scanDataElementPool;

	KyInt32 m_currentX;
	KyInt32 m_justBefore;

	SharedPoolList<Vec2i> m_endpointQueue;
	SharedPoolList<Vec2i>::NodePool m_endPointPool;

	SharedPoolList<ScanDataElement> m_insertionSet; //edge to be inserted after current NavFloorSweepline stop

	KyArray<ScanDataElement> m_insertion_elements; //edge to be inserted after current NavFloorSweepline stop

	SharedPoolList<EdgePoints> m_removalSet;
	SharedPoolList<EdgePoints>::NodePool m_removalSetPool;

	// For debug draw
	KyFloat32 m_rasterPrecision;
	Vec3i m_cellOriginOffset; // retrieved from input when Scan() is called. Used for debug Render
	ScopedDisplayList* m_displayList;
};

} // namespace Kaim

#endif // Navigation_NavFloorSweepline_H

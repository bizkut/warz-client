/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_NavFloorSweeplineTypes_H
#define Navigation_NavFloorSweeplineTypes_H


#include <assert.h>
#include "gwnavruntime/math/integersegmentutils.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/math/vec2i.h"
#include "gwnavruntime/dynamicnavmesh/dynamicnavmeshquerytypes.h"
#include "gwnavruntime/database/navtag.h"
#include "gwnavruntime/kernel/SF_Alg.h"


namespace Kaim
{

class SweepLineOutputEdgePiece
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	SweepLineOutputEdgePiece(): m_correspondingRawInputEdgeIdx(KyUInt32MAXVAL), m_navTagIndex(KyUInt32MAXVAL), m_userData(KY_NULL) {}
	SweepLineOutputEdgePiece(const EdgePoints& subSegment, const InputEdge& parentEdge) :
	m_rawInputEdgePiece(subSegment, parentEdge), m_correspondingRawInputEdgeIdx(KyUInt32MAXVAL), m_navTagIndex(KyUInt32MAXVAL), m_userData(KY_NULL) {}

	const InputEdgeProperty& ParentProperty() const { return m_rawInputEdgePiece.ParentProperty(); }
	bool IsVertical()               const { return m_rawInputEdgePiece.IsVertical();       }
	KyInt32 GetCount()              const { return m_rawInputEdgePiece.GetCount();         }
	const Vec2i& GetEdgeStart()     const { return m_rawInputEdgePiece.GetEdgeStart();     }
	const Vec2i& GetEdgeEnd()       const { return m_rawInputEdgePiece.GetEdgeEnd();       }
	const Vec2i& GetOriginalStart() const { return m_rawInputEdgePiece.GetOriginalStart(); }
	const Vec2i& GetOriginalEnd()   const { return m_rawInputEdgePiece.GetOriginalEnd();   }

public:
	InputEdgePiece m_rawInputEdgePiece;
	KyUInt32 m_correspondingRawInputEdgeIdx;
	KyUInt32 m_navTagIndex;
	mutable void* m_userData;
};

class SweepLineOutput
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_CLASS_WITHOUT_COPY(SweepLineOutput)
public:
	SweepLineOutput() {}

	inline void Insert(InputEdgePiece& elem, KyUInt32 navTagIdx)
	{
		SweepLineOutputEdgePiece& newElem = m_rawResults.PushDefault();
		newElem.m_rawInputEdgePiece = elem;
		
		newElem.m_correspondingRawInputEdgeIdx = elem.m_index;
		newElem.m_rawInputEdgePiece.m_index = m_rawResults.GetCount() - 1;
		newElem.m_navTagIndex = navTagIdx;
	
		const Vec2i& edgeStart = elem.m_extremities.m_start;
		const Vec2i& edgeEnd = elem.m_extremities.m_end;

		// Make sure we don't have edges outside of the cell box (0-255 max)
		KY_ASSERT(edgeEnd.x   >= 0 &&   edgeEnd.x <= KyUInt8MAXVAL && edgeEnd.y   >= 0 &&   edgeEnd.y <= KyUInt8MAXVAL);
		KY_ASSERT(edgeStart.x >= 0 && edgeStart.x <= KyUInt8MAXVAL && edgeStart.y >= 0 && edgeStart.y <= KyUInt8MAXVAL);

		if (edgeEnd < edgeStart)
		{
			//swap
			Alg::Swap(newElem.m_rawInputEdgePiece.m_extremities.m_start, newElem.m_rawInputEdgePiece.m_extremities.m_end);
			newElem.m_rawInputEdgePiece.m_count *= -1;
		}
		
		KyUInt32 thisNavTagIndexPos = 0;
		for (; thisNavTagIndexPos < m_navTagIndexes.GetCount(); ++thisNavTagIndexPos)
		{
			if(m_navTagIndexes[thisNavTagIndexPos] == navTagIdx)
			{
				m_elemByNavTag[thisNavTagIndexPos].PushBack(newElem.m_rawInputEdgePiece.m_index);
				return;
			}
		}

		KY_ASSERT(thisNavTagIndexPos == m_navTagIndexes.GetCount());
		m_navTagIndexes.PushBack(navTagIdx);
		m_elemByNavTag.PushDefault().PushBack(newElem.m_rawInputEdgePiece.m_index);
		KY_ASSERT(m_elemByNavTag.GetCount() == m_navTagIndexes.GetCount());
	}

	void Clear()
	{
		m_rawResults.Clear();
		m_navTagIndexes.Clear();
		m_elemByNavTag.Clear();
	}
public:
	KyArray<SweepLineOutputEdgePiece> m_rawResults;
	KyArray<KyUInt32> m_navTagIndexes;          // m_elemByNavTag.GetCount() == m_navTagIndexes.GetCount()
	KyArray<KyArray<KyUInt32> > m_elemByNavTag; // m_elemByNavTag.GetCount() == m_navTagIndexes.GetCount(), an array idx or MergerRawInputEdgePiece in m_rawResults per navTagIndexes
};


struct SweeplineProperty
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	SweeplineProperty() : m_halfEdge(KY_NULL), m_count(0) {}
	SweeplineProperty(const InputEdgePiece& he) : m_halfEdge(&he), m_count(he.GetCount()) {}

	bool operator <(const SweeplineProperty& rhs)  const { return m_halfEdge->ParentProperty() <  rhs.m_halfEdge->ParentProperty(); }

public:
	const InputEdgePiece* m_halfEdge;
	KyInt32 m_count;
};

static inline bool ArePropertiesFromSameContour(const SweeplineProperty& prop1, const SweeplineProperty& prop2)
{ return prop1.m_halfEdge->ParentProperty() == prop2.m_halfEdge->ParentProperty(); }

class ScanDataElement
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	ScanDataElement() : m_edge(KY_NULL), m_propertyList(KY_NULL) {}
	ScanDataElement(const InputEdgePiece& edge, SharedPoolList<SweeplineProperty>* properties) : m_edge(&edge), m_propertyList(properties) {}

public:
	const InputEdgePiece* m_edge;
	SharedPoolList<SweeplineProperty>* m_propertyList;
};

} //namespace Kaim

#endif // Navigation_NavFloorSweeplineTypes_H

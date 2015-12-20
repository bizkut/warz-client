/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavTagPtr_H
#define Navigation_NavTagPtr_H

#include "gwnavruntime/navmesh/navfloor.h"
#include "gwnavruntime/navgraph/navgraph.h"
#include "gwnavruntime/database/navtag.h"
#include "gwnavruntime/navmesh/identifiers/navtriangleptr.h"
#include "gwnavruntime/navgraph/identifiers/navgraphedgeptr.h"
#include "gwnavruntime/navgraph/identifiers/navgraphvertexptr.h"

namespace Kaim
{

/// Each instance of this class uniquely identifies a single NavFloor.
/// Instances of this class can be safely stored and used in subsequent frames.
class NavTagPtr
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)

public:
	// ---------------------------------- Constructors ----------------------------------

	/// Creates an invalid NavTagPtr.
	NavTagPtr();

	/// Creates a NavTagPtr initialized with the provided NavTrianglePtr relevant information.
	NavTagPtr(const NavTrianglePtr& navTrianglePtr);

	/// Creates a NavTagPtr initialized with the provided NavGraphEdgePtr relevant information.
	NavTagPtr(const NavGraphEdgePtr& navGraphEdgePtr);

	/// Creates a NavTagPtr initialized with the provided NavGraphVertexPtr relevant information.
	NavTagPtr(const NavGraphVertexPtr& navGraphVertexPtr);


	// ---------------------------------- Main API Functions ----------------------------------

	/// \returns true if this object refers to a valid NavFloor. 
	bool IsValid() const;

	/// Invalidates this object. 
	void Invalidate();

	/// \returns true if this object identifies the same NavFloor as navFloorPtr, or if both are invalid. 
	bool operator==(const NavTagPtr& rhs) const;

	/// \returns true if this object identifies a different NavFloor from navFloorPtr. 
	bool operator!=(const NavTagPtr& rhs) const;

	const NavTag* GetNavTag() const
	{
		if(IsValid() == false)
			return KY_NULL;

		if(m_navFloorPtr != KY_NULL)
			return &m_navFloorPtr->m_navTags[m_navTagIdx];
		else
			return &m_navGraphPtr->m_navTags[m_navTagIdx];
	}

public: //internal
	mutable Ptr<NavFloor> m_navFloorPtr;
	mutable Ptr<NavGraph> m_navGraphPtr;
	NavTagIdx m_navTagIdx;
};

KY_INLINE NavTagPtr::NavTagPtr() : m_navTagIdx(NavTagIdx_Invalid) {}
KY_INLINE NavTagPtr::NavTagPtr(const NavTrianglePtr& navTrianglePtr)
{
	if (navTrianglePtr.IsValid())
	{
		m_navFloorPtr = navTrianglePtr.GetNavFloor();
		m_navTagIdx = navTrianglePtr.GetNavFloorBlob_Unsafe()->NavTriangleIdxToNavConnexIdx(navTrianglePtr.GetTriangleIdx());
	}
}
KY_INLINE NavTagPtr::NavTagPtr(const NavGraphEdgePtr& navGraphEdgePtr)
{
	if (navGraphEdgePtr.IsValid())
	{
		m_navGraphPtr = navGraphEdgePtr.GetNavGraph();
		m_navTagIdx = navGraphEdgePtr.GetNavGraphBlob_UnSafe()->GetNavGraphEdgeNavTagIdx(navGraphEdgePtr.m_edgeSmartIdx);
	}
}
KY_INLINE NavTagPtr::NavTagPtr(const NavGraphVertexPtr& navGraphVertexPtr)
{
	if (navGraphVertexPtr.IsValid())
	{
		m_navGraphPtr = navGraphVertexPtr.GetNavGraph();
		m_navTagIdx = navGraphVertexPtr.GetNavGraphBlob_UnSafe()->GetNavGraphVertexNavTagIdx(navGraphVertexPtr.GetNavGraphVertexIdx());
	}
}

KY_INLINE void NavTagPtr::Invalidate() { m_navFloorPtr = KY_NULL; m_navGraphPtr = KY_NULL; m_navTagIdx = NavTagIdx_Invalid; }
KY_INLINE bool NavTagPtr::IsValid() const { return (m_navFloorPtr != KY_NULL || m_navGraphPtr != KY_NULL) && m_navTagIdx != NavTagIdx_Invalid; }

KY_INLINE bool NavTagPtr::operator==(const NavTagPtr& rhs) const { return m_navFloorPtr == rhs.m_navFloorPtr && m_navGraphPtr == rhs.m_navGraphPtr && m_navTagIdx == rhs.m_navTagIdx; }
KY_INLINE bool NavTagPtr::operator!=(const NavTagPtr& rhs) const { return m_navFloorPtr != rhs.m_navFloorPtr || m_navGraphPtr != rhs.m_navGraphPtr || m_navTagIdx != rhs.m_navTagIdx; }

}

#endif //Navigation_NavTagPtr_H

